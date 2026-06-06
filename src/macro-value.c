#include <stdbool.h>
#include <stdint.h>

#include "../include/arena.h"
#include "../include/codeunit.h"
#include "../include/environment.h"
#include "../include/macro-value.h"
#include "../include/seenset.h"
#include "../include/tags.h"

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
    if (env_offset > ARENA_SIZE - sizeof(Environment)) {
        return false;
    }
    Environment *env = environment_resolve(a, env_offset);
    if (env->tag != TAG_ENVIRONMENT) {
        return false;
    }

    Offset codeunit_offset = macro_value->codeunit_offset;
    if (codeunit_offset > ARENA_SIZE - sizeof(CodeUnit)) {
        return false;
    }
    CodeUnit *codeunit = codeunit_resolve(a, codeunit_offset);
    if (codeunit->tag != TAG_CODE_UNIT) {
        return false;
    }

    /* later we want to recursively validate the environment entries and
       code unit tables, but it has to wait for the SeenSet mechanism */

    return true;
}

