#ifndef ARENA_H
#define ARENA_H

#include <assert.h>
#include <stdalign.h>
#include <stddef.h>

#include "typedefs.h"

/* 16 kB is really small. It's temporary; haven't run out of arena memory yet.
 * Will embiggen it in time, but right now being really smol works. Also, will
 * probably implement the GC before increasing the arena size.
 */
enum { ARENA_SIZE = 16 * 1024 };

#define UNSET 0xA3A3A3A3ul

typedef struct {
    _Alignas(max_align_t) unsigned char bytes[ARENA_SIZE];
    size_t used;
} Arena;

typedef u32     Tag;

typedef u32     Offset;
typedef u32     MaybeOffset;

void arena_init(Arena *a);
void *arena_alloc(Arena *a, size_t size, size_t align);

#endif

