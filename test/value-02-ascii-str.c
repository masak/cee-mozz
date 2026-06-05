#include <stdint.h>
#include <string.h>

#include "../include/ascii-str-value.h"
#include "../include/i64-value.h"
#include "../include/test.h"
#include "../include/typedefs.h"
#include "../include/types.h"

/* Compare an AsciiStrValue's payload against an expected C string */
#define ASSERT_ASCII_STR_EQ(val, expected) do { \
    char _buf[256]; \
    size_t _len = (val)->length_in_bytes; \
    ASSERT(_len < sizeof(_buf)); \
    memcpy(_buf, (val)->payload, _len); \
    _buf[_len] = '\0'; \
    ASSERT_STR_EQ(_buf, expected); \
    ASSERT_EQ(_len, strlen(expected)); \
} while (0)

/* Create an AsciiStrValue from a string literal and assert its contents */
#define ASSERT_ASCII_STR_NEW(s, expected) do { \
    Arena arena; \
    arena_init(&arena); \
    s8 str = s8(s); \
    size_t offset = ascii_str_new(&arena, &str); \
    AsciiStrValue *val = ascii_str_resolve(&arena, offset); \
    ASSERT_EQ(val->tag, TAG_ASCII_STR); \
    ASSERT_ASCII_STR_EQ(val, expected); \
} while (0)

/* Convert an i64 to a string via generic_to_str and assert the result */
#define ASSERT_I64_TO_STR(xv, expected) do { \
    Arena arena; \
    arena_init(&arena); \
    size_t i64_offset = i64_new(&arena, xv); \
    size_t str_offset = generic_to_str(&arena, i64_offset); \
    AsciiStrValue *val = ascii_str_resolve(&arena, str_offset); \
    ASSERT_EQ(val->tag, TAG_ASCII_STR); \
    ASSERT_ASCII_STR_EQ(val, expected); \
} while (0)

/* Assert that generic_to_str on an AsciiStrValue returns the same offset */
#define ASSERT_STR_IDENTITY(s) do { \
    Arena arena; \
    arena_init(&arena); \
    s8 str = s8(s); \
    size_t str_offset = ascii_str_new(&arena, &str); \
    size_t result_offset = generic_to_str(&arena, str_offset); \
    ASSERT_EQ(result_offset, str_offset); \
} while (0)

void create_empty_string(void) {
    ASSERT_ASCII_STR_NEW("", "");
}

void create_short_string(void) {
    ASSERT_ASCII_STR_NEW("hi", "hi");
}

void create_single_character(void) {
    ASSERT_ASCII_STR_NEW("x", "x");
}

void create_longer_string(void) {
    ASSERT_ASCII_STR_NEW("hello world", "hello world");
}

void create_with_special_characters(void) {
    ASSERT_ASCII_STR_NEW("a1!@#$%^&*()", "a1!@#$%^&*()");
}

void concat_two_strings(void) {
    Arena arena;
    arena_init(&arena);
    size_t m = ascii_str_new(&arena, &s8("hello"));
    size_t n = ascii_str_new(&arena, &s8("world"));
    size_t result = ascii_str_concat(&arena, m, n);
    AsciiStrValue *val = ascii_str_resolve(&arena, result);
    ASSERT_EQ(val->tag, TAG_ASCII_STR);
    ASSERT_ASCII_STR_EQ(val, "helloworld");
}

void concat_with_empty_left(void) {
    Arena arena;
    arena_init(&arena);
    size_t m = ascii_str_new(&arena, &s8(""));
    size_t n = ascii_str_new(&arena, &s8("world"));
    size_t result = ascii_str_concat(&arena, m, n);
    AsciiStrValue *val = ascii_str_resolve(&arena, result);
    ASSERT_EQ(val->tag, TAG_ASCII_STR);
    ASSERT_ASCII_STR_EQ(val, "world");
}

void concat_with_empty_right(void) {
    Arena arena;
    arena_init(&arena);
    size_t m = ascii_str_new(&arena, &s8("hello"));
    size_t n = ascii_str_new(&arena, &s8(""));
    size_t result = ascii_str_concat(&arena, m, n);
    AsciiStrValue *val = ascii_str_resolve(&arena, result);
    ASSERT_EQ(val->tag, TAG_ASCII_STR);
    ASSERT_ASCII_STR_EQ(val, "hello");
}

void concat_both_empty(void) {
    Arena arena;
    arena_init(&arena);
    size_t m = ascii_str_new(&arena, &s8(""));
    size_t n = ascii_str_new(&arena, &s8(""));
    size_t result = ascii_str_concat(&arena, m, n);
    AsciiStrValue *val = ascii_str_resolve(&arena, result);
    ASSERT_EQ(val->tag, TAG_ASCII_STR);
    ASSERT_ASCII_STR_EQ(val, "");
}

