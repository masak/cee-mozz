#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/crash.h"
#include "../include/generic-integer.h"
#include "../include/int-table.h"
#include "../include/seenset.h"
#include "../include/tags.h"
#include "../include/value.h"

/* Allocate a new IntTable.
 *
 * Precondition: All the `elements` are integers. */
Offset inttable_new(Arena *a, u32 length, Offset elements[]) {
    for (u32 i = 0; i < length; i++) {
        Offset item_offset = elements[i];
        if (!is_generic_integer(a, item_offset)) {
            vm_crash(CRASH_INVALID_TAG);
        }
    }

    size_t elements_size = length * sizeof(Offset);
    IntTable *inttable = arena_alloc(
        a,
        sizeof(IntTable) + elements_size,
        alignof(IntTable)
    );
    inttable->tag = TAG_INT_TABLE;
    inttable->length = length;
    memcpy(inttable->elements, elements, elements_size);
    return (Offset)((unsigned char *)inttable - a->bytes);
}

/* Return a pointer to an IntTable, given an offset into an arena.
 *
 * Precondition: `offset` points to an IntTable.
 */
IntTable *inttable_resolve(Arena *a, Offset offset) {
    if (value_tag(a, offset) != TAG_INT_TABLE) {
        vm_crash(CRASH_INVALID_TAG);
    }

    assert(offset <= ARENA_SIZE - sizeof(IntTable));
    return (IntTable *)(a->bytes + offset);
}

bool inttable_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    IntTable *inttable = inttable_resolve(a, offset);

    if (inttable->length > 0xFFFF) {
        return false;
    }

    Offset total_size = sizeof(IntTable) + inttable->length * sizeof(Offset);
    if (offset + total_size > ARENA_SIZE) {
        return false;
    }

    for (u32 i = 0; i < inttable->length; i++) {
        Offset item_offset = inttable->elements[i];
        if (item_offset == UNSET) {
            return false;
        }
        if (!is_generic_integer(a, item_offset)) {
            return false;
        }
        if (!generic_validate(a, item_offset, seenset)) {
            return false;
        }
    }

    return true;
}

/* Return an entry from the IntTable at `offset`, at the index `index`.
 *
 * Precondition: `offset` points to a valid IntTable.
 * Additional expectation: `index` is within bounds; that is, it falls
 *                         within the range 0 ..^ N, N being the number of
 *                         entries in the IntTable.
 * Postcondition: the returned entry is an integer.
 */
Offset inttable_get(Arena *a, Offset offset, u32 index) {
    IntTable *inttable = inttable_resolve(a, offset);
    if (index >= inttable->length) {
        vm_crash(CRASH_OUT_OF_BOUNDS);
    }
    return inttable->elements[index];
}

