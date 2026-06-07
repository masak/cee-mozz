#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/environment.h"
#include "../include/seenset.h"
#include "../include/typedefs.h"
#include "../include/tags.h"
#include "../include/value.h"

Offset environment_new(
    Arena *a,
    MaybeOffset outer_env_offset,
    u64 entry_count,
    EnvEntry entries[]
) {
    size_t entries_size = entry_count * sizeof(EnvEntry);
    Environment *environment = arena_alloc(
        a,
        sizeof(Environment) + entries_size,
        alignof(Environment)
    );
    environment->tag = TAG_ENVIRONMENT;
    environment->outer_env_offset = outer_env_offset;
    environment->entry_count = entry_count;
    memcpy(environment->entries, entries, entries_size);

    return (Offset)((unsigned char *)environment - a->bytes);
}

Environment *environment_resolve(Arena *a, Offset offset) {
    assert(offset <= ARENA_SIZE - sizeof(Environment));
    return (Environment *)(a->bytes + offset);
}

bool environment_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    Environment *environment = environment_resolve(a, offset);

    MaybeOffset outer_env_offset = environment->outer_env_offset;
    if (outer_env_offset != UNSET) {
        if (value_tag(a, outer_env_offset) != TAG_ENVIRONMENT) {
            return false;
        }
        if (!environment_validate(a, outer_env_offset, seenset)) {
            return false;
        }
    }

    for (u64 i = 0; i < environment->entry_count; i++) {
        MaybeOffset cell = environment->entries[i].cell;
        if (cell == UNSET) {
            continue;
        }
        if (!generic_validate(a, cell, seenset)) {
            return false;
        }
    }

    return true;
}

