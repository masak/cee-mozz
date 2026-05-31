# Environments

Environments were tricky to get right in the data model, so here is a summary
of how they work, and how they fit together with both the instruction design
and the execution.

A program has a well-defined and unchanging tree of nested blocks. Each block
records the variable names declared in this block; in Mozzarella, this
information is a _set_ of identifiers (each one either a string or a unique
symbol). In cee-mozz, we map this set down to a set of offsets 0..N-1, so the
static shape of a block is just the _length_ N of variable bindings.

At runtime, when execution enters a block, we'll want to create an
_environment_ to hold the runtime variable bindings for the execution of that
block. The environment has N entries, where N is the length of that block's
variable bindings.

Why does this matter at all, especially since environments are clearly
second-class values? (Here, "second-class" means they cannot be stored in a
variable or a data structure, or passed as a parameter to a routine, or
returned as a return value from a routine.) It matters because environments do
get captured by first-class values, namely functions and macros:

```mozzarella
my setX;
my getX;

func f() {
    my x;

    func sx(v) {
        x = v;
    }
    setX = sx;

    func gx() {
        return x;
    }
    getX = gx;
}

f();
setX(42);
print(getX());      # 42
```

The environment inside the function `f` which contains the `x` binding was
captured by both `sx`/`setX` and `gx`/`getX`. This is the environment that
stores the 42 value betweem the penultimate and final line of the code.

The `setX` and `getX` variables are scoped to the whole program; even though
the environment in question gets created for execution inside the function `f`,
and this function finishes quickly and releases its own "claim" on the
environment it created, the environment itself lives on (and cannot be
immediately garbage-collected) because it was used as the outer environment
for the two FuncValues for `sx` and `gx`. This is what it means for a function
to "close over" its surrounding environment.

Because functions and macros capture environments when they are created as
values, and because these values _are_ first-class, environments get an
indefinite lifetime; in particular, they can survive past the execution leaving
the block in which they were created.

Two adjustments remain. The first one is that in Mozzarella, environments are
created on block entry, but in cee-mozz environments are created on procedure
entry. What used to be multiple nested environments in a procedure is now
instead one single flattened environment.

The second adjustment is that we want to keep environments small. The maximal
approach of including all bindings is obviously safe and correct, but leads to
unnecessarily big environments. At minimum, we enter only those variables into
an environment that are either read from or written to in a nested procedure.

Applied to the example above, we have:

* Top-level environment: `setX`, `getX`
* Function `f`'s environment: `x`
* Not saved into any environment: `f`, `sx`, `gx`

Each function still has plenty of registers, and we would find all of the above
variables stored in registers, even if they are not in an environment. In this
sense, environments become specifically storage for variables whose reach
exceeds a single procedure.

