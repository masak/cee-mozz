#include <stdbool.h>
#include <stdint.h>

#include "../include/arena.h"
#include "../include/codeunit.h"
#include "../include/crash.h"
#include "../include/environment.h"
#include "../include/func-value.h"
#include "../include/seenset.h"
#include "../include/tags.h"
#include "../include/value.h"

Offset func_new(Arena *a, Offset env_offset, Offset codeunit_offset) {
    FuncValue *func_value = arena_alloc(
        a,
        sizeof(FuncValue),
        alignof(FuncValue)
    );
    func_value->tag = TAG_FUNC;
    func_value->env_offset = env_offset;
    func_value->codeunit_offset = codeunit_offset;
    return (Offset)((unsigned char *)func_value - a->bytes);
}

/* Return a pointer to a FuncValue, given an offset into an arena.
 *
 * Precondition: `offset` points to a valid FuncValue.
 */
FuncValue *func_resolve(Arena *a, Offset offset) {
    if (value_tag(a, offset) != TAG_FUNC) {
        vm_crash(CRASH_INVALID_TAG);
    }

    assert(offset <= ARENA_SIZE - sizeof(FuncValue));
    return (FuncValue *)(a->bytes + offset);
}

bool func_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    FuncValue *func_value = func_resolve(a, offset);

    Offset env_offset = func_value->env_offset;
    if (env_offset == UNSET) {
        return false;
    }
    if (value_tag(a, env_offset) != TAG_ENVIRONMENT) {
        return false;
    }
    if (!environment_validate(a, env_offset, seenset)) {
        return false;
    }

    Offset codeunit_offset = func_value->codeunit_offset;
    if (codeunit_offset == UNSET) {
        return false;
    }
    if (value_tag(a, codeunit_offset) != TAG_CODE_UNIT) {
        return false;
    }
    if (!codeunit_validate(a, codeunit_offset, seenset)) {
        return false;
    }

    return true;
}

