#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/array-value.h"
#include "../include/arena.h"
#include "../include/i64-value.h"
#include "../include/types.h"

// typedef struct {
//     u64 tag;
//     u64 length;
//     size_t elements_offset;
// } ArrayValue;
// 
// typedef struct {
//     u64 tag;
//     u64 capacity;
//     size_t elements[];
// } ArrayElements;

size_t array_new(Arena *a, u64 capacity) {
    size_t elements_offset = array_elements_new(a, capacity);
    ArrayValue *array_value = arena_alloc(
        a,
        sizeof(ArrayValue),
        alignof(ArrayValue)
    );
    array_value->tag = TAG_ARRAY;
    array_value->length = 0;
    array_value->elements_offset = elements_offset;

    return (size_t)((unsigned char *)array_value - a->bytes);
}

ArrayValue *array_resolve(Arena *a, size_t offset) {
    assert(offset <= ARENA_SIZE - sizeof(ArrayValue));
    return (ArrayValue *)(a->bytes + offset);
}

bool array_validate(Arena *a, size_t offset) {
    assert(offset <= ARENA_SIZE - sizeof(ArrayValue));
    ArrayValue *array_value = array_resolve(a, offset);

    size_t elems_offset = array_value->elements_offset;
    if (elems_offset > ARENA_SIZE - sizeof(ArrayElements)) {
        return false;
    }

    ArrayElements *elems = (ArrayElements *)(a->bytes + elems_offset);
    if (elems->tag != TAG_ARRAY_ELEMENTS) {
        return false;
    }

    if (array_value->length > elems->capacity) {
        return false;
    }

    size_t elems_total_size
        = sizeof(ArrayElements) + elems->capacity * sizeof(size_t);
    if (elems_offset + elems_total_size > ARENA_SIZE) {
        return false;
    }

    for (u64 i = 0; i < array_value->length; i++) {
        /* later we want to recursively validate elements, but it has to wait
           for the SeenSet mechanism */
    }

    return true;
}

void array_push(Arena *a, size_t array_offset, size_t value_offset) {
    ArrayValue *array_value = array_resolve(a, array_offset);
    ArrayElements *elems
        = array_elements_resolve(a, array_value->elements_offset);

    if (array_value->length >= elems->capacity) {
        u64 new_capacity = (elems->capacity == 0) ? 8 : elems->capacity * 2;

        size_t new_elems_size
            = sizeof(ArrayElements) + new_capacity * sizeof(size_t);
        ArrayElements *new_elems
            = arena_alloc(a, new_elems_size, alignof(ArrayElements));
        new_elems->tag = TAG_ARRAY_ELEMENTS;
        new_elems->capacity = new_capacity;

        memcpy(
            new_elems->elements,
            elems->elements,
            array_value->length *  sizeof(size_t)
        );

        array_value->elements_offset
            = (size_t)((unsigned char *)new_elems - a->bytes);
        elems = new_elems;
    }

    elems->elements[array_value->length] = value_offset;
    ++array_value->length;
}

size_t array_get(Arena *a, size_t array_offset, size_t index_offset) {
    ArrayValue *array_value = array_resolve(a, array_offset);
    I64Value *index_value = i64_resolve(a, index_offset);
    i64 index = index_value->payload;

    assert(index >= 0);
    assert((u64)index < array_value->length);

    ArrayElements *elems
        = array_elements_resolve(a, array_value->elements_offset);
    return elems->elements[index];
}

void array_set(
    Arena *a,
    size_t array_offset,
    size_t index_offset,
    size_t value_offset
) {
    ArrayValue *array_value = array_resolve(a, array_offset);
    I64Value *index_value = i64_resolve(a, index_offset);
    i64 index = index_value->payload;

    assert(index >= 0);
    assert((u64)index < array_value->length);

    ArrayElements *elems
        = array_elements_resolve(a, array_value->elements_offset);
    elems->elements[index] = value_offset;
}

size_t array_length(Arena *a, size_t array_offset) {
    ArrayValue *array_value = array_resolve(a, array_offset);
    return i64_new(a, (i64)array_value->length);
}

size_t array_concat(Arena *a, size_t array_offset1, size_t array_offset2) {
    ArrayValue *array_value1 = array_resolve(a, array_offset1);
    ArrayValue *array_value2 = array_resolve(a, array_offset2);
    ArrayElements *elems1
        = array_elements_resolve(a, array_value1->elements_offset);
    ArrayElements *elems2
        = array_elements_resolve(a, array_value2->elements_offset);

    u64 total_length = array_value1->length + array_value2->length;

    size_t result = array_new(a, total_length);
    ArrayValue *result_array_value = array_resolve(a, result);
    ArrayElements *result_elems
        = array_elements_resolve(a, result_array_value->elements_offset);

    memcpy(
        result_elems->elements,
        elems1->elements,
        array_value1->length * sizeof(size_t)
    );
    memcpy(
        result_elems->elements + array_value1->length,
        elems2->elements,
        array_value2->length * sizeof(size_t)
    );

    result_array_value->length = total_length;
    return result;
}

size_t array_elements_new(Arena *a, u64 capacity) {
    size_t elements_size = sizeof(ArrayElements) + capacity * sizeof(size_t);
    ArrayElements *elems = arena_alloc(
        a,
        elements_size,
        alignof(ArrayElements)
    );
    elems->tag = TAG_ARRAY_ELEMENTS;
    elems->capacity = capacity;

    return (size_t)((unsigned char *)elems - a->bytes);
}

ArrayElements *array_elements_resolve(Arena *a, size_t offset) {
    assert(offset <= ARENA_SIZE - sizeof(ArrayElements));
    return (ArrayElements *)(a->bytes + offset);
}

