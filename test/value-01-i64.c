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

#define ASSERT_I64_SUBTRACT(xv, yv, zv) do { \
    Arena arena; \
    arena_init(&arena); \
    size_t x = i64_new(&arena, xv); \
    size_t y = i64_new(&arena, yv); \
    size_t z = i64_subtract(&arena, x, y); \
    i64 actual = i64_resolve(&arena, z)->payload; \
    i64 expected = zv; \
    ASSERT_EQ(actual, expected); \
} while (0)

void positive_subtraction(void) {
    ASSERT_I64_SUBTRACT(5, 3, 2);
}

void negative_subtraction_result(void) {
    ASSERT_I64_SUBTRACT(3, 5, -2);
}

void negative_minus_negative(void) {
    ASSERT_I64_SUBTRACT(-10, -4, -6);
}

void subtracting_zero(void) {
    ASSERT_I64_SUBTRACT(42, 0, 42);
}

void zero_minus_something(void) {
    ASSERT_I64_SUBTRACT(0, 5, -5);
}

void negative_overflow_subtraction(void) {
    ASSERT_I64_SUBTRACT(INT64_MIN, 1, INT64_MAX);
}

void positive_overflow_subtraction(void) {
    ASSERT_I64_SUBTRACT(INT64_MAX, -2, INT64_MIN + 1);
}

void big_self_subtraction(void) {
    ASSERT_I64_SUBTRACT(INT64_MAX, INT64_MAX, 0);
}

#define ASSERT_I64_MULTIPLY(xv, yv, zv) do { \
    Arena arena; \
    arena_init(&arena); \
    size_t x = i64_new(&arena, xv); \
    size_t y = i64_new(&arena, yv); \
    size_t z = i64_multiply(&arena, x, y); \
    i64 actual = i64_resolve(&arena, z)->payload; \
    i64 expected = zv; \
    ASSERT_EQ(actual, expected); \
} while (0)

void positive_multiplication(void) {
    ASSERT_I64_MULTIPLY(6, 7, 42);
}

void negative_times_negative_is_positive(void) {
    ASSERT_I64_MULTIPLY(-6, -7, 42);
}

void mixed_signs_multiplication(void) {
    ASSERT_I64_MULTIPLY(-6, 7, -42);
}

void multiply_by_zero(void) {
    ASSERT_I64_MULTIPLY(42, 0, 0);
}

void multiply_by_one(void) {
    ASSERT_I64_MULTIPLY(-42, 1, -42);
}

void large_positive_times_large_positive(void) {
    ASSERT_I64_MULTIPLY(3037000500LL, 3037000500LL, -9223372036709301616LL);
}

void special_min_overflow(void) {
    ASSERT_I64_MULTIPLY(INT64_MIN, -1, INT64_MIN);
}

void extreme_values_that_just_overflow(void) {
    ASSERT_I64_MULTIPLY(4294967296LL, 2147483648LL, INT64_MIN);
}

#define ASSERT_I64_DIVIDE(xv, yv, zv, wv) do { \
    Arena arena; \
    arena_init(&arena); \
    size_t x = i64_new(&arena, xv); \
    size_t y = i64_new(&arena, yv); \
    size_t z = i64_new(&arena, zv); \
    size_t w = i64_divide(&arena, x, y, z); \
    i64 actual = i64_resolve(&arena, w)->payload; \
    i64 expected = wv; \
    ASSERT_EQ(actual, expected); \
} while (0)

void positive_exact_division() {
    ASSERT_I64_DIVIDE(42, 6, 999, 7);
}

void positive_floored_division() {
    ASSERT_I64_DIVIDE(17, 5, 999, 3);
}

void negative_floored_division() {
    ASSERT_I64_DIVIDE(-17, 5, 999, -4);
}

void positive_numerator_negative_denominator_floored() {
    ASSERT_I64_DIVIDE(17, -5, 999, -4);
}

void negative_div_negative_is_positive_floored() {
    ASSERT_I64_DIVIDE(-17, -5, 999, 3);
}

void denominator_is_zero_fallback() {
    ASSERT_I64_DIVIDE(42, 0, -1, -1);
}

void zero_numerator() {
    ASSERT_I64_DIVIDE(0, 5, 999, 0);
}

void min_divide_overflow_trap() {
    ASSERT_I64_DIVIDE(INT64_MIN, -1, 999, 999);
}

#define ASSERT_I64_MODULO(xv, yv, zv, wv) do { \
    Arena arena; \
    arena_init(&arena); \
    size_t x = i64_new(&arena, xv); \
    size_t y = i64_new(&arena, yv); \
    size_t z = i64_new(&arena, zv); \
    size_t w = i64_modulo(&arena, x, y, z); \
    i64 actual = i64_resolve(&arena, w)->payload; \
    i64 expected = wv; \
    ASSERT_EQ(actual, expected); \
} while (0)

void positive_modulo_exact() {
    ASSERT_I64_MODULO(42, 6, 999, 0);
}

void positive_modulo_with_remainder() {
    ASSERT_I64_MODULO(17, 5, 999, 2);
}

void negative_mod_positive() {
    ASSERT_I64_MODULO(-17, 5, 999, 3);
}

void positive_mod_negative() {
    ASSERT_I64_MODULO(17, -5, 999, -3);
}

void negative_mod_negative() {
    ASSERT_I64_MODULO(-17, -5, 999, -2);
}

void mod_denominator_zero_fallback() {
    ASSERT_I64_MODULO(42, 0, -1, -1);
}

void zero_mod_positive() {
    ASSERT_I64_MODULO(0, 5, 999, 0);
}

void minimum_mod_negative_one() {
    ASSERT_I64_MODULO(INT64_MIN, -1, 999, 0);
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
    RUN_TEST(positive_subtraction);
    RUN_TEST(negative_subtraction_result);
    RUN_TEST(negative_minus_negative);
    RUN_TEST(subtracting_zero);
    RUN_TEST(zero_minus_something);
    RUN_TEST(negative_overflow_subtraction);
    RUN_TEST(positive_overflow_subtraction);
    RUN_TEST(big_self_subtraction);
    RUN_TEST(positive_multiplication);
    RUN_TEST(negative_times_negative_is_positive);
    RUN_TEST(mixed_signs_multiplication);
    RUN_TEST(multiply_by_zero);
    RUN_TEST(multiply_by_one);
    RUN_TEST(large_positive_times_large_positive);
    RUN_TEST(special_min_overflow);
    RUN_TEST(extreme_values_that_just_overflow);
    RUN_TEST(positive_exact_division);
    RUN_TEST(positive_floored_division);
    RUN_TEST(negative_floored_division);
    RUN_TEST(positive_numerator_negative_denominator_floored);
    RUN_TEST(negative_div_negative_is_positive_floored);
    RUN_TEST(denominator_is_zero_fallback);
    RUN_TEST(zero_numerator);
    RUN_TEST(min_divide_overflow_trap);
    RUN_TEST(positive_modulo_exact);
    RUN_TEST(positive_modulo_with_remainder);
    RUN_TEST(negative_mod_positive);
    RUN_TEST(positive_mod_negative);
    RUN_TEST(negative_mod_negative);
    RUN_TEST(mod_denominator_zero_fallback);
    RUN_TEST(zero_mod_positive);
    RUN_TEST(minimum_mod_negative_one);
    TEST_SUMMARY();
}

