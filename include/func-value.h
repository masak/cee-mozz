#ifndef FUNC_VALUE_H
#define FUNC_VALUE_H

#include <stdbool.h>

#include "arena.h"
#include "typedefs.h"

typedef struct {
    u64 tag;
    size_t env_offset;
    size_t codeunit_offset;
} FuncValue;

size_t func_new(Arena *a, size_t env_offset, size_t codeunit_offset);
FuncValue *func_resolve(Arena *a, size_t offset);
bool func_validate(Arena *a, size_t offset);

#endif

