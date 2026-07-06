#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/ascii-str-value.h"
#include "../include/crash.h"
#include "../include/int-value.h"
#include "../include/seenset.h"
#include "../include/tags.h"
#include "../include/typedefs.h"
#include "../include/value.h"

static void int_normalize(IntValue *v) {
    while (v->length > 0 && v->payload[v->length - 1] == 0) {
        v->length--;
    }
    if (v->length == 0) {
        v->sign = 0;
    }
}

static bool int_is_zero(IntValue *v) {
    return v->length == 0;
}

/* Compare magnitudes of two normalized IntValues, ignoring sign.
   -1, 0, 1 for less, equal, greater. */
static int compare_magnitude(IntValue *a, IntValue *b) {
    if (a->length != b->length) {
        return a->length < b->length ? -1 : 1;
    }
    for (u32 i = a->length; i-- > 0; ) {
        if (a->payload[i] != b->payload[i]) {
            return a->payload[i] < b->payload[i] ? -1 : 1;
        }
    }
    return 0;
}

/* Compare two raw u32 arrays (ignoring any leading zero limbs). */
static int compare_u32_arrays(u32 *a, u32 a_len, u32 *b, u32 b_len) {
    u32 a_eff = a_len;
    while (a_eff > 0 && a[a_eff - 1] == 0) {
        a_eff--;
    }
    u32 b_eff = b_len;
    while (b_eff > 0 && b[b_eff - 1] == 0) {
        b_eff--;
    }

    if (a_eff != b_eff) {
        return a_eff < b_eff ? -1 : 1;
    }
    for (u32 i = a_eff; i-- > 0; ) {
        if (a[i] != b[i]) {
            return a[i] < b[i] ? -1 : 1;
        }
    }
    return 0;
}

/* Add two positive magnitudes. */
static Offset add_magnitude(Arena *a, IntValue *lhs, IntValue *rhs) {
    u32 max_len = lhs->length > rhs->length ? lhs->length : rhs->length;
    IntValue *result = arena_alloc(
        a,
        sizeof(IntValue) + (max_len + 1) * sizeof(u32),
        alignof(IntValue)
    );
    result->tag = TAG_INT;
    result->sign = 0;
    result->length = max_len + 1;
    memset(result->payload, 0, (max_len + 1) * sizeof(u32));

    u64 carry = 0;
    for (u32 i = 0; i < max_len + 1; i++) {
        u64 sum = carry;
        if (i < lhs->length) {
            sum += lhs->payload[i];
        }
        if (i < rhs->length) {
            sum += rhs->payload[i];
        }
        result->payload[i] = (u32)sum;
        carry = sum >> 32;
    }

    int_normalize(result);
    return (Offset)((unsigned char *)result - a->bytes);
}

/* Subtract smaller magnitude 'rhs' from larger magnitude 'lhs'.
   Precondition: compare_magnitude(lhs, rhs) >= 0. */
static Offset sub_magnitude(Arena *a, IntValue *lhs, IntValue *rhs) {
    IntValue *result = arena_alloc(
        a,
        sizeof(IntValue) + lhs->length * sizeof(u32),
        alignof(IntValue)
    );
    result->tag = TAG_INT;
    result->sign = 0;
    result->length = lhs->length;

    u64 borrow = 0;
    for (u32 i = 0; i < lhs->length; i++) {
        u64 minuend = lhs->payload[i];
        u64 subtrahend = borrow;
        if (i < rhs->length) {
            subtrahend += rhs->payload[i];
        }

        if (minuend >= subtrahend) {
            result->payload[i] = (u32)(minuend - subtrahend);
            borrow = 0;
        }
        else {
            result->payload[i] = (u32)(((u64)1 << 32) + minuend - subtrahend);
            borrow = 1;
        }
    }
    assert(borrow == 0);  /* guaranteed by precondition */

    int_normalize(result);
    return (Offset)((unsigned char *)result - a->bytes);
}

