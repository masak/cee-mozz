#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/environment.h"
#include "../include/typedefs.h"
#include "../include/types.h"

size_t environment_new(
    Arena *a,
    size_t outer_env_offset,
    u64 entry_count,
    EnvEntry entries[]
) {
    Environment *environment
        = arena_alloc(a, sizeof(Environment), alignof(Environment));
    environment->tag = TAG_ENVIRONMENT;
    environment->outer_env_offset = outer_env_offset;
    environment->entry_count = entry_count;
    memcpy(environment->entries, entries, entry_count * sizeof(EnvEntry));

    return (size_t)((unsigned char *)environment - a->bytes);
}

Environment *environment_resolve(Arena *a, size_t offset) {
    assert(offset <= ARENA_SIZE - sizeof(Environment));
    return (Environment *)(a->bytes + offset);
}

bool environment_validate(Arena *a, size_t offset) {
    Environment *environment = environment_resolve(a, offset);
    (void) environment;     /* remove this line once we start doing the below
                               things; just hiding the warning for now */

    /* check that outer_env_offset points either to a valid environment,
       or is unset */

    /* for each entry_count:
           check that its cell points to a valid value tag and that its
           resolved value validates, or is unset */

    /* both of the above checks have to wait for the SeenSet mechanism */

    return true;
}

