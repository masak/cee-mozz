#ifndef CODEUNIT_H
#define CODEUNIT_H

#include <stdbool.h>

#include "arena.h"
#include "instruction.h"
#include "parameters.h"
#include "seenset.h"
#include "typedefs.h"

typedef struct {
    Tag tag;
    Offset parameters;
    u32 register_count;
    u32 env_length;
    MaybeOffset inttable_offset;
    MaybeOffset strtable_offset;
    MaybeOffset codetable_offset;
    u32 instr_count;
    Instruction instructions[];
} CodeUnit;

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
);
CodeUnit *codeunit_resolve(Arena *a, Offset offset);
bool codeunit_validate(Arena *a, Offset offset, SeenSet *seenset);

#endif

