#include <assert.h>
#include <stdalign.h>
#include <stddef.h>
#include <string.h>

#include "../include/arena.h"

void arena_init(Arena *a) {
    a->used = 0;
    memset(a->bytes, (unsigned char)0xA3, ARENA_SIZE);
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

