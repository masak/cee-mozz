#ifndef FUNC_VALUE_H
#define FUNC_VALUE_H

#include <stdbool.h>

#include "arena.h"
#include "typedefs.h"

typedef struct {
    u64 tag;
    Offset env_offset;
    Offset codeunit_offset;
} FuncValue;

Offset func_new(Arena *a, Offset env_offset, Offset codeunit_offset);
FuncValue *func_resolve(Arena *a, Offset offset);
bool func_validate(Arena *a, Offset offset);

#endif

