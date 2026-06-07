#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <stdbool.h>

#include "arena.h"
#include "seenset.h"
#include "typedefs.h"

typedef struct {
    u64 writable;
    MaybeOffset cell;
} EnvEntry;

typedef struct {
    u64 tag;
    MaybeOffset outer_env_offset;
    u64 entry_count;
    EnvEntry entries[];
} Environment;

Offset environment_new(
    Arena *a,
    MaybeOffset outer_env_offset,
    u64 entry_count,
    EnvEntry entries[]
);
Environment *environment_resolve(Arena *a, Offset offset);
bool environment_validate(Arena *a, Offset offset, SeenSet *seenset);

Offset environment_load(Arena *a, Offset env_offset, u64 entry_index);
void environment_store(
    Arena *a,
    Offset env_offset,
    Offset value_offset,
    u64 outer_steps,
    u64 entry_index,
    Offset fallback_offset
);

#endif

