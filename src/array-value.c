#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/array-value.h"
#include "../include/arena.h"
#include "../include/i64-value.h"
#include "../include/tags.h"

Offset array_new(Arena *a, u64 capacity) {
    Offset elements_offset = array_elements_new(a, capacity);
    ArrayValue *array_value = arena_alloc(
        a,
        sizeof(ArrayValue),
        alignof(ArrayValue)
    );
    array_value->tag = TAG_ARRAY;
    array_value->length = 0;
    array_value->elements_offset = elements_offset;

    return (Offset)((unsigned char *)array_value - a->bytes);
}

ArrayValue *array_resolve(Arena *a, Offset offset) {
    assert(offset <= ARENA_SIZE - sizeof(ArrayValue));
    return (ArrayValue *)(a->bytes + offset);
}

bool array_validate(Arena *a, Offset offset) {
    assert(offset <= ARENA_SIZE - sizeof(ArrayValue));
    ArrayValue *array_value = array_resolve(a, offset);

    Offset elems_offset = array_value->elements_offset;
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

    Offset elems_total_size
        = sizeof(ArrayElements) + elems->capacity * sizeof(Offset);
    if (elems_offset + elems_total_size > ARENA_SIZE) {
        return false;
    }

    for (u64 i = 0; i < array_value->length; i++) {
        /* later we want to recursively validate elements, but it has to wait
           for the SeenSet mechanism */
    }

    return true;
}

void array_push(Arena *a, Offset array_offset, Offset value_offset) {
    ArrayValue *array_value = array_resolve(a, array_offset);
    ArrayElements *elems
        = array_elements_resolve(a, array_value->elements_offset);

    if (array_value->length >= elems->capacity) {
        u64 new_capacity = (elems->capacity == 0) ? 8 : elems->capacity * 2;

        size_t new_elems_size
            = sizeof(ArrayElements) + new_capacity * sizeof(Offset);
        ArrayElements *new_elems
            = arena_alloc(a, new_elems_size, alignof(ArrayElements));
        new_elems->tag = TAG_ARRAY_ELEMENTS;
        new_elems->capacity = new_capacity;

        memcpy(
            new_elems->elements,
            elems->elements,
            array_value->length *  sizeof(Offset)
        );

        array_value->elements_offset
            = (Offset)((unsigned char *)new_elems - a->bytes);
        elems = new_elems;
    }

    elems->elements[array_value->length] = value_offset;
    ++array_value->length;
}

Offset array_get(Arena *a, Offset array_offset, Offset index_offset) {
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
    Offset array_offset,
    Offset index_offset,
    Offset value_offset
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

Offset array_length(Arena *a, Offset array_offset) {
    ArrayValue *array_value = array_resolve(a, array_offset);
    return i64_new(a, (i64)array_value->length);
}

Offset array_concat(Arena *a, Offset array_offset1, Offset array_offset2) {
    ArrayValue *array_value1 = array_resolve(a, array_offset1);
    ArrayValue *array_value2 = array_resolve(a, array_offset2);
    ArrayElements *elems1
        = array_elements_resolve(a, array_value1->elements_offset);
    ArrayElements *elems2
        = array_elements_resolve(a, array_value2->elements_offset);

    u64 total_length = array_value1->length + array_value2->length;

    Offset result = array_new(a, total_length);
    ArrayValue *result_array_value = array_resolve(a, result);
    ArrayElements *result_elems
        = array_elements_resolve(a, result_array_value->elements_offset);

    memcpy(
        result_elems->elements,
        elems1->elements,
        array_value1->length * sizeof(Offset)
    );
    memcpy(
        result_elems->elements + array_value1->length,
        elems2->elements,
        array_value2->length * sizeof(Offset)
    );

    result_array_value->length = total_length;
    return result;
}

Offset array_elements_new(Arena *a, u64 capacity) {
    size_t elements_size = capacity * sizeof(Offset);
    ArrayElements *elems = arena_alloc(
        a,
        sizeof(ArrayElements) + elements_size,
        alignof(ArrayElements)
    );
    elems->tag = TAG_ARRAY_ELEMENTS;
    elems->capacity = capacity;

    return (Offset)((unsigned char *)elems - a->bytes);
}

ArrayElements *array_elements_resolve(Arena *a, Offset offset) {
    assert(offset <= ARENA_SIZE - sizeof(ArrayElements));
    return (ArrayElements *)(a->bytes + offset);
}

