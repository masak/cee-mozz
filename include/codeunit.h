#ifndef CODEUNIT_H
#define CODEUNIT_H

#include <stdbool.h>

#include "arena.h"
#include "seenset.h"
#include "typedefs.h"

typedef struct {
    u8 operator;
    u8 operand1;
    u8 operand2;
    u8 operand3;
} Instruction;

typedef struct {
    Tag tag;
    u32 parameter_count;
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
    u32 parameter_count,
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

