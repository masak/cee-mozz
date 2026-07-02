#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/arena.h"
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
    memcpy(strtable->elements, elements, elements_size);
    return (Offset)((unsigned char *)strtable - a->bytes);
}

StrTable *strtable_resolve(Arena *a, Offset offset) {
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
        /* extend this with StrValue and SmallStrValue later */
        if (value_tag(a, item_offset) != TAG_ASCII_STR) {
            return false;
        }
        if (!generic_validate(a, item_offset, seenset)) {
            return false;
        }
    }

    return true;
}

Offset strtable_get(Arena *a, Offset offset, u32 index) {
    StrTable *strtable = strtable_resolve(a, offset);
    assert(index < strtable->length);
    return strtable->elements[index];
}