/* Create a copy with the opposite sign. */
static Offset int_negate(Arena *a, Offset offset) {
    IntValue *v = int_resolve(a, offset);
    if (int_is_zero(v)) {
        return offset;
    }

    IntValue *result = arena_alloc(
        a,
        sizeof(IntValue) + v->length * sizeof(u32),
        alignof(IntValue)
    );
    result->tag = TAG_INT;
    result->sign = !v->sign;
    result->length = v->length;
    memcpy(result->payload, v->payload, v->length * sizeof(u32));
    return (Offset)((unsigned char *)result - a->bytes);
}

/* Create a nonnegative copy (absolute value). */
static Offset int_abs(Arena *a, Offset offset) {
    IntValue *v = int_resolve(a, offset);
    if (v->sign == 0) {
        return offset;
    }
    return int_negate(a, offset);
}

/* Bit-by-bit unsigned magnitude division.
   Both inputs are positive and non-zero; |dividend| >= |divisor|. */
static void divmod_magnitude(
    Arena *a,
    IntValue *dividend,
    IntValue *divisor,
    Offset *quot_out,
    Offset *rem_out
) {
    /* Find the index of the highest set bit in dividend */
    u32 total_bits = dividend->length * 32;
    u32 top = dividend->payload[dividend->length - 1];
    while (top != 0 && (top >> 31) == 0) {
        top <<= 1;
        total_bits--;
    }

    /* Quotient */
    u32 quot_len = dividend->length;
    IntValue *quot = arena_alloc(
        a,
        sizeof(IntValue) + quot_len * sizeof(u32),
        alignof(IntValue)
    );
    quot->tag = TAG_INT;
    quot->sign = 0;
    quot->length = quot_len;
    memset(quot->payload, 0, quot_len * sizeof(u32));

    /* Remainder (one extra limb for left-shift overflow) */
    u32 rem_len = divisor->length + 1;
    IntValue *rem = arena_alloc(
        a,
        sizeof(IntValue) + rem_len * sizeof(u32),
        alignof(IntValue)
    );
    rem->tag = TAG_INT;
    rem->sign = 0;
    rem->length = rem_len;
    memset(rem->payload, 0, rem_len * sizeof(u32));

    for (i32 bit = (i32)total_bits - 1; bit >= 0; bit--) {
        /* Shift remainder left by 1 */
        u32 carry = 0;
        for (u32 j = 0; j < rem_len; j++) {
            u32 new_carry = rem->payload[j] >> 31;
            rem->payload[j] = (rem->payload[j] << 1) | carry;
            carry = new_carry;
        }

        /* Bring down the current bit from dividend */
        u32 word_idx = (u32)bit / 32;
        u32 bit_idx = (u32)bit % 32;
        rem->payload[0] |= (dividend->payload[word_idx] >> bit_idx) & 1;

        /* If remainder >= divisor, subtract and set quotient bit */
        if (compare_u32_arrays(
                rem->payload,
                rem_len,
                divisor->payload,
                divisor->length
            ) >= 0) {
            u32 borrow = 0;
            for (u32 j = 0; j < divisor->length; j++) {
                u64 diff = (u64)rem->payload[j] - divisor->payload[j] - borrow;
                rem->payload[j] = (u32)diff;
                borrow = (diff >> 63) ? 1 : 0;
            }
            if (borrow && rem_len > divisor->length) {
                rem->payload[divisor->length] -= borrow;
            }
            quot->payload[word_idx] |= (1ULL << bit_idx);
        }
    }

    int_normalize(quot);
    int_normalize(rem);
    *quot_out = (Offset)((unsigned char *)quot - a->bytes);
    *rem_out = (Offset)((unsigned char *)rem - a->bytes);
}

/* Divide a big-integer magnitude by 10 in-place; return the remainder. */
static u32 divmod10(u32 *limbs, u32 len) {
    u64 rem = 0;
    for (u32 i = len; i-- > 0; ) {
        rem = (rem << 32) | limbs[i];
        limbs[i] = (u32)(rem / 10);
        rem = rem % 10;
    }
    return (u32)rem;
}

