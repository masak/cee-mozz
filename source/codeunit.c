#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/code-table.h"
#include "../include/codeunit.h"
#include "../include/crash.h"
#include "../include/environment.h"
#include "../include/instruction.h"
#include "../include/int-table.h"
#include "../include/parameters.h"
#include "../include/seenset.h"
#include "../include/str-table.h"
#include "../include/tags.h"
#include "../include/value.h"

Offset codeunit_new(
    Arena *a,
    Offset parameters,
    u32 register_count,
    u32 env_length,
    MaybeOffset inttable_offset,
    MaybeOffset strtable_offset,
    MaybeOffset codetable_offset,
    u32 instr_count,
    Instruction instructions[]
) {
    size_t instructions_size = instr_count * sizeof(Instruction);
    CodeUnit *codeunit = arena_alloc(
        a,
        sizeof(CodeUnit) + instructions_size,
        alignof(CodeUnit)
    );
    codeunit->tag = TAG_CODE_UNIT;
    codeunit->parameters = parameters;
    codeunit->register_count = register_count;
    codeunit->env_length = env_length;
    codeunit->inttable_offset = inttable_offset;
    codeunit->strtable_offset = strtable_offset;
    codeunit->codetable_offset = codetable_offset;
    codeunit->instr_count = instr_count;
    if (instructions != NULL) {
        memcpy(codeunit->instructions, instructions, instructions_size);
    }

    return (Offset)((unsigned char *)codeunit - a->bytes);
}

/* Returns a pointer to a CodeUnit, given an offset into an arena.
 *
 * Precondition: `offset` points to a CodeUnit.
 */
CodeUnit *codeunit_resolve(Arena *a, Offset offset) {
    if (value_tag(a, offset) != TAG_CODE_UNIT) {
        vm_crash(CRASH_INVALID_TAG);
    }

    assert(offset <= ARENA_SIZE - sizeof(CodeUnit));
    return (CodeUnit *)(a->bytes + offset);
}

bool codeunit_validate(Arena *a, Offset offset, SeenSet *seenset) {
    if (seen(seenset, offset)) {
        return true;
    }
    seenset_add(seenset, offset);

    assert(offset <= ARENA_SIZE - sizeof(CodeUnit));
    CodeUnit *codeunit = codeunit_resolve(a, offset);

    Offset params_offset = codeunit->parameters;
    if (params_offset == UNSET) {
        return false;
    }
    if (value_tag(a, params_offset) != TAG_PARAMETERS) {
        return false;
    }
    if (!parameters_validate(a, params_offset, seenset)) {
        return false;
    }

    Parameters *parameters = parameters_resolve(a, params_offset);
    if (parameters->parameter_count > codeunit->register_count) {
        return false;
    }

    if (codeunit->inttable_offset != UNSET) {
        if (value_tag(a, codeunit->inttable_offset) != TAG_INT_TABLE) {
            return false;
        }
        if (!inttable_validate(a, codeunit->inttable_offset, seenset)) {
            return false;
        }
    }

    if (codeunit->strtable_offset != UNSET) {
        if (value_tag(a, codeunit->strtable_offset) != TAG_STR_TABLE) {
            return false;
        }
        if (!strtable_validate(a, codeunit->strtable_offset, seenset)) {
            return false;
        }
    }

    if (codeunit->codetable_offset != UNSET) {
        if (value_tag(a, codeunit->codetable_offset) != TAG_CODE_TABLE) {
            return false;
        }
        if (!codetable_validate(a, codeunit->codetable_offset, seenset)) {
            return false;
        }
    }

    /* for each instruction:
           check each register is within range
           check that each register that is read from was initialized
           check that int, str, code indices are within range
           check that env load/store indices are within range
           check that jump targets are within range */

    /* check that the instructions don't fall off the end without a jump
       or a return */

    /* later we want to recursively validate the IntTable, StrTable, and
       CodeTable, but we have to implement those types first */

    return true;
}

