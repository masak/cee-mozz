#include <string.h>

#include "../include/arena.h"
#include "../include/array-value.h"
#include "../include/i64-value.h"
#include "../include/seenset.h"
#include "../include/test.h"
#include "../include/tags.h"
#include "../include/value.h"

#define ASSERT_ARRAY_LENGTH(arr, expected) do { \
    Offset len_offset = array_length(&arena, arr); \
    i64 actual = i64_resolve(&arena, len_offset)->payload; \
    ASSERT_I64_EQ(actual, expected); \
} while (0)

#define ASSERT_ARRAY_GET(arr, idx, expected_offset) do { \
    Offset idx_offset = i64_new(&arena, idx); \
    Offset actual_offset = array_get(&arena, arr, idx_offset); \
    ASSERT_OFFSET_EQ(actual_offset, expected_offset); \
} while (0)

static Arena arena;

void create_empty_array(void) {
    arena_init(&arena);
    Offset array_offset = array_new(&arena, 0);
    ArrayValue *array_value = array_resolve(&arena, array_offset);
    SeenSet seenset;
    seenset_init(&seenset);

    ASSERT_TAG_EQ(array_value->tag, TAG_ARRAY);
    ASSERT_I64_EQ(array_value->length, 0);
    ASSERT(array_validate(&arena, array_offset, &seenset));
}

void array_with_capacity(void) {
    arena_init(&arena);
    Offset array_offset = array_new(&arena, 16);
    ArrayValue *array_value = array_resolve(&arena, array_offset);
    SeenSet seenset;
    seenset_init(&seenset);

    ASSERT_TAG_EQ(array_value->tag, TAG_ARRAY);
    ASSERT_I64_EQ(array_value->length, 0);
    ASSERT(array_validate(&arena, array_offset, &seenset));
}

void push_increases_length(void) {
    arena_init(&arena);
    Offset array_offset = array_new(&arena, 2);
    Offset element_offset = i64_new(&arena, 42);
    array_push(&arena, array_offset, element_offset);
    ASSERT_ARRAY_LENGTH(array_offset, 1);
}

void push_stores_value(void) {
    arena_init(&arena);
    Offset array_offset = array_new(&arena, 2);
    Offset element_offset = i64_new(&arena, 42);
    array_push(&arena, array_offset, element_offset);
    ASSERT_ARRAY_GET(array_offset, 0, element_offset);
}

void push_beyond_capacity_doubles(void) {
    arena_init(&arena);
    Offset array_offset = array_new(&arena, 1);
    Offset el1_offset = i64_new(&arena, 1);
    Offset el2_offset = i64_new(&arena, 2);
    array_push(&arena, array_offset, el1_offset);
    array_push(&arena, array_offset, el2_offset);
    ASSERT_ARRAY_LENGTH(array_offset, 2);
    ASSERT_ARRAY_GET(array_offset, 0, el1_offset);
    ASSERT_ARRAY_GET(array_offset, 1, el2_offset);
}

void push_zero_capacity_grows_to_eight(void) {
    arena_init(&arena);
    Offset array_offset = array_new(&arena, 1);
    Offset element_offsets[9];
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
    Offset array_offset = array_new(&arena, 4);
    Offset el1_offset = i64_new(&arena, 10);
    Offset el2_offset = i64_new(&arena, 20);
    array_push(&arena, array_offset, el1_offset);
    array_push(&arena, array_offset, el2_offset);

    Offset index = i64_new(&arena, 0);
    array_set(&arena, array_offset, index, el2_offset);
    ASSERT_ARRAY_GET(array_offset, 0, el2_offset);
}

void concatenate_two_arrays(void) {
    arena_init(&arena);
    Offset arr1_offset = array_new(&arena, 2);
    Offset arr2_offset = array_new(&arena, 2);
    Offset a = i64_new(&arena, 1);
    Offset b = i64_new(&arena, 2);
    Offset c = i64_new(&arena, 3);
    array_push(&arena, arr1_offset, a);
    array_push(&arena, arr1_offset, b);
    array_push(&arena, arr2_offset, c);

    Offset result = array_concat(&arena, arr1_offset, arr2_offset);
    ASSERT_ARRAY_LENGTH(result, 3);
    ASSERT_ARRAY_GET(result, 0, a);
    ASSERT_ARRAY_GET(result, 1, b);
    ASSERT_ARRAY_GET(result, 2, c);
}

void concatenate_with_empty(void) {
    arena_init(&arena);
    Offset arr1_offset = array_new(&arena, 2);
    Offset arr2_offset = array_new(&arena, 2);
    Offset a = i64_new(&arena, 1);
    array_push(&arena, arr1_offset, a);

    Offset result = array_concat(&arena, arr1_offset, arr2_offset);
    ASSERT_ARRAY_LENGTH(result, 1);
    ASSERT_ARRAY_GET(result, 0, a);
}

void concatenate_creates_distinct_array(void) {
    arena_init(&arena);
    Offset arr1_offset = array_new(&arena, 2);
    Offset arr2_offset = array_new(&arena, 2);
    Offset a = i64_new(&arena, 1);
    array_push(&arena, arr1_offset, a);
    array_push(&arena, arr2_offset, a);

    Offset result = array_concat(&arena, arr1_offset, arr2_offset);
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

