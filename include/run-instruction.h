#ifndef RUN_INSTRUCTION_H
#define RUN_INSTRUCTION_H

#include "activation-record.h"
#include "arena.h"
#include "instruction.h"
#include "outcome.h"

Outcome run_instruction(
    Instruction *instruction,
    Arena *arena,
    ActivationRecord **activation_record
);

#endif