static bool magnitude_is_nonzero(u32 *limbs, u32 len) {
    for (u32 i = 0; i < len; i++) {
        if (limbs[i] != 0) {
            return true;
        }
    }
    return false;
}

static Offset int_from_i32(Arena *a, i32 payload) {
    if (payload == 0) {
        return int_new(a, 0, 0, NULL);
    }

    u32 sign = payload < 0 ? 1 : 0;
    u32 limb = sign ? (u32)(-(i64)payload) : (u32)payload;
    return int_new(a, sign, 1, &limb);
}


Offset int_new(Arena *a, u32 sign, u32 length, u32 payload[]) {
    if (length == 0) {
        IntValue *v = arena_alloc(a, sizeof(IntValue), alignof(IntValue));
        v->tag = TAG_INT;
        v->sign = 0;
        v->length = 0;
        return (Offset)((unsigned char *)v - a->bytes);
    }

    IntValue *v = arena_alloc(
        a,
        sizeof(IntValue) + length * sizeof(u32),
        alignof(IntValue)
    );
    v->tag = TAG_INT;
    v->sign = sign;
    v->length = length;

    if (payload == NULL) {
        vm_crash(CRASH_NULL);
    }
    memcpy(v->payload, payload, length * sizeof(u32));
    int_normalize(v);
    return (Offset)((unsigned char *)v - a->bytes);
}

/* Returns a pointer to an IntValue, given an offset into an arena.
 *
 * Precondition: `offset` points to an IntValue.
 */
IntValue *int_resolve(Arena *a, Offset offset) {
    if (value_tag(a, offset) != TAG_INT) {
        vm_crash(CRASH_INVALID_TAG);
    }

    assert(offset <= ARENA_SIZE - sizeof(IntValue));
    return (IntValue *)(a->bytes + offset);
}

bool int_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    assert(offset <= ARENA_SIZE - sizeof(IntValue));
    IntValue *v = int_resolve(a, offset);

    Offset total_size = sizeof(IntValue) + v->length * sizeof(u32);
    if (offset + total_size > ARENA_SIZE) {
        return false;
    }

    if (v->length == 0 && v->sign != 0) {
        return false;
    }
    if (v->length > 0 && v->payload[v->length - 1] == 0) {
        return false;   /* leading zero limb */
    }

    return true;
}

/* Add two IntValues `m` and `n`, yielding an IntValue result.
 *
 * Preconditions: `m` is a valid I64Value. `n` is a valid I64Value.
 */
Offset int_add(Arena *a, Offset m, Offset n) {
    IntValue *lhs = int_resolve(a, m);
    IntValue *rhs = int_resolve(a, n);

    if (int_is_zero(lhs)) {
        return n;
    }
    if (int_is_zero(rhs)) {
        return m;
    }

    if (lhs->sign == rhs->sign) {
        Offset result = add_magnitude(a, lhs, rhs);
        int_resolve(a, result)->sign = lhs->sign;
        return result;
    }
    else {
        int cmp = compare_magnitude(lhs, rhs);
        if (cmp == 0) {
            return int_from_i32(a, 0);
        }
        else if (cmp > 0) {
            Offset result = sub_magnitude(a, lhs, rhs);
            int_resolve(a, result)->sign = lhs->sign;
            return result;
        }
        else {
            Offset result = sub_magnitude(a, rhs, lhs);
            int_resolve(a, result)->sign = rhs->sign;
            return result;
        }
    }
}

/* Subtract two IntValues `m` and `n`, yielding an IntValue result.
 *
 * Preconditions: `m` is a valid I64Value. `n` is a valid I64Value.
 */
