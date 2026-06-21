#include <stdint.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/ascii-str-value.h"
#include "../include/int-value.h"
#include "../include/seenset.h"
#include "../include/tags.h"
#include "../include/test.h"

#define ASSERT_INT_EQ(actual_offset, expected_offset) do { \
    IntValue *_actual = int_resolve(&arena, actual_offset); \
    IntValue *_expected = int_resolve(&arena, expected_offset); \
    ASSERT_U32_EQ(_actual->sign, _expected->sign); \
    ASSERT_U32_EQ(_actual->length, _expected->length); \
    for (u32 _i = 0; _i < _actual->length; _i++) { \
        ASSERT_U32_EQ(_actual->payload[_i], _expected->payload[_i]); \
    } \
} while (0)

#define ASSERT_INT_ADD(m_offset, n_offset, expected_offset) do { \
    Offset result = int_add(&arena, m_offset, n_offset); \
    ASSERT_INT_EQ(result, expected_offset); \
} while (0)

#define ASSERT_INT_SUBTRACT(m_offset, n_offset, expected_offset) do { \
    Offset result = int_subtract(&arena, m_offset, n_offset); \
    ASSERT_INT_EQ(result, expected_offset); \
} while (0)

#define ASSERT_INT_MULTIPLY(m_offset, n_offset, expected_offset) do { \
    Offset result = int_multiply(&arena, m_offset, n_offset); \
    ASSERT_INT_EQ(result, expected_offset); \
} while (0)

#define ASSERT_INT_DIVIDE(m_offset, n_offset, fallback_offset, \
        expected_offset) do { \
    Offset result = int_divide(&arena, m_offset, n_offset, fallback_offset); \
    ASSERT_INT_EQ(result, expected_offset); \
} while (0)

#define ASSERT_INT_MODULO(m_offset, n_offset, fallback_offset, \
        expected_offset) do { \
    Offset result = int_modulo(&arena, m_offset, n_offset, fallback_offset); \
    ASSERT_INT_EQ(result, expected_offset); \
} while (0)

static Arena arena;

static Offset i32_int(i32 v) {
    if (v == 0) {
        return int_new(&arena, 0, 0, NULL);
    }
    u32 sign = v < 0 ? 1 : 0;
    u32 limb = sign ? (u32)(-(i64)v) : (u32)v;
    return int_new(&arena, sign, 1, &limb);
}

static Offset u32_int(u32 v) {
    if (v == 0) {
        return int_new(&arena, 0, 0, NULL);
    }
    return int_new(&arena, 0, 1, &v);
}

/* Construct an IntValue from two u32 limbs */
static Offset limbs2(u32 sign, u32 limb1, u32 limb2) {
    u32 payload[] = { limb1, limb2 };
    return int_new(&arena, sign, 2, payload);
}

#define ASSERT_INT_TO_STR(int_offset, expected) do { \
    Offset str_offset = int_to_str(&arena, int_offset); \
    AsciiStrValue *str = ascii_str_resolve(&arena, str_offset); \
    char _buf[256]; \
    ASSERT_TRUE(str->length_in_bytes < sizeof(_buf)); \
    memcpy(_buf, str->payload, str->length_in_bytes); \
    _buf[str->length_in_bytes] = '\0'; \
    ASSERT_STR_EQ(_buf, expected); \
} while (0)

void create_zero(void) {
    arena_init(&arena);
    Offset z = int_new(&arena, 0, 0, NULL);
    IntValue *v = int_resolve(&arena, z);
    ASSERT_TAG_EQ(v->tag, TAG_INT);
    ASSERT_I64_EQ(v->sign, 0);
    ASSERT_I64_EQ(v->length, 0);

    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(int_validate(&arena, z, &seenset));
}

void create_positive_one_limb(void) {
    arena_init(&arena);
    u32 payload[] = { 42 };
    Offset n = int_new(&arena, 0, 1, payload);
    IntValue *v = int_resolve(&arena, n);
    ASSERT_TAG_EQ(v->tag, TAG_INT);
    ASSERT_I64_EQ(v->sign, 0);
    ASSERT_I64_EQ(v->length, 1);
    ASSERT_I64_EQ(v->payload[0], 42);
}

