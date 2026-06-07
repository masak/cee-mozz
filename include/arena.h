#ifndef ARENA_H
#define ARENA_H

#include <assert.h>
#include <stdalign.h>
#include <stddef.h>

#include "typedefs.h"

enum { ARENA_SIZE = 16 * 1024 };

#define UNSET 0xA3A3A3A3A3A3A3A3ull

typedef struct {
    _Alignas(max_align_t) unsigned char bytes[ARENA_SIZE];
    size_t used;
} Arena;

typedef u32     Tag;

typedef size_t  Offset;
typedef size_t  MaybeOffset;

void arena_init(Arena *a);
void *arena_alloc(Arena *a, size_t size, size_t align);

#endif