Offset int_subtract(Arena *a, Offset m, Offset n) {
    IntValue *lhs = int_resolve(a, m);
    IntValue *rhs = int_resolve(a, n);

    if (int_is_zero(rhs)) {
        return m;
    }
    if (int_is_zero(lhs)) {
        return int_negate(a, n);
    }

    if (lhs->sign != rhs->sign) {
        Offset result = add_magnitude(a, lhs, rhs);
        int_resolve(a, result)->sign = lhs->sign;
        return result;
    }
    else {
        int cmp = compare_magnitude(lhs, rhs);
        if (cmp == 0) {
            return int_from_i32(a, 0);
        }
        else if (cmp > 0) {
            Offset result = sub_magnitude(a, lhs, rhs);
            int_resolve(a, result)->sign = lhs->sign;
            return result;
        }
        else {
            Offset result = sub_magnitude(a, rhs, lhs);
            int_resolve(a, result)->sign = !lhs->sign;
            return result;
        }
    }
}

/* Multiply two IntValues `m` and `n`, yielding an IntValue result.
 *
 * Preconditions: `m` is a valid I64Value. `n` is a valid I64Value.
 */
Offset int_multiply(Arena *a, Offset m, Offset n) {
    IntValue *lhs = int_resolve(a, m);
    IntValue *rhs = int_resolve(a, n);

    if (int_is_zero(lhs) || int_is_zero(rhs)) {
        return int_from_i32(a, 0);
    }

    u32 result_len = lhs->length + rhs->length;
    IntValue *result = arena_alloc(
        a,
        sizeof(IntValue) + result_len * sizeof(u32),
        alignof(IntValue)
    );
    result->tag = TAG_INT;
    result->sign = lhs->sign ^ rhs->sign;
    result->length = result_len;
    memset(result->payload, 0, result_len * sizeof(u32));

    for (u32 i = 0; i < lhs->length; i++) {
        u64 carry = 0;
        for (u32 j = 0; j < rhs->length; j++) {
            u64 product = (u64)lhs->payload[i] * rhs->payload[j];
            u64 sum = product + result->payload[i + j] + carry;
            result->payload[i + j] = (u32)sum;
            carry = sum >> 32;
        }
        /* Propagate carry through the remaining higher limbs */
        u32 k = i + rhs->length;
        while (carry) {
            u64 sum = result->payload[k] + carry;
            result->payload[k] = (u32)sum;
            carry = sum >> 32;
            k++;
        }
    }

    int_normalize(result);
    return (Offset)((unsigned char *)result - a->bytes);
}

/* Divide two IntValues `m` and `n`, yielding an IntValues result. The
 * resulting quotient is rounded towards negative infinity. The `fallback` is
 * returned instead of doing the division, in case the denominator is zero.
 *
 * Preconditions: `m` is a valid IntValue. `n` is a valid IntValue.
 */
Offset int_divide(Arena *a, Offset m, Offset n, Offset fallback) {
    IntValue *lhs = int_resolve(a, m);
    IntValue *rhs = int_resolve(a, n);

    if (int_is_zero(rhs)) {
        return fallback;
    }
    if (int_is_zero(lhs)) {
        return m;
    }

    if (compare_magnitude(lhs, rhs) < 0) {
        /* |lhs| < |rhs|  =>  truncating quotient is 0
           Flooring: if signs differ, quotient is -1; otherwise 0. */
        if (lhs->sign != rhs->sign) {
            return int_from_i32(a, -1);
        }
        return int_from_i32(a, 0);
    }

    Offset quot_offset, rem_offset;
    divmod_magnitude(a, lhs, rhs, &quot_offset, &rem_offset);

    IntValue *quot = int_resolve(a, quot_offset);
    quot->sign = lhs->sign ^ rhs->sign;

    IntValue *rem = int_resolve(a, rem_offset);
    if (!int_is_zero(rem) && lhs->sign != rhs->sign) {
        /* Adjust truncating quotient to flooring quotient: |q| += 1 */
        Offset one = int_from_i32(a, 1);
        return int_subtract(a, quot_offset, one);   /* e.g. -3 becomes -4 */
    }

    return quot_offset;
}

