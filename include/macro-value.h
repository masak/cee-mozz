#ifndef MACRO_VALUE_H
#define MACRO_VALUE_H

#include <stdbool.h>

#include "arena.h"
#include "seenset.h"
#include "typedefs.h"

typedef struct {
    Tag tag;
    Offset env_offset;
    Offset codeunit_offset;
} MacroValue;

Offset macro_new(Arena *a, Offset env_offset, Offset codeunit_offset);
MacroValue *macro_resolve(Arena *a, Offset offset);
bool macro_validate(Arena *a, Offset offset, SeenSet *seenset);

#endif