void create_negative_one_limb(void) {
    arena_init(&arena);
    u32 payload[] = { 17 };
    Offset n = int_new(&arena, 1, 1, payload);
    IntValue *v = int_resolve(&arena, n);
    ASSERT_TAG_EQ(v->tag, TAG_INT);
    ASSERT_I64_EQ(v->sign, 1);
    ASSERT_I64_EQ(v->length, 1);
    ASSERT_I64_EQ(v->payload[0], 17);
}

void create_leading_zeros_normalized(void) {
    arena_init(&arena);
    u32 payload[] = { 42, 0, 0 };
    Offset n = int_new(&arena, 0, 3, payload);
    IntValue *v = int_resolve(&arena, n);
    ASSERT_I64_EQ(v->length, 1);
    ASSERT_I64_EQ(v->payload[0], 42);
}

void validate_zero_with_bad_sign(void) {
    arena_init(&arena);
    IntValue *v = arena_alloc(&arena, sizeof(IntValue), alignof(IntValue));
    v->tag = TAG_INT;
    v->sign = 1;
    v->length = 0;
    Offset bad = (Offset)((unsigned char *)v - arena.bytes);

    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(!int_validate(&arena, bad, &seenset));
}

void validate_leading_zero_limb(void) {
    arena_init(&arena);
    IntValue *v = arena_alloc(
        &arena,
        sizeof(IntValue) + 2 * sizeof(u32),
        alignof(IntValue)
    );
    v->tag = TAG_INT;
    v->sign = 0;
    v->length = 2;
    v->payload[0] = 1;
    v->payload[1] = 0;  /* leading zero - invalid */
    Offset bad = (Offset)((unsigned char *)v - arena.bytes);

    SeenSet seenset;
    seenset_init(&seenset);
    ASSERT_TRUE(!int_validate(&arena, bad, &seenset));
}

void add_positive_same_sign(void) {
    arena_init(&arena);
    Offset a = i32_int(5);
    Offset b = i32_int(7);
    Offset expected = i32_int(12);
    ASSERT_INT_ADD(a, b, expected);
}

void add_negative_same_sign(void) {
    arena_init(&arena);
    Offset a = i32_int(-5);
    Offset b = i32_int(-7);
    Offset expected = i32_int(-12);
    ASSERT_INT_ADD(a, b, expected);
}

void add_mixed_sign_cancels(void) {
    arena_init(&arena);
    Offset a = i32_int(5);
    Offset b = i32_int(-5);
    Offset expected = i32_int(0);
    ASSERT_INT_ADD(a, b, expected);
}

void add_mixed_sign_positive_result(void) {
    arena_init(&arena);
    Offset a = i32_int(10);
    Offset b = i32_int(-3);
    Offset expected = i32_int(7);
    ASSERT_INT_ADD(a, b, expected);
}

void add_mixed_sign_negative_result(void) {
    arena_init(&arena);
    Offset a = i32_int(3);
    Offset b = i32_int(-10);
    Offset expected = i32_int(-7);
    ASSERT_INT_ADD(a, b, expected);
}

void add_zero_identity(void) {
    arena_init(&arena);
    Offset a = i32_int(42);
    Offset z = i32_int(0);
    ASSERT_INT_ADD(a, z, a);
    ASSERT_INT_ADD(z, a, a);
}

void add_multi_limb_carry(void) {
    arena_init(&arena);
    Offset a = u32_int(0xFFFFFFFF);
    Offset b = i32_int(1);
    Offset expected = limbs2(0, 0x00000000, 0x00000001);
    ASSERT_INT_ADD(a, b, expected);
}

void subtract_positive_result(void) {
    arena_init(&arena);
    Offset a = i32_int(10);
    Offset b = i32_int(3);
    Offset expected = i32_int(7);
    ASSERT_INT_SUBTRACT(a, b, expected);
}

