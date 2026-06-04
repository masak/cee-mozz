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
    size_t elements_offset;
} ArrayValue;

typedef struct {
    u64 tag;
    u64 capacity;
    size_t elements[];
} ArrayElements;

size_t array_new(Arena *a, u64 capacity);
ArrayValue *array_resolve(Arena *a, size_t offset);
bool array_validate(Arena *a, size_t offset);

void array_push(Arena *a, size_t array_offset, size_t value_offset);
size_t array_get(Arena *a, size_t array_offset, size_t index_offset);
void array_set(
    Arena *a,
    size_t array_offset,
    size_t index_offset,
    size_t value_offset
);
size_t array_length(Arena *a, size_t array_offset);
size_t array_concat(Arena *a, size_t array_offset1, size_t array_offset2);

size_t array_elements_new(Arena *a, u64 capacity);
ArrayElements *array_elements_resolve(Arena *a, size_t offset);

#endif