/* Get the remainder of a division of two IntValues `m` and `n`, yielding an
 * IntValue result. The remainder `r` has the same sign as the divisor `n`,
 * and `|r| < |n|`, uniquely such that `m = q * n + r`. The `fallback` is
 * returned instead of computing the remainder, in case the denominator is
 * zero.
 *
 * Preconditions: `m` is a valid IntValue. `n` is a valid IntValue.
 */
Offset int_modulo(Arena *a, Offset m, Offset n, Offset fallback) {
    IntValue *lhs = int_resolve(a, m);
    IntValue *rhs = int_resolve(a, n);

    if (int_is_zero(rhs)) {
        return fallback;
    }
    if (int_is_zero(lhs)) {
        return m;
    }

    if (compare_magnitude(lhs, rhs) < 0) {
        /* |lhs| < |rhs|.
           If signs agree, remainder is lhs.
           If signs differ, remainder is |rhs| - |lhs| with sign of rhs. */
        if (lhs->sign == rhs->sign) {
            return m;
        }
        else {
            Offset abs_rhs = int_abs(a, n);
            Offset abs_lhs = int_abs(a, m);
            Offset new_rem = sub_magnitude(
                a,
                int_resolve(a, abs_rhs),
                int_resolve(a, abs_lhs)
            );
            int_resolve(a, new_rem)->sign = rhs->sign;
            return new_rem;
        }
    }

    Offset quot_offset, rem_offset;
    divmod_magnitude(a, lhs, rhs, &quot_offset, &rem_offset);

    IntValue *rem = int_resolve(a, rem_offset);
    if (int_is_zero(rem)) {
        return rem_offset;
    }

    if (lhs->sign != rhs->sign) {
        /* Adjust truncating remainder to flooring remainder:
           |r| = |rhs| - |r| */
        Offset abs_rhs = int_abs(a, n);
        Offset new_rem = sub_magnitude(a, int_resolve(a, abs_rhs), rem);
        int_resolve(a, new_rem)->sign = rhs->sign;
        return new_rem;
    }

    rem->sign = lhs->sign;
    return rem_offset;
}

/* Return a string representation (in the form of an AsciiStrValue) of the
 * IntValue `offset`.
 *
 * Preconditions: `offset` is a valid IntValue.
 */
Offset int_to_str(Arena *a, Offset offset) {
    IntValue *v = int_resolve(a, offset);
    if (int_is_zero(v)) {
        s8 str = s8("0");
        return ascii_str_new(a, &str);
    }

    /* Copy magnitude so we can destroy it with repeated divmod10 */
    u32 len = v->length;
    u32 *tmp = malloc(len * sizeof(u32));
    if (tmp == NULL) {
        vm_crash(CRASH_MALLOC);
    }
    memcpy(tmp, v->payload, len * sizeof(u32));

    /* Each u32 is at most 10 decimal digits */
    u32 max_digits = len * 10 + 1;  /* + 1 for sign */
    char *rev = malloc(max_digits);
    if (rev == NULL) {
        vm_crash(CRASH_MALLOC);
    }
    u32 n = 0;

    while (magnitude_is_nonzero(tmp, len)) {
        u32 rem = divmod10(tmp, len);
        rev[n++] = '0' + (char)rem;
    }

    u32 result_len = n + (v->sign ? 1 : 0);
    AsciiStrValue *result = arena_alloc(
        a,
        sizeof(AsciiStrValue) + result_len,
        alignof(AsciiStrValue)
    );
    result->tag = TAG_ASCII_STR;
    result->length_in_bytes = result_len;

    u32 pos = 0;
    if (v->sign) {
        result->payload[pos++] = '-';
    }
    for (u32 i = n; i-- > 0; ) {
        result->payload[pos++] = rev[i];
    }

    free(rev);
    free(tmp);
    return (Offset)((unsigned char *)result - a->bytes);
}

