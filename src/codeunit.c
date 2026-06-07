#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/arena.h"
#include "../include/codeunit.h"
#include "../include/environment.h"
#include "../include/seenset.h"
#include "../include/tags.h"

Offset codeunit_new(
    Arena *a,
    u32 parameter_count,
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
    codeunit->parameter_count = parameter_count;
    codeunit->register_count = register_count;
    codeunit->env_length = env_length;
    codeunit->inttable_offset = inttable_offset;
    codeunit->strtable_offset = strtable_offset;
    codeunit->codetable_offset = codetable_offset;
    codeunit->instr_count = instr_count;
    memcpy(codeunit->instructions, instructions, instructions_size);

    return (Offset)((unsigned char *)codeunit - a->bytes);
}

CodeUnit *codeunit_resolve(Arena *a, Offset offset) {
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

    if (codeunit->parameter_count > 0x100) {
        return false;
    }

    if (codeunit->parameter_count > codeunit->register_count) {
        return false;
    }

    /* check inttable_offset (IntTable tag or unset) */
    /* check strtable_offset (StrTable tag or unset) */
    /* check codetable_offset (CodeTable tag or unset) */

    /* for each instruction:
           check each register is within range
           check that each register that is read from was initialized
           check that int, str, code indixes are within range
           check that jump targets are within range */

    /* later we want to recursively validate the IntTable, StrTable, and
       CodeTable, but we have to implement those types first */

    return true;
}

