#include <stdbool.h>
#include <stdint.h>

#include "../include/arena.h"
#include "../include/i64-value.h"
#include "../include/seenset.h"
#include "../include/tags.h"

Offset i64_new(Arena *a, i64 payload) {
    I64Value *i64_value = arena_alloc(a, sizeof(I64Value), alignof(I64Value));
    i64_value->tag = TAG_I64;
    i64_value->payload = payload;
    return (Offset)((unsigned char *)i64_value - a->bytes);
}

/* Resolve an offset back to a pointer. */
I64Value *i64_resolve(Arena *a, Offset offset) {
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

Offset i64_add(Arena *a, Offset m, Offset n) {
    I64Value *lhs = i64_resolve(a, m);
    I64Value *rhs = i64_resolve(a, n);
    u64 sum = (u64)lhs->payload + (u64)rhs->payload;
    return i64_new(a, (i64)sum);
}

Offset i64_subtract(Arena *a, Offset m, Offset n) {
    I64Value *lhs = i64_resolve(a, m);
    I64Value *rhs = i64_resolve(a, n);
    u64 difference = (u64)lhs->payload - (u64)rhs->payload;
    return i64_new(a, (i64)difference);
}

Offset i64_multiply(Arena *a, Offset m, Offset n) {
    I64Value *lhs = i64_resolve(a, m);
    I64Value *rhs = i64_resolve(a, n);
    u64 product = (u64)lhs->payload * (u64)rhs->payload;
    return i64_new(a, (i64)product);
}

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

