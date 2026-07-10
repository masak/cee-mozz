#include <stdint.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/seenset.h"
#include "../include/small-str-value.h"
#include "../include/str-value.h"
#include "../include/tags.h"
#include "../include/test.h"
#include "../include/typedefs.h"
#include "../include/value.h"

/* Extract SmallStrValue payload into a buffer and returns its length.
 * Leading zero bytes are padding; the payload is right-aligned. */
#define SMALL_STR_EXTRACT(val, buf, out_len) do { \
    u32 _i = 0; \
    while (_i < 8 && (val)->payload[_i] == 0) { ++_i; } \
    *(out_len) = 8 - _i; \
    memcpy((buf), (val)->payload + _i, *(out_len)); \
} while (0)

/* Compare a SmallStrValue's payload against expected bytes. */
#define ASSERT_SMALL_STR_EQ_BYTES(val, expected_bytes, expected_len) do { \
    u8 _buf[8]; \
    u32 _len; \
    SMALL_STR_EXTRACT((val), _buf, &_len); \
    ASSERT_U32_EQ(_len, (expected_len)); \
    ASSERT_TRUE(memcmp(_buf, (expected_bytes), _len) == 0); \
} while (0)

/* Create a SmallStrValue from a string literal and assert its contents */
#define ASSERT_SMALL_STR_NEW(s, expected_bytes, expected_len) do { \
    Arena arena; \
    arena_init(&arena); \
    s8 str = s8(s); \
    Offset offset = small_str_new(&arena, &str); \
    SmallStrValue *val = small_str_resolve(&arena, offset); \
    ASSERT_TAG_EQ(val->tag, TAG_SMALL_STR); \
    ASSERT_SMALL_STR_EQ_BYTES(val, expected_bytes, expected_len); \
} while (0)

void create_empty_string(void) {
    ASSERT_SMALL_STR_NEW("", "", 0);
}

void create_single_ascii(void) {
    ASSERT_SMALL_STR_NEW("x", "x", 1);
}

void create_exactly_eight_bytes(void) {
    ASSERT_SMALL_STR_NEW("abcdefgh", "abcdefgh", 8);
}

void create_two_byte_utf8(void) {
    /* U+00E9 LATIN SMALL LETTER E WITH ACUTE: C3 A9 */
    u8 bytes[] = { 0xC3, 0xA9 };
    ASSERT_SMALL_STR_NEW("\xC3\xA9", bytes, 2);
}

void create_three_byte_utf8(void) {
    /* U+9053 CJK UNIFIED IDEOGRAPH-9053: E9 81 93 */
    u8 bytes[] = { 0xE9, 0x81, 0x93 };
    ASSERT_SMALL_STR_NEW("\xE9\x81\x93", bytes, 3);
}

void create_four_byte_utf8(void) {
    /* U+1F600 GRINNING FACE: F0 9F 98 80 */
    u8 bytes[] = { 0xF0, 0x9F, 0x98, 0x80 };
    ASSERT_SMALL_STR_NEW("\xF0\x9F\x98\x80", bytes, 4);
}

void create_mixed_ascii_and_multibyte(void) {
    /* aéb = a + U+00E9 + b */
    u8 bytes[] = { 'a', 0xC3, 0xA9, 'b' };
    ASSERT_SMALL_STR_NEW("a\xC3\xA9" "b", bytes, 4);
}

void create_five_byte_leaves_padding(void) {
    Arena arena;
    arena_init(&arena);
    s8 str = s8("hello");
    Offset offset = small_str_new(&arena, &str);
    SmallStrValue *val = small_str_resolve(&arena, offset);
    ASSERT_TAG_EQ(val->tag, TAG_SMALL_STR);
    /* First three payload bytes should be zero padding */
    ASSERT_TRUE(val->payload[0] == 0);
    ASSERT_TRUE(val->payload[1] == 0);
    ASSERT_TRUE(val->payload[2] == 0);
    /* Content starts at index 3 */
    ASSERT_TRUE(val->payload[3] == 'h');
}

