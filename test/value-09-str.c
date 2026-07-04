#include <stdint.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/seenset.h"
#include "../include/str-value.h"
#include "../include/tags.h"
#include "../include/test.h"
#include "../include/typedefs.h"
#include "../include/value.h"

/* Compare a StrValue's payload against expected UTF-8 bytes */
#define ASSERT_STR_EQ_BYTES(val, expected_bytes, expected_len) do { \
    ASSERT_U32_EQ((val)->length_in_bytes, (expected_len)); \
    ASSERT_TRUE(memcmp( \
        (val)->payload, \
        (expected_bytes), \
        (expected_len) \
    ) == 0); \
} while (0)

/* Create a StrValue from a string literal and assert basic properties */
#define ASSERT_STR_NEW(s, expected_bytes, expected_byte_len, expected_cp) do \
{ \
    Arena arena; \
    arena_init(&arena); \
    s8 str = s8(s); \
    Offset offset = str_new(&arena, &str); \
    StrValue *val = str_resolve(&arena, offset); \
    ASSERT_TAG_EQ(val->tag, TAG_STR); \
    ASSERT_U32_EQ(val->length_in_bytes, (expected_byte_len)); \
    ASSERT_U32_EQ(val->length_in_codepoints, (expected_cp)); \
    ASSERT_TRUE(memcmp( \
        val->payload, \
        (expected_bytes), \
        (expected_byte_len) \
    ) == 0); \
} while (0)

void create_empty_string(void) {
    ASSERT_STR_NEW("", "", 0, 0);
}

void create_ascii_only(void) {
    ASSERT_STR_NEW("hello", "hello", 5, 5);
}

void create_with_newline_escape(void) {
    ASSERT_STR_NEW("a\nb", "a\nb", 3, 3);
}

void create_with_two_byte_utf8(void) {
    /* U+00E9 LATIN SMALL LETTER E WITH ACUTE: C3 A9 */
    u8 bytes[] = { 0xC3, 0xA9 };
    ASSERT_STR_NEW("\xC3\xA9", bytes, 2, 1);
}

void create_with_three_byte_utf8(void) {
    /* U+9053 CJK UNIFIED IDEOGRAPH-9053: E9 81 93 */
    u8 bytes[] = { 0xE9, 0x81, 0x93 };
    ASSERT_STR_NEW("\xE9\x81\x93", bytes, 3, 1);
}

void create_with_four_byte_utf8(void) {
    /* U+1F600 GRINNING FACE: F0 9F 98 80 */
    u8 bytes[] = { 0xF0, 0x9F, 0x98, 0x80 };
    ASSERT_STR_NEW("\xF0\x9F\x98\x80", bytes, 4, 1);
}

void create_mixed_ascii_and_multibyte(void) {
    /* "aéb" = a + U+00E9 + b */
    u8 bytes[] = { 'a', 0xC3, 0xA9, 'b' };
    ASSERT_STR_NEW("a\xC3\xA9" "b", bytes, 4, 3);
}

void create_multiple_multibyte_characters(void) {
    /* "éé" = two U+00E9 */
    u8 bytes[] = { 0xC3, 0xA9, 0xC3, 0xA9 };
    ASSERT_STR_NEW("\xC3\xA9\xC3\xA9", bytes, 4, 2);
}

void concat_two_ascii_strings(void) {
    Arena arena;
    arena_init(&arena);
    Offset m = str_new(&arena, &s8("hello"));
    Offset n = str_new(&arena, &s8("world"));
    Offset result = str_concat(&arena, m, n);
    StrValue *val = str_resolve(&arena, result);
    ASSERT_TAG_EQ(val->tag, TAG_STR);
    ASSERT_STR_EQ_BYTES(val, "helloworld", 10);
    ASSERT_U32_EQ(val->length_in_codepoints, 10);
}

void concat_with_multibyte(void) {
    Arena arena;
    arena_init(&arena);
    Offset m = str_new(&arena, &s8("\xC3\xA9"));            /* é */
    Offset n = str_new(&arena, &s8("\xF0\x9F\x98\x80"));    /* grinning face */
    Offset result = str_concat(&arena, m, n);
    StrValue *val = str_resolve(&arena, result);
    u8 expected[] = { 0xC3, 0xA9, 0xF0, 0x9F, 0x98, 0x80 };
    ASSERT_STR_EQ_BYTES(val, expected, 6);
    ASSERT_U32_EQ(val->length_in_codepoints, 2);
}

