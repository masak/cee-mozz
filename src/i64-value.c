#include <stdbool.h>
#include <stdint.h>

#include "../include/i64-value.h"

size_t i64_new(Arena *a, i64 payload) {
    I64Value *i64_value = arena_alloc(a, sizeof(I64Value), alignof(I64Value));
    i64_value->tag = 0;
    i64_value->payload = payload;
    return (size_t)((unsigned char *)i64_value - a->bytes);
}

/* Resolve an offset back to a pointer. */
I64Value *i64_resolve(Arena *a, size_t offset) {
    assert(offset <= ARENA_SIZE - sizeof(I64Value));
    return (I64Value *)(a->bytes + offset);
}

size_t i64_add(Arena *a, size_t m, size_t n) {
    I64Value *lhs = i64_resolve(a, m);
    I64Value *rhs = i64_resolve(a, n);
    return i64_new(a, lhs->payload + rhs->payload);
}

size_t i64_subtract(Arena *a, size_t m, size_t n) {
    I64Value *lhs = i64_resolve(a, m);
    I64Value *rhs = i64_resolve(a, n);
    return i64_new(a, lhs->payload - rhs->payload);
}

size_t i64_multiply(Arena *a, size_t m, size_t n) {
    I64Value *lhs = i64_resolve(a, m);
    I64Value *rhs = i64_resolve(a, n);
    return i64_new(a, lhs->payload * rhs->payload);
}

size_t i64_divide(Arena *a, size_t m, size_t n, size_t fallback) {
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

size_t i64_modulo(Arena *a, size_t m, size_t n, size_t fallback) {
    I64Value *numerator = i64_resolve(a, m);
    I64Value *denominator = i64_resolve(a, n);

    i64 num = numerator->payload;
    i64 den = denominator->payload;

    if (den == 0) {
        return fallback;
    }

    if (num == INT64_MIN && den == -1) {
        return i64_new(a, 0);
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

