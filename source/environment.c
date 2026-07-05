#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/crash.h"
#include "../include/environment.h"
#include "../include/seenset.h"
#include "../include/typedefs.h"
#include "../include/tags.h"
#include "../include/value.h"

Offset environment_new(
    Arena *a,
    MaybeOffset outer_env_offset,
    u32 entry_count,
    EnvEntry entries[]
) {
    size_t entries_size = entry_count * sizeof(EnvEntry);
    Environment *environment = arena_alloc(
        a,
        sizeof(Environment) + entries_size,
        alignof(Environment)
    );
    environment->tag = TAG_ENVIRONMENT;
    environment->outer_env_offset = outer_env_offset;
    environment->entry_count = entry_count;
    if (entries != NULL) {
        memcpy(environment->entries, entries, entries_size);
    }

    return (Offset)((unsigned char *)environment - a->bytes);
}

/* Returns a pointer to an Environment, given an offset into an arena.
 *
 * Precondition: `offset` points to a Environment.
 */
Environment *environment_resolve(Arena *a, Offset offset) {
    if (value_tag(a, offset) != TAG_ENVIRONMENT) {
        vm_crash(CRASH_INVALID_TAG);
    }

    assert(offset <= ARENA_SIZE - sizeof(Environment));
    return (Environment *)(a->bytes + offset);
}

bool environment_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    Environment *environment = environment_resolve(a, offset);

    MaybeOffset outer_env_offset = environment->outer_env_offset;
    if (outer_env_offset != UNSET) {
        if (value_tag(a, outer_env_offset) != TAG_ENVIRONMENT) {
            return false;
        }
        if (!environment_validate(a, outer_env_offset, seenset)) {
            return false;
        }
    }

    for (u32 i = 0; i < environment->entry_count; i++) {
        MaybeOffset cell = environment->entries[i].cell;
        if (cell == UNSET) {
            continue;
        }
        if (!generic_validate(a, cell, seenset)) {
            return false;
        }
    }

    return true;
}

/* Return an entry from the Environment at `env_offset`, at the index
 * `entry_index`.
 *
 * Precondition: `env_offset` points to a valid Environment.
 * Additional expectation: `entry_index` is within bounds; that is, it falls
 *                         within the range 0 ..^ N, N being the number of
 *                         entries in the Environment.
 */
static Offset environment_direct_load(
    Arena *a,
    Offset env_offset,
    u32 entry_index,
    Offset fallback_offset
) {
    Environment *environment = environment_resolve(a, env_offset);
    if (entry_index >= environment->entry_count) {
        /* XXX: Turn this into a vm_crash() (and remove `fallback_outset`) */
        return fallback_offset;
    }

    MaybeOffset cell = environment->entries[entry_index].cell;
    if (cell == UNSET) {
        return fallback_offset;
    }

    return cell;
}

/* Return an entry from `outer_steps` outer environments up from the
 * Environment at `env_offset`, at the index `entry_index`.
 *
 * Precondition: `env_offset` points to a valid Environment.
 * Additional expectation: There actually `outer_steps` of environments to
 *                         follow. `entry_index` is within bounds; that is, it
 *                         falls within the range 0 ..^ N, N being the number
 *                         of entries in the indicated Environment.
 */
Offset environment_load(
    Arena *a,
    Offset env_offset,
    u32 outer_steps,
    u32 entry_index,
    Offset fallback_offset
) {
    Offset current_env_offset = env_offset;

    for (u32 i = 0; i < outer_steps; i++) {
        Environment *environment = environment_resolve(a, current_env_offset);
        MaybeOffset outer_env_offset = environment->outer_env_offset;
        if (outer_env_offset == UNSET) {
            return fallback_offset;
        }
        current_env_offset = outer_env_offset;
    }

    return environment_direct_load(
        a,
        current_env_offset,
        entry_index,
        fallback_offset
    );
}

/* Store `value_offset` into an entry from the Environment at `env_offset`,
 * at the index `entry_index`.
 *
 * Precondition: `env_offset` points to a valid Environment.
 * Additional expectations: `entry_index` is within bounds; that is, it falls
 *                          within the range 0 ..^ N, N being the number of
 *                          entries in the Environment. The entry being stored
 *                          into needs to be writable.
 */
static void environment_direct_store(
    Arena *a,
    Offset env_offset,
    u32 entry_index,
    Offset value_offset
) {
    if (value_offset == UNSET) {
        vm_crash(CRASH_STORE_UNSET);
    }

    Environment *environment = environment_resolve(a, env_offset);
    if (entry_index >= environment->entry_count) {
        vm_crash(CRASH_OUT_OF_BOUNDS);
    }

    if (!environment->entries[entry_index].writable) {
        vm_crash(CRASH_STORE_READONLY);
    }

    environment->entries[entry_index].cell = value_offset;
}

/* Store `value_offset` into an entry from the `outer_steps` outer environments
 * up from the Environment at `env_offset`, at the index `entry_index`.
 *
 * Precondition: `env_offset` points to a valid Environment.
 * Additional expectations: There are actually `outer_steps` of environments
 *                          to follow. `entry_index` is within bounds; that is,
 *                          it falls within the range 0 ..^ N, N being the
 *                          number of entries in the indicated Environment. The
 *                          entry being stored into needs to be writable.
 */
void environment_store(
    Arena *a,
    Offset env_offset,
    u32 outer_steps,
    u32 entry_index,
    Offset value_offset
) {
    Offset current_env_offset = env_offset;

    for (u32 i = 0; i < outer_steps; i++) {
        Environment *environment = environment_resolve(a, current_env_offset);
        MaybeOffset outer_env_offset = environment->outer_env_offset;
        if (outer_env_offset == UNSET) {
            vm_crash(CRASH_OUT_OF_BOUNDS);
        }
        current_env_offset = outer_env_offset;
    }

    environment_direct_store(a, current_env_offset, entry_index, value_offset);
}

