#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <stdbool.h>

#include "arena.h"
#include "typedefs.h"

typedef struct {
    u64 writable;
    size_t cell;
} EnvEntry;

typedef struct {
    u64 tag;
    size_t outer_env_offset;
    u64 entry_count;
    EnvEntry entries[];
} Environment;

size_t environment_new(
    Arena *a,
    size_t outer_env_offset,
    u64 entry_count,
    EnvEntry entries[]
);
Environment *environment_resolve(Arena *a, size_t offset);
bool environment_validate(Arena *a, size_t offset);

size_t environment_load(Arena *a, size_t env_offset, u64 entry_index);
void environment_store(
    Arena *a,
    size_t env_offset,
    size_t value_offset,
    u64 outer_steps,
    u64 entry_index,
    size_t fallback_offset
);

#endif