void concat_multiple(void) {
    Arena arena;
    arena_init(&arena);
    size_t a = ascii_str_new(&arena, &s8("a"));
    size_t b = ascii_str_new(&arena, &s8("b"));
    size_t c = ascii_str_new(&arena, &s8("c"));
    size_t ab = ascii_str_concat(&arena, a, b);
    size_t abc = ascii_str_concat(&arena, ab, c);
    AsciiStrValue *val = ascii_str_resolve(&arena, abc);
    ASSERT_ASCII_STR_EQ(val, "abc");
}

void concat_preserves_lengths(void) {
    Arena arena;
    arena_init(&arena);
    size_t m = ascii_str_new(&arena, &s8("hello"));
    size_t n = ascii_str_new(&arena, &s8("world"));
    AsciiStrValue *lhs = ascii_str_resolve(&arena, m);
    AsciiStrValue *rhs = ascii_str_resolve(&arena, n);
    size_t result = ascii_str_concat(&arena, m, n);
    AsciiStrValue *val = ascii_str_resolve(&arena, result);
    ASSERT_EQ(
        val->length_in_bytes,
        lhs->length_in_bytes + rhs->length_in_bytes
    );
}

void concat_creates_distinct_values(void) {
    Arena arena;
    arena_init(&arena);
    size_t m = ascii_str_new(&arena, &s8("x"));
    size_t n = ascii_str_new(&arena, &s8("y"));
    size_t result = ascii_str_concat(&arena, m, n);
    ASSERT(result != m);
    ASSERT(result != n);
}

void to_str_from_positive_i64(void) {
    ASSERT_I64_TO_STR(42, "42");
}

void to_str_from_negative_i64(void) {
    ASSERT_I64_TO_STR(-17, "-17");
}

void to_str_from_zero_i64(void) {
    ASSERT_I64_TO_STR(0, "0");
}

void to_str_from_one_digit_i64(void) {
    ASSERT_I64_TO_STR(7, "7");
}

void to_str_from_large_positive_i64(void) {
    ASSERT_I64_TO_STR(1234567890123LL, "1234567890123");
}

void to_str_from_large_negative_i64(void) {
    ASSERT_I64_TO_STR(-1234567890123LL, "-1234567890123");
}

void to_str_from_max_i64(void) {
    ASSERT_I64_TO_STR(INT64_MAX, "9223372036854775807");
}

void to_str_from_min_i64(void) {
    ASSERT_I64_TO_STR(INT64_MIN, "-9223372036854775808");
}

void to_str_from_i64_creates_new_value(void) {
    Arena arena;
    arena_init(&arena);
    size_t i64_offset = i64_new(&arena, 42);
    size_t str_offset = generic_to_str(&arena, i64_offset);
    ASSERT(str_offset != i64_offset);
    ASSERT_EQ(ascii_str_resolve(&arena, str_offset)->tag, TAG_ASCII_STR);
}

void to_str_identity_for_ascii_str(void) {
    ASSERT_STR_IDENTITY("hello");
}

void to_str_identity_for_empty_ascii_str(void) {
    ASSERT_STR_IDENTITY("");
}

void to_str_identity_returns_same_offset(void) {
    Arena arena;
    arena_init(&arena);
    size_t offset = ascii_str_new(&arena, &s8("unchanged"));
    size_t result = generic_to_str(&arena, offset);
    ASSERT_EQ(result, offset);
}

int main(void) {
    RUN_TEST(create_empty_string);
    RUN_TEST(create_short_string);
    RUN_TEST(create_single_character);
    RUN_TEST(create_longer_string);
    RUN_TEST(create_with_special_characters);
    RUN_TEST(concat_two_strings);
    RUN_TEST(concat_with_empty_left);
    RUN_TEST(concat_with_empty_right);
    RUN_TEST(concat_both_empty);
    RUN_TEST(concat_multiple);
    RUN_TEST(concat_preserves_lengths);
    RUN_TEST(concat_creates_distinct_values);
    RUN_TEST(to_str_from_positive_i64);
    RUN_TEST(to_str_from_negative_i64);
    RUN_TEST(to_str_from_zero_i64);
    RUN_TEST(to_str_from_one_digit_i64);
    RUN_TEST(to_str_from_large_positive_i64);
    RUN_TEST(to_str_from_large_negative_i64);
    RUN_TEST(to_str_from_max_i64);
    RUN_TEST(to_str_from_min_i64);
    RUN_TEST(to_str_from_i64_creates_new_value);
    RUN_TEST(to_str_identity_for_ascii_str);
    RUN_TEST(to_str_identity_for_empty_ascii_str);
    RUN_TEST(to_str_identity_returns_same_offset);
    TEST_SUMMARY();
}

