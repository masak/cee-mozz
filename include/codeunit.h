#ifndef CODEUNIT_H
#define CODEUNIT_H

#include <stdbool.h>

#include "arena.h"
#include "typedefs.h"

typedef struct {
    u8 operator;
    u8 operand1;
    u8 operand2;
    u8 operand3;
} Instruction;

typedef struct {
    u64 tag;
    u64 parameter_count;
    u64 register_count;
    u64 env_length;
    size_t inttable_offset;
    size_t strtable_offset;
    size_t codetable_offset;
    u64 instr_count;
    Instruction instructions[];
} CodeUnit;

size_t codeunit_new(
    Arena *a,
    u64 parameter_count,
    u64 register_count,
    u64 env_length,
    size_t inttable_offset,
    size_t strtable_offset,
    size_t codetable_offset,
    u64 instr_count,
    Instruction instructions[]
);
CodeUnit *codeunit_resolve(Arena *a, size_t offset);
bool codeunit_validate(Arena *a, size_t offset);

#endif

