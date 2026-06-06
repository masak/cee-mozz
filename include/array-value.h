#ifndef ARRAY_VALUE_H
#define ARRAY_VALUE_H

#include <stdalign.h>
#include <stdbool.h>

#include "arena.h"
#include "typedefs.h"
#include "value.h"

typedef struct {
    u64 tag;
    u64 length;
    Offset elements_offset;
} ArrayValue;

typedef struct {
    u64 tag;
    u64 capacity;
    Offset elements[];
} ArrayElements;

Offset array_new(Arena *a, u64 capacity);
ArrayValue *array_resolve(Arena *a, Offset offset);
bool array_validate(Arena *a, Offset offset);

void array_push(Arena *a, Offset array_offset, Offset value_offset);
Offset array_get(Arena *a, Offset array_offset, Offset index_offset);
void array_set(
    Arena *a,
    Offset array_offset,
    Offset index_offset,
    Offset value_offset
);
Offset array_length(Arena *a, Offset array_offset);
Offset array_concat(Arena *a, Offset array_offset1, Offset array_offset2);

Offset array_elements_new(Arena *a, u64 capacity);
ArrayElements *array_elements_resolve(Arena *a, Offset offset);

#endif