void subtract_negative_result(void) {
    arena_init(&arena);
    Offset a = i32_int(3);
    Offset b = i32_int(10);
    Offset expected = i32_int(-7);
    ASSERT_INT_SUBTRACT(a, b, expected);
}

void subtract_self_is_zero(void) {
    arena_init(&arena);
    Offset a = i32_int(42);
    Offset expected = i32_int(0);
    ASSERT_INT_SUBTRACT(a, a, expected);
}

void subtract_from_zero(void) {
    arena_init(&arena);
    Offset z = i32_int(0);
    Offset a = i32_int(5);
    Offset expected = i32_int(-5);
    ASSERT_INT_SUBTRACT(z, a, expected);
}

void subtract_zero_identity(void) {
    arena_init(&arena);
    Offset a = i32_int(42);
    Offset z = i32_int(0);
    ASSERT_INT_SUBTRACT(a, z, a);
}

void multiply_positive(void) {
    arena_init(&arena);
    Offset a = i32_int(6);
    Offset b = i32_int(7);
    Offset expected = i32_int(42);
    ASSERT_INT_MULTIPLY(a, b, expected);
}

void multiply_negative_times_negative(void) {
    arena_init(&arena);
    Offset a = i32_int(-6);
    Offset b = i32_int(-7);
    Offset expected = i32_int(42);
    ASSERT_INT_MULTIPLY(a, b, expected);
}

void multiply_mixed_signs(void) {
    arena_init(&arena);
    Offset a = i32_int(-6);
    Offset b = i32_int(7);
    Offset expected = i32_int(-42);
    ASSERT_INT_MULTIPLY(a, b, expected);
}

void multiply_by_zero(void) {
    arena_init(&arena);
    Offset a = i32_int(42);
    Offset z = i32_int(0);
    ASSERT_INT_MULTIPLY(a, z, z);
}

void multiply_by_one(void) {
    arena_init(&arena);
    Offset a = i32_int(-42);
    Offset b = i32_int(1);
    ASSERT_INT_MULTIPLY(a, b, a);
}

void multiply_multi_limb(void) {
    arena_init(&arena);
    Offset a = i32_int(0x00010000);
    Offset b = i32_int(0x00010000);
    Offset expected = limbs2(0, 0x00000000, 0x00000001);
    ASSERT_INT_MULTIPLY(a, b, expected);
}

void divide_positive_exact(void) {
    arena_init(&arena);
    Offset a = i32_int(42);
    Offset b = i32_int(6);
    Offset fallback = i32_int(999);
    Offset expected = i32_int(7);
    ASSERT_INT_DIVIDE(a, b, fallback, expected);
}

void divide_positive_floored(void) {
    arena_init(&arena);
    Offset a = i32_int(17);
    Offset b = i32_int(5);
    Offset fallback = i32_int(999);
    Offset expected = i32_int(3);
    ASSERT_INT_DIVIDE(a, b, fallback, expected);
}

void divide_negative_floored(void) {
    arena_init(&arena);
    Offset a = i32_int(-17);
    Offset b = i32_int(5);
    Offset fallback = i32_int(999);
    Offset expected = i32_int(-4);
    ASSERT_INT_DIVIDE(a, b, fallback, expected);
}

void divide_positive_num_negative_den(void) {
    arena_init(&arena);
    Offset a = i32_int(17);
    Offset b = i32_int(-5);
    Offset fallback = i32_int(999);
    Offset expected = i32_int(-4);
    ASSERT_INT_DIVIDE(a, b, fallback, expected);
}

void divide_negative_both(void) {
    arena_init(&arena);
    Offset a = i32_int(-17);
    Offset b = i32_int(-5);
    Offset fallback = i32_int(999);
    Offset expected = i32_int(3);
    ASSERT_INT_DIVIDE(a, b, fallback, expected);
}

void divide_by_zero_fallback(void) {
    arena_init(&arena);
    Offset a = i32_int(42);
    Offset b = i32_int(0);
    Offset fallback = i32_int(999);
    ASSERT_INT_DIVIDE(a, b, fallback, fallback);
}

