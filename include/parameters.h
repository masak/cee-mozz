#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdbool.h>

#include "arena.h"
#include "seenset.h"
#include "typedefs.h"
#include "value.h"

typedef struct {
    Tag tag;
    u32 parameter_count;
    Offset parameter_names[];
} Parameters;

Offset parameters_new(Arena *a, u32 parameter_count, u32 parameter_names[]);
Parameters *parameters_resolve(Arena *a, Offset offset);
bool parameters_validate(Arena *a, Offset offset, SeenSet *seenset);

#endif

