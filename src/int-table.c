#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/int-table.h"
#include "../include/seenset.h"
#include "../include/tags.h"
#include "../include/value.h"

Offset inttable_new(Arena *a, u32 length, Offset elements[]) {
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

IntTable *inttable_resolve(Arena *a, Offset offset) {
    assert(offset <= ARENA_SIZE - sizeof(IntTable));
    return (IntTable *)(a->bytes + offset);
}

bool inttable_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    IntTable *inttable = inttable_resolve(a, offset);

    Offset total_size = sizeof(IntTable) + inttable->length * sizeof(Offset);
    if (offset + total_size > ARENA_SIZE) {
        return false;
    }

    for (u32 i = 0; i < inttable->length; i++) {
        Offset item_offset = inttable->elements[i];
        if (item_offset == UNSET) {
            return false;
        }
        /* extend this with IntValue later */
        if (value_tag(a, item_offset) != TAG_I64) {
            return false;
        }
        if (!generic_validate(a, item_offset, seenset)) {
            return false;
        }
    }

    return true;
}

Offset inttable_get(Arena *a, Offset offset, u32 index) {
    IntTable *inttable = inttable_resolve(a, offset);
    assert(index < inttable->length);
    return inttable->elements[index];
}