void concat_empty_left(void) {
    Arena arena;
    arena_init(&arena);
    Offset m = str_new(&arena, &s8(""));
    Offset n = str_new(&arena, &s8("hello"));
    Offset result = str_concat(&arena, m, n);
    StrValue *val = str_resolve(&arena, result);
    ASSERT_STR_EQ_BYTES(val, "hello", 5);
    ASSERT_U32_EQ(val->length_in_codepoints, 5);
}

void concat_empty_right(void) {
    Arena arena;
    arena_init(&arena);
    Offset m = str_new(&arena, &s8("hello"));
    Offset n = str_new(&arena, &s8(""));
    Offset result = str_concat(&arena, m, n);
    StrValue *val = str_resolve(&arena, result);
    ASSERT_STR_EQ_BYTES(val, "hello", 5);
    ASSERT_U32_EQ(val->length_in_codepoints, 5);
}

void concat_both_empty(void) {
    Arena arena;
    arena_init(&arena);
    Offset m = str_new(&arena, &s8(""));
    Offset n = str_new(&arena, &s8(""));
    Offset result = str_concat(&arena, m, n);
    StrValue *val = str_resolve(&arena, result);
    ASSERT_STR_EQ_BYTES(val, "", 0);
    ASSERT_U32_EQ(val->length_in_codepoints, 0);
}

void concat_preserves_counts(void) {
    Arena arena;
    arena_init(&arena);
    Offset m = str_new(&arena, &s8("\xC3\xA9\xC3\xA9")); /* éé: 4 bs, 2 cp */
    Offset n = str_new(&arena, &s8("a"));                /*  a: 1 b, 1 cp */
    Offset result = str_concat(&arena, m, n);
    StrValue *val = str_resolve(&arena, result);
    ASSERT_U32_EQ(val->length_in_bytes, 5);
    ASSERT_U32_EQ(val->length_in_codepoints, 3);
}

void concat_creates_distinct_value(void) {
    Arena arena;
    arena_init(&arena);
    Offset m = str_new(&arena, &s8("x"));
    Offset n = str_new(&arena, &s8("y"));
    Offset result = str_concat(&arena, m, n);
    ASSERT_OFFSET_NE(result, m);
    ASSERT_OFFSET_NE(result, n);
}

void validate_empty_string(void) {
    Arena arena;
    arena_init(&arena);
    Offset offset = str_new(&arena, &s8(""));
    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(str_validate(&arena, offset, &seenset));
}

void validate_ascii(void) {
    Arena arena;
    arena_init(&arena);
    Offset offset = str_new(&arena, &s8("hello"));
    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(str_validate(&arena, offset, &seenset));
}

void validate_multibyte(void) {
    Arena arena;
    arena_init(&arena);
    Offset offset = str_new(&arena, &s8("\xE9\x81\x93"));
    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(str_validate(&arena, offset, &seenset));
}

void generic_to_str_identity_on_str(void) {
    Arena arena;
    arena_init(&arena);
    s8 str = s8("hello");
    Offset str_offset = str_new(&arena, &str);
    Offset result_offset = generic_to_str(&arena, str_offset);
    ASSERT_OFFSET_EQ(result_offset, str_offset);
}

/* Helper: manually construct a malformed StrValue in the arena */
#define MAKE_MALFORMED_STR(name, payload_bytes, payload_len, declared_cp) do \
{ \
    Arena arena; \
    arena_init(&arena); \
    StrValue *sv = arena_alloc( \
        &arena, \
        sizeof(StrValue) + (payload_len), \
        alignof(StrValue) \
    ); \
    sv->tag = TAG_STR; \
    sv->length_in_bytes = (payload_len); \
    sv->length_in_codepoints = (declared_cp); \
    memcpy(sv->payload, (payload_bytes), (payload_len)); \
    Offset name = (Offset)((unsigned char *)sv - arena.bytes); \
    SeenSet seenset; \
    seenset_init(&seenset); \
    ASSERT_TRUE(!str_validate(&arena, name, &seenset)); \
} while (0)

void validate_rejects_truncated_two_byte(void) {
    u8 bad[] = { 0xC3 };                /* leading byte without continuation */
    MAKE_MALFORMED_STR(offset, bad, 1, 1);
}

void validate_rejects_truncated_three_byte(void) {
    u8 bad[] = { 0xE4, 0xBA };          /* missing final continuation */
    MAKE_MALFORMED_STR(offset, bad, 2, 1);
}

