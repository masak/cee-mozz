#include <string.h>

#include "../include/array-value.h"
#include "../include/i64-value.h"
#include "../include/test.h"
#include "../include/types.h"
#include "../include/value.h"

#define ASSERT_ARRAY_LENGTH(arr, expected) do { \
    size_t len_offset = array_length(&arena, arr); \
    i64 actual = i64_resolve(&arena, len_offset)->payload; \
    ASSERT_EQ(actual, expected); \
} while (0)

#define ASSERT_ARRAY_GET(arr, idx, expected_offset) do { \
    size_t idx_offset = i64_new(&arena, idx); \
    size_t actual_offset = array_get(&arena, arr, idx_offset); \
    ASSERT_EQ(actual_offset, expected_offset); \
} while (0)

static Arena arena;

void create_empty_array(void) {
    arena_init(&arena);
    size_t array_offset = array_new(&arena, 0);
    ArrayValue *array_value = array_resolve(&arena, array_offset);
    ASSERT_EQ(array_value->tag, TAG_ARRAY);
    ASSERT_EQ(array_value->length, 0);
    ASSERT(array_validate(&arena, array_offset));
}

void array_with_capacity(void) {
    arena_init(&arena);
    size_t array_offset = array_new(&arena, 16);
    ArrayValue *array_value = array_resolve(&arena, array_offset);
    ASSERT_EQ(array_value->tag, TAG_ARRAY);
    ASSERT_EQ(array_value->length, 0);
    ASSERT(array_validate(&arena, array_offset));
}

void push_increases_length(void) {
    arena_init(&arena);
    size_t array_offset = array_new(&arena, 2);
    size_t element_offset = i64_new(&arena, 42);
    array_push(&arena, array_offset, element_offset);
    ASSERT_ARRAY_LENGTH(array_offset, 1);
}

void push_stores_value(void) {
    arena_init(&arena);
    size_t array_offset = array_new(&arena, 2);
    size_t element_offset = i64_new(&arena, 42);
    array_push(&arena, array_offset, element_offset);
    ASSERT_ARRAY_GET(array_offset, 0, element_offset);
}

void push_beyond_capacity_doubles(void) {
    arena_init(&arena);
    size_t array_offset = array_new(&arena, 1);
    size_t el1_offset = i64_new(&arena, 1);
    size_t el2_offset = i64_new(&arena, 2);
    array_push(&arena, array_offset, el1_offset);
    array_push(&arena, array_offset, el2_offset);
    ASSERT_ARRAY_LENGTH(array_offset, 2);
    ASSERT_ARRAY_GET(array_offset, 0, el1_offset);
    ASSERT_ARRAY_GET(array_offset, 1, el2_offset);
}

void push_zero_capacity_grows_to_eight(void) {
    arena_init(&arena);
    size_t array_offset = array_new(&arena, 1);
    size_t element_offsets[9];
    for (int i = 0; i < 9; i++) {
        element_offsets[i] = i64_new(&arena, i);
        array_push(&arena, array_offset, element_offsets[i]);
    }
    ASSERT_ARRAY_LENGTH(array_offset, 9);
    for (int i = 0; i < 9; i++) {
        ASSERT_ARRAY_GET(array_offset, i, element_offsets[i]);
    }
}

void get_and_set(void) {
    arena_init(&arena);
    size_t array_offset = array_new(&arena, 4);
    size_t el1_offset = i64_new(&arena, 10);
    size_t el2_offset = i64_new(&arena, 20);
    array_push(&arena, array_offset, el1_offset);
    array_push(&arena, array_offset, el2_offset);

    size_t index = i64_new(&arena, 0);
    array_set(&arena, array_offset, index, el2_offset);
    ASSERT_ARRAY_GET(array_offset, 0, el2_offset);
}

void concatenate_two_arrays(void) {
    arena_init(&arena);
    size_t arr1_offset = array_new(&arena, 2);
    size_t arr2_offset = array_new(&arena, 2);
    size_t a = i64_new(&arena, 1);
    size_t b = i64_new(&arena, 2);
    size_t c = i64_new(&arena, 3);
    array_push(&arena, arr1_offset, a);
    array_push(&arena, arr1_offset, b);
    array_push(&arena, arr2_offset, c);

    size_t result = array_concat(&arena, arr1_offset, arr2_offset);
    ASSERT_ARRAY_LENGTH(result, 3);
    ASSERT_ARRAY_GET(result, 0, a);
    ASSERT_ARRAY_GET(result, 1, b);
    ASSERT_ARRAY_GET(result, 2, c);
}

void concatenate_with_empty(void) {
    arena_init(&arena);
    size_t arr1_offset = array_new(&arena, 2);
    size_t arr2_offset = array_new(&arena, 2);
    size_t a = i64_new(&arena, 1);
    array_push(&arena, arr1_offset, a);

    size_t result = array_concat(&arena, arr1_offset, arr2_offset);
    ASSERT_ARRAY_LENGTH(result, 1);
    ASSERT_ARRAY_GET(result, 0, a);
}

void concatenate_creates_distinct_array(void) {
    arena_init(&arena);
    size_t arr1_offset = array_new(&arena, 2);
    size_t arr2_offset = array_new(&arena, 2);
    size_t a = i64_new(&arena, 1);
    array_push(&arena, arr1_offset, a);
    array_push(&arena, arr2_offset, a);

    size_t result = array_concat(&arena, arr1_offset, arr2_offset);
    ASSERT(result != arr1_offset);
    ASSERT(result != arr2_offset);
}

int main(void) {
    RUN_TEST(create_empty_array);
    RUN_TEST(array_with_capacity);
    RUN_TEST(push_increases_length);
    RUN_TEST(push_stores_value);
    RUN_TEST(push_beyond_capacity_doubles);
    RUN_TEST(push_zero_capacity_grows_to_eight);
    RUN_TEST(get_and_set);
    RUN_TEST(concatenate_two_arrays);
    RUN_TEST(concatenate_with_empty);
    RUN_TEST(concatenate_creates_distinct_array);
    TEST_SUMMARY();
}

