#include <stdbool.h>
#include <stdint.h>

#include "../include/arena.h"
#include "../include/crash.h"
#include "../include/i64-value.h"
#include "../include/seenset.h"
#include "../include/tags.h"
#include "../include/value.h"

Offset i64_new(Arena *a, i64 payload) {
    I64Value *i64_value = arena_alloc(a, sizeof(I64Value), alignof(I64Value));
    i64_value->tag = TAG_I64;
    i64_value->payload = payload;
    return (Offset)((unsigned char *)i64_value - a->bytes);
}

/* Returns a pointer to an I64Value, given an offset into an arena.
 *
 * Precondition: `offset` points to a I64Value.
 */
I64Value *i64_resolve(Arena *a, Offset offset) {
    if (value_tag(a, offset) != TAG_I64) {
        vm_crash(CRASH_INVALID_TAG);
    }

    assert(offset <= ARENA_SIZE - sizeof(I64Value));
    return (I64Value *)(a->bytes + offset);
}

bool i64_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    (void)a;    /* all I64Value objects are valid; we don't use the arena */
    assert(offset <= ARENA_SIZE - sizeof(I64Value));
    return true;
}

/* Add two I64Values `m` and `n`, yielding an I64Value result. Overflow is
 * silent and wraps.
 *
 * Preconditions: `m` is a valid I64Value. `n` is a valid I64Value.
 */
Offset i64_add(Arena *a, Offset m, Offset n) {
    I64Value *lhs = i64_resolve(a, m);
    I64Value *rhs = i64_resolve(a, n);
    u64 sum = (u64)lhs->payload + (u64)rhs->payload;
    return i64_new(a, (i64)sum);
}

/* Subtract two I64Values `m` and `n`, yielding an I64Value result. Overflow is
 * silent and wraps.
 *
 * Preconditions: `m` is a valid I64Value. `n` is a valid I64Value.
 */
Offset i64_subtract(Arena *a, Offset m, Offset n) {
    I64Value *lhs = i64_resolve(a, m);
    I64Value *rhs = i64_resolve(a, n);
    u64 difference = (u64)lhs->payload - (u64)rhs->payload;
    return i64_new(a, (i64)difference);
}

/* Multiply two I64Values `m` and `n`, yielding an I64Value result. Overflow is
 * silent and wraps.
 *
 * Preconditions: `m` is a valid I64Value. `n` is a valid I64Value.
 */
Offset i64_multiply(Arena *a, Offset m, Offset n) {
    I64Value *lhs = i64_resolve(a, m);
    I64Value *rhs = i64_resolve(a, n);
    u64 product = (u64)lhs->payload * (u64)rhs->payload;
    return i64_new(a, (i64)product);
}

/* Divide two I64Values `m` and `n`, yielding an I64Value result. The resulting
 * quotient is rounded towards negative infinity. The `fallback` is returned
 * instead of doing the division, in case the denominator is zero or in the
 * special case of (INT64_MIN, -1).
 *
 * Preconditions: `m` is a valid I64Value. `n` is a valid I64Value.
 */
Offset i64_divide(Arena *a, Offset m, Offset n, Offset fallback) {
    I64Value *numerator = i64_resolve(a, m);
    I64Value *denominator = i64_resolve(a, n);

    i64 num = numerator->payload;
    i64 den = denominator->payload;

    if (den == 0) {
        return fallback;
    }

    if (num == INT64_MIN && den == -1) {
        return fallback;
    }

    i64 quotient = num / den;
    i64 remainder = num % den;

    // C does truncating division (round towards 0), but we want flooring
    // division in Mozzarella (round towards negative infinity).
    // This whole thing (including calculating `remainder`) gets optimized to
    // a single `idiv` instruction under clang with -O1 or -O2.
    bool negative_result = (num < 0) != (den < 0);
    if (remainder != 0 && negative_result) {
        quotient -= 1;
    }

    return i64_new(a, quotient);
}

/* Get the remainder of a division of two I64Values `m` and `n`, yielding an
 * I64Value result. The remainder `r` has the same sign as the divisor `n`, and
 * `|r| < |n|`, uniquely such that `m = q * n + r`. The `fallback` is returned
 * instead of computing the remainder, in case the denominator is zero or in
 * the special case of (INT64_MIN, -1).
 *
 * Preconditions: `m` is a valid I64Value. `n` is a valid I64Value.
 */
Offset i64_modulo(Arena *a, Offset m, Offset n, Offset fallback) {
    I64Value *numerator = i64_resolve(a, m);
    I64Value *denominator = i64_resolve(a, n);

    i64 num = numerator->payload;
    i64 den = denominator->payload;

    if (den == 0) {
        return fallback;
    }

    if (num == INT64_MIN && den == -1) {
        return fallback;
    }

    // num == (num // den) * den + (num % den)
    // num % den == num - (num // den) * den

    i64 quotient = num / den;
    i64 remainder = num % den;

    // C does truncating division (round towards 0), but we want flooring
    // division in Mozzarella (round towards negative infinity).
    // This whole thing (including calculating `remainder`) gets optimized to
    // a single `idiv` instruction under clang with -O1 or -O2.
    bool negative_result = (num < 0) != (den < 0);
    if (remainder != 0 && negative_result) {
        quotient -= 1;
    }

    return i64_new(a, num - quotient * den);
}