void validate_rejects_truncated_four_byte(void) {
    u8 bad[] = { 0xF0, 0x9F, 0x98 };    /* missing final continuation */
    MAKE_MALFORMED_STR(offset, bad, 3, 1);
}

void validate_rejects_invalid_continuation(void) {
    u8 bad[] = { 0xC3, 0x00 };          /* 0x00 is not a continuation byte */
    MAKE_MALFORMED_STR(offset, bad, 2, 1);
}

void validate_rejects_overlong_two_byte(void) {
    /* U+0041 'A' encoded as two bytes: C1 81 (overlong) */
    u8 bad[] = { 0xC1, 0x81 };
    MAKE_MALFORMED_STR(offset, bad, 2, 1);
}

void validate_rejects_overlong_three_byte(void) {
    /* U+0041 'A' encoded as three bytes: E0 81 81 (overlong) */
    u8 bad[] = { 0xE0, 0x81, 0x81 };
    MAKE_MALFORMED_STR(offset, bad, 3, 1);
}

void validate_rejects_surrogate_half(void) {
    /* U+D800 encoded in UTF-8: ED A0 80 (surrogate half, invalid) */
    u8 bad[] = { 0xED, 0xA0, 0x80 };
    MAKE_MALFORMED_STR(offset, bad, 3, 1);
}

void validate_rejects_beyond_unicode(void) {
    /* U+110000 encoded in UTF-8: F4 90 80 80 (beyond max valid U+10FFFF) */
    u8 bad[] = { 0xF4, 0x90, 0x80, 0x80 };
    MAKE_MALFORMED_STR(offset, bad, 4, 1);
}

void validate_rejects_invalid_leading_byte(void) {
    u8 bad[] = { 0xFE };
    MAKE_MALFORMED_STR(offset, bad, 1, 1);
}

void validate_rejects_lone_continuation_byte(void) {
    u8 bad[] = { 0x80 };    /* bare continuation byte */
    MAKE_MALFORMED_STR(offset, bad, 1, 1);
}

void validate_rejects_codepoint_count_mismatch(void) {
    /* Valid UTF-8 but wrong declared codepoint count: "hi" is 2 cp, claim 3 */
    Arena arena;
    arena_init(&arena);
    StrValue *sv = arena_alloc(
        &arena,
        sizeof(StrValue) + 2,
        alignof(StrValue)
    );
    sv->tag = TAG_STR;
    sv->length_in_bytes = 2;
    sv->length_in_codepoints = 99;      /* deliberately wrong */
    memcpy(sv->payload, "hi", 2);
    Offset offset = (Offset)((unsigned char *)sv - arena.bytes);
    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(!str_validate(&arena, offset, &seenset));
}

int main(void) {
    PLAN(30);
    RUN_TEST(create_empty_string);
    RUN_TEST(create_ascii_only);
    RUN_TEST(create_with_newline_escape);
    RUN_TEST(create_with_two_byte_utf8);
    RUN_TEST(create_with_three_byte_utf8);
    RUN_TEST(create_with_four_byte_utf8);
    RUN_TEST(create_mixed_ascii_and_multibyte);
    RUN_TEST(create_multiple_multibyte_characters);
    RUN_TEST(concat_two_ascii_strings);
    RUN_TEST(concat_with_multibyte);
    RUN_TEST(concat_empty_left);
    RUN_TEST(concat_empty_right);
    RUN_TEST(concat_both_empty);
    RUN_TEST(concat_preserves_counts);
    RUN_TEST(concat_creates_distinct_value);
    RUN_TEST(validate_empty_string);
    RUN_TEST(validate_ascii);
    RUN_TEST(validate_multibyte);
    RUN_TEST(generic_to_str_identity_on_str);
    RUN_TEST(validate_rejects_truncated_two_byte);
    RUN_TEST(validate_rejects_truncated_three_byte);
    RUN_TEST(validate_rejects_truncated_four_byte);
    RUN_TEST(validate_rejects_invalid_continuation);
    RUN_TEST(validate_rejects_overlong_two_byte);
    RUN_TEST(validate_rejects_overlong_three_byte);
    RUN_TEST(validate_rejects_surrogate_half);
    RUN_TEST(validate_rejects_beyond_unicode);
    RUN_TEST(validate_rejects_invalid_leading_byte);
    RUN_TEST(validate_rejects_lone_continuation_byte);
    RUN_TEST(validate_rejects_codepoint_count_mismatch);
    TEST_SUMMARY();
}