void concat_two_small_strings(void) {
    Arena arena;
    arena_init(&arena);
    Offset m = small_str_new(&arena, &s8("hi"));
    Offset n = small_str_new(&arena, &s8("ya"));
    Offset result = small_str_concat(&arena, m, n);
    SmallStrValue *val = small_str_resolve(&arena, result);
    ASSERT_TAG_EQ(val->tag, TAG_SMALL_STR);
    ASSERT_SMALL_STR_EQ_BYTES(val, "hiya", 4);
}

void concat_with_empty_left(void) {
    Arena arena;
    arena_init(&arena);
    Offset m = small_str_new(&arena, &s8(""));
    Offset n = small_str_new(&arena, &s8("x"));
    Offset result = small_str_concat(&arena, m, n);
    SmallStrValue *val = small_str_resolve(&arena, result);
    ASSERT_TAG_EQ(val->tag, TAG_SMALL_STR);
    ASSERT_SMALL_STR_EQ_BYTES(val, "x", 1);
}

void concat_with_empty_right(void) {
    Arena arena;
    arena_init(&arena);
    Offset m = small_str_new(&arena, &s8("x"));
    Offset n = small_str_new(&arena, &s8(""));
    Offset result = small_str_concat(&arena, m, n);
    SmallStrValue *val = small_str_resolve(&arena, result);
    ASSERT_TAG_EQ(val->tag, TAG_SMALL_STR);
    ASSERT_SMALL_STR_EQ_BYTES(val, "x", 1);
}

void concat_both_empty(void) {
    Arena arena;
    arena_init(&arena);
    Offset m = small_str_new(&arena, &s8(""));
    Offset n = small_str_new(&arena, &s8(""));
    Offset result = small_str_concat(&arena, m, n);
    SmallStrValue *val = small_str_resolve(&arena, result);
    ASSERT_TAG_EQ(val->tag, TAG_SMALL_STR);
    ASSERT_SMALL_STR_EQ_BYTES(val, "", 0);
}

void concat_exactly_eight(void) {
    Arena arena;
    arena_init(&arena);
    Offset m = small_str_new(&arena, &s8("ship"));
    Offset n = small_str_new(&arena, &s8("mate"));
    Offset result = small_str_concat(&arena, m, n);
    SmallStrValue *val = small_str_resolve(&arena, result);
    ASSERT_TAG_EQ(val->tag, TAG_SMALL_STR);
    ASSERT_SMALL_STR_EQ_BYTES(val, "shipmate", 8);
}

void concat_seven_plus_one(void) {
    Arena arena;
    arena_init(&arena);
    Offset m = small_str_new(&arena, &s8("polygon"));
    Offset n = small_str_new(&arena, &s8("s"));
    Offset result = small_str_concat(&arena, m, n);
    SmallStrValue *val = small_str_resolve(&arena, result);
    ASSERT_TAG_EQ(val->tag, TAG_SMALL_STR);
    ASSERT_SMALL_STR_EQ_BYTES(val, "polygons", 8);
}

void concat_eight_plus_empty(void) {
    Arena arena;
    arena_init(&arena);
    Offset m = small_str_new(&arena, &s8("garbanzo"));
    Offset n = small_str_new(&arena, &s8(""));
    Offset result = small_str_concat(&arena, m, n);
    SmallStrValue *val = small_str_resolve(&arena, result);
    ASSERT_TAG_EQ(val->tag, TAG_SMALL_STR);
    ASSERT_SMALL_STR_EQ_BYTES(val, "garbanzo", 8);
}

void concat_overflow_to_str(void) {
    Arena arena;
    arena_init(&arena);
    Offset m = small_str_new(&arena, &s8("screw"));
    Offset n = small_str_new(&arena, &s8("driver"));
    Offset result = small_str_concat(&arena, m, n);
    /* 11 bytes overflows SmallStrValue; should promote to StrValue */
    ASSERT_TAG_EQ(value_tag(&arena, result), TAG_STR);
    StrValue *val = str_resolve(&arena, result);
    ASSERT_U32_EQ(val->length_in_bytes, 11);
    ASSERT_U32_EQ(val->length_in_codepoints, 11);
    ASSERT_TRUE(memcmp(val->payload, "screwdriver", 11) == 0);
}

