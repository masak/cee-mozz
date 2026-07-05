#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/crash.h"
#include "../include/seenset.h"
#include "../include/str-table.h"
#include "../include/tags.h"
#include "../include/value.h"

Offset strtable_new(Arena *a, u32 length, Offset elements[]) {
    size_t elements_size = length * sizeof(Offset);
    StrTable *strtable = arena_alloc(
        a,
        sizeof(StrTable) + elements_size,
        alignof(StrTable)
    );
    strtable->tag = TAG_STR_TABLE;
    strtable->length = length;
    /* XXX: Should loop through all elements and `vm_crash` if any one of them
            isn't a string. This is required to guarantee the postcondition
            in `strtable_get` later. */
    memcpy(strtable->elements, elements, elements_size);
    return (Offset)((unsigned char *)strtable - a->bytes);
}

/* Returns a pointer to a StrTable, given an offset into an arena.
 *
 * Precondition: `offset` points to a StrTable.
 */
StrTable *strtable_resolve(Arena *a, Offset offset) {
    if (value_tag(a, offset) != TAG_STR_TABLE) {
        vm_crash(CRASH_INVALID_TAG);
    }

    assert(offset <= ARENA_SIZE - sizeof(StrTable));
    return (StrTable *)(a->bytes + offset);
}

bool strtable_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    StrTable *strtable = strtable_resolve(a, offset);

    if (strtable->length > 0xFFFF) {
        return false;
    }

    Offset total_size = sizeof(StrTable) + strtable->length * sizeof(Offset);
    if (offset + total_size > ARENA_SIZE) {
        return false;
    }

    for (u32 i = 0; i < strtable->length; i++) {
        Offset item_offset = strtable->elements[i];
        if (item_offset == UNSET) {
            return false;
        }
        u32 tag = value_tag(a, item_offset);
        /* extend this with SmallStrValue later */
        if (tag != TAG_ASCII_STR && tag != TAG_STR) {
            return false;
        }
        if (!generic_validate(a, item_offset, seenset)) {
            return false;
        }
    }

    return true;
}

/* Return an entry from the StrTable at `offset`, at the index `index`.
 *
 * Precondition: `offset` points to a valid StrTable.
 * Additional expectation: `index` is within bounds; that is, it falls
 *                         within the range 0 ..^ N, N being the number of
 *                         entries in the StrTable.
 * Postcondition: the returned entry is a string.
 */
Offset strtable_get(Arena *a, Offset offset, u32 index) {
    StrTable *strtable = strtable_resolve(a, offset);
    if (index >= strtable->length) {
        vm_crash(CRASH_OUT_OF_BOUNDS);
    }
    return strtable->elements[index];
}

