#include <assert.h>
#include <stdalign.h>
#include <stddef.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/crash.h"

void arena_init(Arena *a) {
    a->used = 0;
    memset(a->bytes, (unsigned char)0xA3, ARENA_SIZE);
}

/* Bump-allocate `size` bytes with a given power-of-two alignment.
 *
 * Precondition: `align` should be a power of 2.
 * Additional requirement: There should be sufficient space at the end of the
 *                         arena for the new value of size `size`.
 * Postcondition: The new value has been allocated and sits within the arena,
 *                with the returned pointer pointing to its first byte.
 */
void *arena_alloc(Arena *a, size_t size, size_t align) {
    if (align == 0 || (align & (align - 1)) != 0) {
        vm_crash(CRASH_ALIGN_NOT_POW2);
    }

    size_t mask = align - 1;
    size_t pos = (a->used + mask) & ~mask;

    if (pos + size > ARENA_SIZE) {
        /* Later on, this is where we'll perform GC to try and compact the
           arena. For now, we just crash heedlessly. */
        vm_crash(CRASH_FULL_ARENA);
    }

    void *ptr = &a->bytes[pos];
    a->used = pos + size;
    return ptr;
}

