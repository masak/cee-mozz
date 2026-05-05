#include <stdint.h>

#include "../include/i64-value.h"
#include "../include/test.h"

#define ASSERT_I64_ADD(xv, yv, zv) do { \
    Arena arena; \
    arena_init(&arena); \
    size_t x = i64_new(&arena, xv); \
    size_t y = i64_new(&arena, yv); \
    size_t z = i64_add(&arena, x, y); \
    i64 actual = i64_resolve(&arena, z)->payload; \
    i64 expected = zv; \
    ASSERT_EQ(actual, expected); \
} while (0)

void positive_addition(void) {
    ASSERT_I64_ADD(1, 2, 3);
}

void negative_addition(void) {
    ASSERT_I64_ADD(-5, -7, -12);
}

void mixed_signs(void) {
    ASSERT_I64_ADD(10, -3, 7);
}

void adding_zero(void) {
    ASSERT_I64_ADD(42, 0, 42);
}

void maximum_positive_overflow(void) {
    ASSERT_I64_ADD(INT64_MAX, 1, INT64_MIN);
}

void maximum_negative_overflow(void) {
    ASSERT_I64_ADD(INT64_MIN, -1, INT64_MAX);
}

void large_value_that_does_not_overflow(void) {
    ASSERT_I64_ADD(INT64_MAX - 1, 1, INT64_MAX);
}

void large_opposite_values(void) {
    ASSERT_I64_ADD(INT64_MAX, INT64_MIN, -1);
}

int main(void) {
    RUN_TEST(positive_addition);
    RUN_TEST(negative_addition);
    RUN_TEST(mixed_signs);
    RUN_TEST(adding_zero);
    RUN_TEST(maximum_positive_overflow);
    RUN_TEST(maximum_negative_overflow);
    RUN_TEST(large_value_that_does_not_overflow);
    RUN_TEST(large_opposite_values);
    TEST_SUMMARY();
}

