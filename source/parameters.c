#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/parameters.h"
#include "../include/seenset.h"
#include "../include/typedefs.h"
#include "../include/tags.h"
#include "../include/value.h"

Offset parameters_new(
    Arena *a,
    u32 parameter_count,
    Offset parameter_names[]
) {
    size_t entries_size = parameter_count * sizeof(Offset);
    Parameters *parameters = arena_alloc(
        a,
        sizeof(Parameters) + entries_size,
        alignof(Parameters)
    );
    parameters->tag = TAG_PARAMETERS;
    parameters->parameter_count = parameter_count;
    if (parameter_names != NULL) {
        memcpy(
            parameters->parameter_names,
            parameter_names,
            parameter_count * sizeof(Offset)
        );
    }

    return (Offset)((unsigned char *)parameters - a->bytes);
}

Parameters *parameters_resolve(Arena *a, Offset offset) {
    assert(offset <= ARENA_SIZE - sizeof(Parameters));
    return (Parameters *)(a->bytes + offset);
}

bool parameters_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    Parameters *parameters = parameters_resolve(a, offset);

    if (parameters->parameter_count > 0x100) {
        return false;
    }

    for (u32 i = 0; i < parameters->parameter_count; i++) {
        Offset name = parameters->parameter_names[i];
        if (name == UNSET) {
            return false;
        }
        Tag tag = value_tag(a, name);
        if (tag != TAG_ASCII_STR) {     /* XXX: extend later */
            return false;
        }
        if (!generic_validate(a, name, seenset)) {
            return false;
        }
    }

    return true;
}

