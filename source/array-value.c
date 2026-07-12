#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/array-value.h"
#include "../include/arena.h"
#include "../include/crash.h"
#include "../include/i64-value.h"
#include "../include/outcome.h"
#include "../include/seenset.h"
#include "../include/tags.h"
#include "../include/value.h"

/* Allocate a new ArrayValue. */
Offset array_new(Arena *a, u32 capacity) {
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

/* Return a pointer to an ArrayValue, given an offset into an arena.
 *
 * Precondition: `offset` points to an ArrayValue.
 */
ArrayValue *array_resolve(Arena *a, Offset offset) {
    if (value_tag(a, offset) != TAG_ARRAY) {
        vm_crash(CRASH_INVALID_TAG);
    }

    assert(offset <= ARENA_SIZE - sizeof(ArrayValue));
    return (ArrayValue *)(a->bytes + offset);
}

/* Return true if `offset` is already seen, or valid.
 *
 * Precondition: `offset` points to an ArrayValue.
 */
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
    if (!array_elements_validate(a, elems_offset, seenset)) {
        return false;
    }

    ArrayElements *array_elements = (ArrayElements *)(a->bytes + elems_offset);
    if (array_value->length > array_elements->capacity) {
        return false;
    }

    for (u32 i = 0; i < array_value->length; i++) {
        MaybeOffset element_offset = array_elements->elements[i];
        if (element_offset == UNSET) {
            return false;
        }
    }
    for (u32 i = array_value->length; i < array_elements->capacity; i++) {
        MaybeOffset element_offset = array_elements->elements[i];
        if (element_offset != UNSET) {
            return false;
        }
    }

    return true;
}

/* Add a value to the end of an ArrayValue.
 *
 * Preconditions: `array_offset` points to a valid ArrayValue. There's room for
 *                at least one more element, without the `u32` length
 *                overflowing.
 * Postcondition: The array has been extended with one element at the end,
 *                namely `value_offset`.
 */
Outcome array_push(Arena *a, Offset array_offset, Offset value_offset) {
    ArrayValue *array_value = array_resolve(a, array_offset);
    ArrayElements *elems
        = array_elements_resolve(a, array_value->elements_offset);

    if (array_value->length + 1 < array_value->length) { /* overflow */
        vm_crash(CRASH_ARRAY_TOO_LONG);
    }

    if (array_value->length >= elems->capacity) {
        u32 new_capacity = (elems->capacity == 0) ? 8 : elems->capacity * 2;

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

    return OUTCOME_OK;
}

/* Return an element from the ArrayValue at `array_offset`, at the index
 * indicated by `index_offset`.
 *
 * Precondition: `array_offset` points to a valid ArrayValue; `index_offset`
 *               points to a valid integer value.
 * Additional expectation: `index_offset` is within bounds; that is, it falls
 *                         within the range 0 ..^ N, N being the length of the
 *                         array.
 */
Outcome array_get(
    Arena *a,
    Offset array_offset,
    Offset index_offset,
    Offset *out_offset
) {
    ArrayValue *array_value = array_resolve(a, array_offset);
    /* XXX: `index_value` can be an `IntValue`, not just an `I64Value`. We
       should extend the code here, but also add a test for it. */
    I64Value *index_value = i64_resolve(a, index_offset);
    i64 index = index_value->payload;

    if (index < 0 || (u32)index >= array_value->length) {
        return OUTCOME_E604_INDEX;
    }

    ArrayElements *elems
        = array_elements_resolve(a, array_value->elements_offset);
    *out_offset = elems->elements[index];
    return OUTCOME_OK;
}

/* Store a value from `value_offset` into an element slot in the ArrayValue at
 * `array_offset`, at the index indicated by `index_offset`.
 *
 * Precondition: `array_offset` points to a valid ArrayValue; `index_offset`
 *               points to a valid integer value.
 * Additional expectation: `index_offset` is within bounds; that is, it falls
 *                         within the range 0 ..^ N, N being the length of the
 *                         array.
 */
Outcome array_set(
    Arena *a,
    Offset array_offset,
    Offset index_offset,
    Offset value_offset
) {
    ArrayValue *array_value = array_resolve(a, array_offset);
    /* XXX: `index_value` can be an `IntValue`, not just an `I64Value`. We
       should extend the code here, but also add a test for it. */
    I64Value *index_value = i64_resolve(a, index_offset);
    i64 index = index_value->payload;

    if (index < 0 || (u32)index >= array_value->length) {
        return OUTCOME_E604_INDEX;
    }

    ArrayElements *elems
        = array_elements_resolve(a, array_value->elements_offset);
    elems->elements[index] = value_offset;
    return OUTCOME_OK;
}

/* Return the length of the ArrayValue at `array_offset`.
 *
 * Precondition: `array_offset` points to a valid ArrayValue.
 */
Outcome array_length(Arena *a, Offset array_offset, Offset *out_offset) {
    ArrayValue *array_value = array_resolve(a, array_offset);
    *out_offset = i64_new(a, (i64)array_value->length);
    return OUTCOME_OK;
}

/* Return an ArrayValue whose elements are those of `array_offset`, followed by
 * those of `array_offset2`.
 *
 * Preconditions: `array_offset1` points to a valid ArrayValue; `array_offset2`
 *                points to a valid ArrayValue. Their combined length fits in a
 *                `u32`.
 */
Outcome array_concat(
    Arena *a,
    Offset array_offset1,
    Offset array_offset2,
    Offset *out_offset
) {
    ArrayValue *array_value1 = array_resolve(a, array_offset1);
    ArrayValue *array_value2 = array_resolve(a, array_offset2);
    ArrayElements *elems1
        = array_elements_resolve(a, array_value1->elements_offset);
    ArrayElements *elems2
        = array_elements_resolve(a, array_value2->elements_offset);

    u32 total_length = array_value1->length + array_value2->length;
    if (total_length < array_value1->length) {
        vm_crash(CRASH_ARRAY_TOO_LONG);
    }

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
    *out_offset = result;
    return OUTCOME_OK;
}

/* Allocate a new ArrayElements. */
Offset array_elements_new(Arena *a, u32 capacity) {
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

/* Returns a pointer to an ArrayElements, given an offset into an arena.
 *
 * Precondition: `offset` points to a valid ArrayElements.
 */
ArrayElements *array_elements_resolve(Arena *a, Offset offset) {
    if (value_tag(a, offset) != TAG_ARRAY_ELEMENTS) {
        vm_crash(CRASH_INVALID_TAG);
    }

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

    for (u32 i = 0; i < array_elements->capacity; i++) {
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