void divide_zero_numerator(void) {
    arena_init(&arena);
    Offset a = i32_int(0);
    Offset b = i32_int(5);
    Offset fallback = i32_int(999);
    Offset expected = i32_int(0);
    ASSERT_INT_DIVIDE(a, b, fallback, expected);
}

void divide_magnitude_less_than_one(void) {
    arena_init(&arena);
    Offset a = i32_int(3);
    Offset b = i32_int(5);
    Offset fallback = i32_int(999);
    Offset expected = i32_int(0);
    ASSERT_INT_DIVIDE(a, b, fallback, expected);
}

void divide_magnitude_less_opposite_sign(void) {
    arena_init(&arena);
    Offset a = i32_int(3);
    Offset b = i32_int(-5);
    Offset fallback = i32_int(999);
    Offset expected = i32_int(-1);
    ASSERT_INT_DIVIDE(a, b, fallback, expected);
}

void modulo_positive_exact(void) {
    arena_init(&arena);
    Offset a = i32_int(42);
    Offset b = i32_int(6);
    Offset fallback = i32_int(999);
    Offset expected = i32_int(0);
    ASSERT_INT_MODULO(a, b, fallback, expected);
}

void modulo_positive_with_remainder(void) {
    arena_init(&arena);
    Offset a = i32_int(17);
    Offset b = i32_int(5);
    Offset fallback = i32_int(999);
    Offset expected = i32_int(2);
    ASSERT_INT_MODULO(a, b, fallback, expected);
}

void modulo_negative_dividend(void) {
    arena_init(&arena);
    Offset a = i32_int(-17);
    Offset b = i32_int(5);
    Offset fallback = i32_int(999);
    Offset expected = i32_int(3);
    ASSERT_INT_MODULO(a, b, fallback, expected);
}

void modulo_negative_divisor(void) {
    arena_init(&arena);
    Offset a = i32_int(17);
    Offset b = i32_int(-5);
    Offset fallback = i32_int(999);
    /* 17 = (-4) * (-5) + (-3) */
    Offset expected = i32_int(-3);
    ASSERT_INT_MODULO(a, b, fallback, expected);
}

void modulo_negative_both(void) {
    arena_init(&arena);
    Offset a = i32_int(-17);
    Offset b = i32_int(-5);
    Offset fallback = i32_int(999);
    /* 17 = (-4) * (-5) + (-3) */
    Offset expected = i32_int(-2);
    ASSERT_INT_MODULO(a, b, fallback, expected);
}

void modulo_by_zero_fallback(void) {
    arena_init(&arena);
    Offset a = i32_int(42);
    Offset b = i32_int(0);
    Offset fallback = i32_int(-1);
    ASSERT_INT_MODULO(a, b, fallback, fallback);
}

void modulo_zero_numerator(void) {
    arena_init(&arena);
    Offset a = i32_int(0);
    Offset b = i32_int(5);
    Offset fallback = i32_int(999);
    Offset expected = i32_int(0);
    ASSERT_INT_MODULO(a, b, fallback, expected);
}

void modulo_magnitude_less_same_sign(void) {
    arena_init(&arena);
    Offset a = i32_int(3);
    Offset b = i32_int(5);
    Offset fallback = i32_int(999);
    ASSERT_INT_MODULO(a, b, fallback, a);
}

void modulo_magnitude_less_opposite_sign(void) {
    arena_init(&arena);
    Offset a = i32_int(3);
    Offset b = i32_int(-5);
    Offset fallback = i32_int(999);
    Offset expected = i32_int(-2);
    ASSERT_INT_MODULO(a, b, fallback, expected);
}

void to_str_zero(void) {
    arena_init(&arena);
    Offset z = i32_int(0);
    ASSERT_INT_TO_STR(z, "0");
}

void to_str_positive(void) {
    arena_init(&arena);
    Offset n = i32_int(42);
    ASSERT_INT_TO_STR(n, "42");
}