void concat_with_multibyte(void) {
    Arena arena;
    arena_init(&arena);
    Offset m = small_str_new(&arena, &s8("\xC3\xA9"));
    Offset n = small_str_new(&arena, &s8("x"));
    Offset result = small_str_concat(&arena, m, n);
    SmallStrValue *val = small_str_resolve(&arena, result);
    ASSERT_TAG_EQ(val->tag, TAG_SMALL_STR);
    u8 expected[] = { 0xC3, 0xA9, 'x' };
    ASSERT_SMALL_STR_EQ_BYTES(val, expected, 3);
}

void concat_creates_distinct_value(void) {
    Arena arena;
    arena_init(&arena);
    Offset m = small_str_new(&arena, &s8("a"));
    Offset n = small_str_new(&arena, &s8("b"));
    Offset result = small_str_concat(&arena, m, n);
    ASSERT_OFFSET_NE(result, m);
    ASSERT_OFFSET_NE(result, n);
}

void validate_empty_string(void) {
    Arena arena;
    arena_init(&arena);
    Offset offset = small_str_new(&arena, &s8(""));
    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(small_str_validate(&arena, offset, &seenset));
}

void validate_ascii(void) {
    Arena arena;
    arena_init(&arena);
    Offset offset = small_str_new(&arena, &s8("yowza!"));
    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(small_str_validate(&arena, offset, &seenset));
}

void validate_multibyte(void) {
    Arena arena;
    arena_init(&arena);
    Offset offset = small_str_new(&arena, &s8("\xE9\x81\x93"));
    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(small_str_validate(&arena, offset, &seenset));
}

/* Helper: manually construct a malformed SmallStrValue in the arena */
#define MAKE_MALFORMED_SMALL_STR(name, payload_bytes) do { \
    Arena arena; \
    arena_init(&arena); \
    SmallStrValue *sv = arena_alloc( \
        &arena, \
        sizeof(SmallStrValue), \
        alignof(SmallStrValue) \
    ); \
    sv->tag = TAG_SMALL_STR; \
    memcpy(sv->payload, (payload_bytes), 8); \
    Offset name = (Offset)((unsigned char *)sv - arena.bytes); \
    SeenSet seenset; \
    seenset_init(&seenset); \
    ASSERT_TRUE(!small_str_validate(&arena, name, &seenset)); \
} while (0)

void validate_rejects_malformed_utf8(void) {
    /* Invalid leading byte 0xFE in the content region */
    u8 bad[] = { 0, 0, 0, 0, 0, 0, 0, 0xFE };
    MAKE_MALFORMED_SMALL_STR(offset, bad);
}

void validate_rejects_truncated_utf8(void) {
    /* Leading byte 0xC3 without continuation */
    u8 bad[] = { 0, 0, 0, 0, 0, 0, 0, 0xC3 };
    MAKE_MALFORMED_SMALL_STR(offset, bad);
}

int main(void) {
    PLAN(23);
    RUN_TEST(create_empty_string);
    RUN_TEST(create_single_ascii);
    RUN_TEST(create_exactly_eight_bytes);
    RUN_TEST(create_two_byte_utf8);
    RUN_TEST(create_three_byte_utf8);
    RUN_TEST(create_four_byte_utf8);
    RUN_TEST(create_mixed_ascii_and_multibyte);
    RUN_TEST(create_five_byte_leaves_padding);
    RUN_TEST(concat_two_small_strings);
    RUN_TEST(concat_with_empty_left);
    RUN_TEST(concat_with_empty_right);
    RUN_TEST(concat_both_empty);
    RUN_TEST(concat_exactly_eight);
    RUN_TEST(concat_seven_plus_one);
    RUN_TEST(concat_eight_plus_empty);
    RUN_TEST(concat_overflow_to_str);
    RUN_TEST(concat_with_multibyte);
    RUN_TEST(concat_creates_distinct_value);
    RUN_TEST(validate_empty_string);
    RUN_TEST(validate_ascii);
    RUN_TEST(validate_multibyte);
    RUN_TEST(validate_rejects_malformed_utf8);
    RUN_TEST(validate_rejects_truncated_utf8);
    TEST_SUMMARY();
}

