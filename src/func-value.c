#include <stdbool.h>
#include <stdint.h>

#include "../include/arena.h"
#include "../include/codeunit.h"
#include "../include/environment.h"
#include "../include/func-value.h"
#include "../include/tags.h"

size_t func_new(Arena *a, size_t env_offset, size_t codeunit_offset) {
    FuncValue *func_value = arena_alloc(
        a,
        sizeof(FuncValue),
        alignof(FuncValue)
    );
    func_value->tag = TAG_FUNC;
    func_value->env_offset = env_offset;
    func_value->codeunit_offset = codeunit_offset;
    return (size_t)((unsigned char *)func_value - a->bytes);
}

FuncValue *func_resolve(Arena *a, size_t offset) {
    assert(offset <= ARENA_SIZE - sizeof(FuncValue));
    return (FuncValue *)(a->bytes + offset);
}

bool func_validate(Arena *a, size_t offset) {
    assert(offset <= ARENA_SIZE - sizeof(FuncValue));
    FuncValue *func_value = func_resolve(a, offset);

    size_t env_offset = func_value->env_offset;
    if (env_offset > ARENA_SIZE - sizeof(Environment)) {
        return false;
    }
    Environment *env = environment_resolve(a, env_offset);
    if (env->tag != TAG_ENVIRONMENT) {
        return false;
    }

    size_t codeunit_offset = func_value->codeunit_offset;
    if (codeunit_offset > ARENA_SIZE - sizeof(CodeUnit)) {
        return false;
    }
    CodeUnit *codeunit = codeunit_resolve(a, codeunit_offset);
    if (codeunit->tag != TAG_CODE_UNIT) {
        return false;
    }

    /* later we want to recursively validate the environment entries and
       code unit tables, but it has to wait for theh SeenSet mechanism */

    return true;
}

