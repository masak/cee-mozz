#include <stdbool.h>
#include <stdint.h>

#include "../include/arena.h"
#include "../include/codeunit.h"
#include "../include/environment.h"
#include "../include/macro-value.h"
#include "../include/seenset.h"
#include "../include/tags.h"
#include "../include/value.h"

Offset macro_new(Arena *a, Offset env_offset, Offset codeunit_offset) {
    MacroValue *macro_value = arena_alloc(
        a,
        sizeof(MacroValue),
        alignof(MacroValue)
    );
    macro_value->tag = TAG_MACRO;
    macro_value->env_offset = env_offset;
    macro_value->codeunit_offset = codeunit_offset;
    return (Offset)((unsigned char *)macro_value - a->bytes);
}

MacroValue *macro_resolve(Arena *a, Offset offset) {
    assert(offset <= ARENA_SIZE - sizeof(MacroValue));
    return (MacroValue *)(a->bytes + offset);
}

bool macro_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    assert(offset <= ARENA_SIZE - sizeof(MacroValue));
    MacroValue *macro_value = macro_resolve(a, offset);

    Offset env_offset = macro_value->env_offset;
    if (value_tag(a, env_offset) != TAG_ENVIRONMENT) {
        return false;
    }
    if (!environment_validate(a, env_offset, seenset)) {
        return false;
    }

    Offset codeunit_offset = macro_value->codeunit_offset;
    if (value_tag(a, codeunit_offset) != TAG_CODE_UNIT) {
        return false;
    }
    if (!codeunit_validate(a, codeunit_offset, seenset)) {
        return false;
    }

    return true;
}

