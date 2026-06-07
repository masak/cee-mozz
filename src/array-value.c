#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/array-value.h"
#include "../include/arena.h"
#include "../include/i64-value.h"
#include "../include/seenset.h"
#include "../include/tags.h"
#include "../include/value.h"

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

bool array_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    ArrayValue *array_value = array_resolve(a, offset);

    Offset elems_offset = array_value->elements_offset;
    if (value_tag(a, elems_offset) != TAG_ARRAY_ELEMENTS) {
        return false;
    }
    array_elements_validate(a, elems_offset, seenset);

    ArrayElements *array_elements = (ArrayElements *)(a->bytes + elems_offset);
    if (array_value->length > array_elements->capacity) {
        return false;
    }

    for (u64 i = 0; i < array_value->length; i++) {
        MaybeOffset element_offset = array_elements->elements[i];
        if (element_offset == UNSET) {
            return false;
        }
    }
    for (u64 i = array_value->length; i < array_elements->capacity; i++) {
        MaybeOffset element_offset = array_elements->elements[i];
        if (element_offset != UNSET) {
            return false;
        }
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

bool array_elements_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    ArrayElements *array_elements = array_elements_resolve(a, offset);

    Offset elems_total_size
        = sizeof(ArrayElements) + array_elements->capacity * sizeof(Offset);
    if (offset + elems_total_size > ARENA_SIZE) {
        return false;
    }

    for (u64 i = 0; i < array_elements->capacity; i++) {
        MaybeOffset element_offset = array_elements->elements[i];
        if (element_offset == UNSET) {
            continue;
        }
        if (!generic_validate(a, element_offset, seenset)) {
            return false;
        }
    }

    return true;
}

