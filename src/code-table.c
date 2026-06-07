#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/code-table.h"
#include "../include/seenset.h"
#include "../include/tags.h"
#include "../include/value.h"

Offset codetable_new(Arena *a, u32 length, Offset elements[]) {
    size_t elements_size = length * sizeof(Offset);
    CodeTable *codetable = arena_alloc(
        a,
        sizeof(CodeTable) + elements_size,
        alignof(CodeTable)
    );
    codetable->tag = TAG_CODE_TABLE;
    codetable->length = length;
    memcpy(codetable->elements, elements, elements_size);
    return (Offset)((unsigned char *)codetable - a->bytes);
}

CodeTable *codetable_resolve(Arena *a, Offset offset) {
    assert(offset <= ARENA_SIZE - sizeof(CodeTable));
    return (CodeTable *)(a->bytes + offset);
}

bool codetable_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    CodeTable *codetable = codetable_resolve(a, offset);

    Offset total_size
        = sizeof(CodeTable) + codetable->length * sizeof(Offset);
    if (offset + total_size > ARENA_SIZE) {
        return false;
    }

    for (u32 i = 0; i < codetable->length; i++) {
        Offset item_offset = codetable->elements[i];
        if (item_offset == UNSET) {
            return false;
        }
        if (value_tag(a, item_offset) != TAG_CODE_UNIT) {
            return false;
        }
        if (!generic_validate(a, item_offset, seenset)) {
            return false;
        }
    }

    return true;
}

