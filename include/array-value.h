#ifndef ARRAY_VALUE_H
#define ARRAY_VALUE_H

#include <stdalign.h>
#include <stdbool.h>

#include "arena.h"
#include "outcome.h"
#include "seenset.h"
#include "typedefs.h"
#include "value.h"

typedef struct {
    Tag tag;
    u32 length;
    Offset elements_offset;
} ArrayValue;

typedef struct {
    Tag tag;
    u32 capacity;
    MaybeOffset elements[];
} ArrayElements;

Offset array_new(Arena *a, u32 capacity);
ArrayValue *array_resolve(Arena *a, Offset offset);
bool array_validate(Arena *a, Offset offset, SeenSet *seenset);

Outcome array_push(Arena *a, Offset array_offset, Offset value_offset);
Outcome array_get(
    Arena *a,
    Offset array_offset,
    Offset index_offset,
    Offset *out_offset
);
Outcome array_set(
    Arena *a,
    Offset array_offset,
    Offset index_offset,
    Offset value_offset
);
Outcome array_length(Arena *a, Offset array_offset, Offset *out_offset);
Outcome array_concat(
    Arena *a,
    Offset array_offset1,
    Offset array_offset2,
    Offset *out_offset
);

Offset array_elements_new(Arena *a, u32 capacity);
ArrayElements *array_elements_resolve(Arena *a, Offset offset);
bool array_elements_validate(Arena *a, Offset offset, SeenSet *seenset);

#endif

