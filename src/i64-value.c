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

