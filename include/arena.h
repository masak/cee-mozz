#ifndef ARENA_H
#define ARENA_H

#include <assert.h>
#include <stdalign.h>
#include <stddef.h>

enum { ARENA_SIZE = 16 * 1024 };

typedef struct {
    _Alignas(max_align_t) unsigned char bytes[ARENA_SIZE];
    size_t used;
} Arena;

typedef size_t Offset;

void arena_init(Arena *a);
void *arena_alloc(Arena *a, size_t size, size_t align);

#endif

