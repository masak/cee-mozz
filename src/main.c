#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint64_t    u64;
typedef int64_t     i64;

enum { ARENA_SIZE = 16 * 1024 };

typedef struct {
    _Alignas(max_align_t) unsigned char bytes[ARENA_SIZE];
    size_t used;
} Arena;

void arena_init(Arena *a) {
    a->used = 0;
}

/* Bump-allocate `size` bytes with a given power-of-two alignment. */
void *arena_alloc(Arena *a, size_t size, size_t align) {
    assert(align != 0);
    assert((align & (align - 1)) == 0);

    size_t mask = align - 1;
    size_t pos = (a->used + mask) & ~mask;

    assert(pos <= ARENA_SIZE);
    assert(size <= ARENA_SIZE - pos);

    void *ptr = &a->bytes[pos];
    a->used = pos + size;
    return ptr;
}

typedef struct {
    u64 tag;
    i64 payload;
} I64Value;

size_t i64_new(Arena *a, i64 payload) {
    I64Value *i64_value = arena_alloc(a, sizeof(I64Value), alignof(I64Value));
    i64_value->tag = 0;
    i64_value->payload = payload;
    return (size_t)((unsigned char *)i64_value - a->bytes);
}

/* Resolve an offset back to a pointer. */
static I64Value *i64_resolve(Arena *a, size_t offset) {
    assert(offset <= ARENA_SIZE - sizeof(I64Value));
    return (I64Value *)(a->bytes + offset);
}

size_t i64_add(Arena *a, size_t m, size_t n) {
    I64Value *lhs = i64_resolve(a, m);
    I64Value *rhs = i64_resolve(a, n);
    return i64_new(a, lhs->payload + rhs->payload);
}

int main(void) {
    Arena a;
    arena_init(&a);

    size_t x = i64_new(&a, 40);
    size_t y = i64_new(&a, 2);
    size_t z = i64_add(&a, x, y);

    printf(
        "x offset = %zu, value = %" PRId64 "\n",
        x,
        i64_resolve(&a, x)->payload
    );
    printf(
        "y offset = %zu, value = %" PRId64 "\n",
        y,
        i64_resolve(&a, y)->payload
    );
    printf(
        "z offset = %zu, value = %" PRId64 "\n",
        z,
        i64_resolve(&a, z)->payload
    );

    return 0;
}

