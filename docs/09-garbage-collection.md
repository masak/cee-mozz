# Mark-and-compact garbage collection

The cee-mozz runtime stores all values in a single contiguous arena. The arena
is a bump allocator: `arena_alloc` advances a `used` cursor and never reclaims
memory. For long-running programs, the 16 kB arena exhausts quickly. Garbage
collection (GC) reclaims unreachable values, compacts live values to the start
of the arena, and resets the `used` cursor.

This document describes a **stop-the-world, mark-and-compact** collector.
"Stop-the-world" means all interpreter execution is paused during collection.
The phases "mark", "compact", and "fixup" are described further below.

## The root set

The collector begins by identifying **roots**: offsets that the runtime treats
as inherently reachable. The mark phase ends up marking everything that's
transitively reachable from the roots.

Each active call frame in the interpreter's stack holds:

* The `Offset` of the routine (`FuncValue` or `MacroValue`) being executed.

* The `Offset` of the environment created for the call.

* An array of register slots, each holding a `MaybeOffset`.

Every non-`UNSET` offset in every frame is a root. The routine value being a
root ensures that we'll also mark the routine's environment and code unit, as
well as things reachable from these.

## The mark phase

Starting from every root, the value graph is traversed by visiting all values
reachable from a visited value. (This traversal could be depth-first,
breadth-first, or some other order.) Immediately upon visiting a value, it is
marked (which means it will survive the collection, although possibly at a
lower offset). The algorithm uses a `SeenSet` (a bitset over arena offsets) to
record the marked values.

For each offset `o` that we visit:

1. If `seen(seenset, o)`, do nothing.

2. `seenset_add(seenset, o)`.

3. Using the tag at `o` to get the type and layout of the value.

4. Based on this information, recursively visit all the offsets directly
   contained in the value.

The recursive step is type-specific. An `I64Value` contains no offsets. An
`ArrayValue` points to an `ArrayElements`, which contains `MaybeOffset`
elements. An `Environment` points to an (optional) outer `Environment` and
contains `MaybeOffset` cells. A `FuncValue` points to an `Environment` and a
`CodeUnit`. A `CodeUnit` points to a string name, `Parameters`, and optional
`IntTable`, `StrTable`, and `CodeTable` values, each of which contains further
offsets.

The full set of recursive rules mirrors the structure of `generic_validate`.

## The compact phase

### Forwarding offsets

After marking, the `SeenSet` bitset tells us exactly which offsets are live.
The compact phase builds a **forwarding table**: for each live offset `o`,
`forward_get(fwtbl, o)` is the new offset where the value will be moved.

The forwarding table maps each live old offset to its new offset. Because
compaction scans the arena in increasing order, entries are added in strictly
increasing order of old offset. We exploit this with a sorted array of `(old,
new)` pairs, grown geometrically. Insertion is amortized O(1); lookup is O(log
n) via binary search. Space is `2 * n * sizeof(Offset)` for `n` live values,
plus at most 2 times transient overallocation. We initialize this table with
a capacity of 1024 entries (8 kB), which is typically enough to not need
doubling.

The compact phase scans the arena from low to high. For each live value at
offset `o`:

1. Compute `size = generic_size(a, o)`, the total bytes occupied by this value
   (header + payload).

2. Call `forward_add(fwtbl, o, next_free)`.

3. Set `next_free += size`.

4. `next_free` is aligned to `max_align_t` after each value, matching the
   alignment guarantees of `arena_alloc`.

Values are moved in place during this scan:
`memmove(&a->bytes[forward_get(fwtbl, o)], &a->bytes[o], size)`. Because the
scan proceeds from low to high and `forward_get(fwtbl, o) <= o` always holds
(compaction squeezes values toward lower offsets), `memmove` with overlapping
regions is safe. We do not need a separate copy buffer.

### Computing value size

We declare a new function `generic_size(Arena *a, Offset o)` that dispatches
to type-specific `X_size` functions for computing the size of a value.

## The fixup phase

After compaction, every live value has moved to a new offset. All offsets
&mdash; in values in the arena, all roots, including in all register banks
&mdash; must be updated via the forwarding table.

### Fixing roots

For each root offset `o` (from the same enumeration used in marking), replace
it with `forward_get(fwtbl, o)`. This includes:

* Every register slot in every call frame.

* The routine value offset in every call frame.

* The environment offset in every call frame.

### Fixing value-internal offsets

Scan the arena from `0` to `next_free` (the new `used` value). At each value
boundary, read the tag and apply tag-specific fixup rules. To step to the next
value, compute `size = generic_size(a, current)`, apply fixups, then `current
+= size` (aligned).

## Collector invocation

Collection triggers when `arena_alloc` cannot satisfy a request: when the
aligned allocation would exceed `ARENA_SIZE`. At that point, the interpreter
calls `gc_collect()`, which performs mark-compact-fixup, and then retries the
allocation. If the allocation still fails (the value is larger than the arena,
or too many values are live), the runtime reports an out-of-memory error.