void to_str_negative(void) {
    arena_init(&arena);
    Offset n = i32_int(-17);
    ASSERT_INT_TO_STR(n, "-17");
}

void to_str_large_multi_limb(void) {
    arena_init(&arena);
    Offset n = limbs2(0, 0x00000000, 0x00000001);
    ASSERT_INT_TO_STR(n, "4294967296");
}

void to_str_negative_multi_limb(void) {
    arena_init(&arena);
    Offset n = limbs2(1, 0x00000000, 0x00000001);
    ASSERT_INT_TO_STR(n, "-4294967296");
}

void one_hundred_factorial(void) {
    arena_init(&arena);
    Offset product_offset = i32_int(1);
    for (i32 i = 2; i <= 100; i++) {
        Offset factor_offset = i32_int(i);
        product_offset = int_multiply(&arena, product_offset, factor_offset);
    }
    IntValue *actual = int_resolve(&arena, product_offset);
    ASSERT_U32_EQ(actual->sign, 0);
    ASSERT_U32_EQ(actual->length, 17);
    u32 expected_limbs[] = {
        0, 0, 0, 657835546, 4002087658, 3015126738, 2485176044, 1163332810,
        3742382359, 2486841884, 3944854962, 1726978672, 2752321257, 685067197,
        3693348501, 2521744277, 6960
    };
    for (u32 i = 0; i < 17; i++) {
        ASSERT_U32_EQ(actual->payload[i], expected_limbs[i]);
    }
}

int main(void) {
    PLAN(48);

    RUN_TEST(create_zero);
    RUN_TEST(create_positive_one_limb);
    RUN_TEST(create_negative_one_limb);
    RUN_TEST(create_leading_zeros_normalized);
    RUN_TEST(validate_zero_with_bad_sign);
    RUN_TEST(validate_leading_zero_limb);

    RUN_TEST(add_positive_same_sign);
    RUN_TEST(add_negative_same_sign);
    RUN_TEST(add_mixed_sign_cancels);
    RUN_TEST(add_mixed_sign_positive_result);
    RUN_TEST(add_mixed_sign_negative_result);
    RUN_TEST(add_zero_identity);
    RUN_TEST(add_multi_limb_carry);

    RUN_TEST(subtract_positive_result);
    RUN_TEST(subtract_negative_result);
    RUN_TEST(subtract_self_is_zero);
    RUN_TEST(subtract_from_zero);
    RUN_TEST(subtract_zero_identity);

    RUN_TEST(multiply_positive);
    RUN_TEST(multiply_negative_times_negative);
    RUN_TEST(multiply_mixed_signs);
    RUN_TEST(multiply_by_zero);
    RUN_TEST(multiply_by_one);
    RUN_TEST(multiply_multi_limb);

    RUN_TEST(divide_positive_exact);
    RUN_TEST(divide_positive_floored);
    RUN_TEST(divide_negative_floored);
    RUN_TEST(divide_positive_num_negative_den);
    RUN_TEST(divide_negative_both);
    RUN_TEST(divide_by_zero_fallback);
    RUN_TEST(divide_zero_numerator);
    RUN_TEST(divide_magnitude_less_than_one);
    RUN_TEST(divide_magnitude_less_opposite_sign);

    RUN_TEST(modulo_positive_exact);
    RUN_TEST(modulo_positive_with_remainder);
    RUN_TEST(modulo_negative_dividend);
    RUN_TEST(modulo_negative_divisor);
    RUN_TEST(modulo_negative_both);
    RUN_TEST(modulo_by_zero_fallback);
    RUN_TEST(modulo_zero_numerator);
    RUN_TEST(modulo_magnitude_less_same_sign);
    RUN_TEST(modulo_magnitude_less_opposite_sign);

    RUN_TEST(to_str_zero);
    RUN_TEST(to_str_positive);
    RUN_TEST(to_str_negative);
    RUN_TEST(to_str_large_multi_limb);
    RUN_TEST(to_str_negative_multi_limb);

    RUN_TEST(one_hundred_factorial);

    TEST_SUMMARY();
}

