#include "../include/activation-record.h"
#include "../include/arena.h"
#include "../include/crash.h"
#include "../include/instruction.h"
#include "../include/outcome.h"
#include "../include/run-instruction.h"

static u16 little_endian_u16(u8 high_byte, u8 low_byte) {
    return ((u16)high_byte << 8) + (u16)low_byte;
}

static Outcome run_no_op(Arena *arena, ActivationRecord **activation_record) {
    (void)arena;
    (void)activation_record;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_mov(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_assert_int(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_load_int(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    u16 index
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)index;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_int_add(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1,
    Offset r2
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    (void)r2;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_int_neg(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_int_sub(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1,
    Offset r2
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    (void)r2;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_int_mul(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1,
    Offset r2
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    (void)r2;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_int_div(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1,
    Offset r2
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    (void)r2;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_int_mod(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1,
    Offset r2
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    (void)r2;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_assert_str(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_load_str(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    u16 index
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)index;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_stringify(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_str_concat(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1,
    Offset r2
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    (void)r2;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_assert_bool(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_load_true(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_load_false(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_boolify(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_bool_neg(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_assert_none(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_load_none(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_cmp_lt(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1,
    Offset r2
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    (void)r2;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_cmp_le(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1,
    Offset r2
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    (void)r2;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_cmp_gt(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1,
    Offset r2
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    (void)r2;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_cmp_ge(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1,
    Offset r2
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    (void)r2;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_cmp_eq(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1,
    Offset r2
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    (void)r2;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_cmp_ne(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1,
    Offset r2
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    (void)r2;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_assert_array(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_array_init(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    u16 capacity
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)capacity;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_array_push(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset r1,
    Offset r2
) {
    (void)arena;
    (void)activation_record;
    (void)r1;
    (void)r2;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_array_get(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1,
    Offset r2
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    (void)r2;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_array_set(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset r1,
    Offset r2,
    Offset r3
) {
    (void)arena;
    (void)activation_record;
    (void)r1;
    (void)r2;
    (void)r3;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_array_len(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_array_concat(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1,
    Offset r2
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    (void)r2;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_assert_func(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_func_init(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    u16 index
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)index;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_func_call(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1,
    Offset r2
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    (void)r2;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_func_tailcall(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset r1,
    Offset r2
) {
    (void)arena;
    (void)activation_record;
    (void)r1;
    (void)r2;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_assert_macro(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_macro_init(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    u16 index
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)index;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_env_load(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    u8 ee,
    u8 oo
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)ee;
    (void)oo;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_env_store(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset r1,
    u8 ee,
    u8 oo
) {
    (void)arena;
    (void)activation_record;
    (void)r1;
    (void)ee;
    (void)oo;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_syntax_node_create(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_int_node_create(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_str_node_create(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_bool_node_create(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_print(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_input(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset tr,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)tr;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_jump(
    Arena *arena,
    ActivationRecord **activation_record,
    u16 offset
) {
    (void)arena;
    (void)activation_record;
    (void)offset;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_jump_if_true(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset r1,
    u16 offset
) {
    (void)arena;
    (void)activation_record;
    (void)r1;
    (void)offset;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_return(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

static Outcome run_proc_exit(
    Arena *arena,
    ActivationRecord **activation_record,
    Offset r1
) {
    (void)arena;
    (void)activation_record;
    (void)r1;
    vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
}

Outcome run_instruction(
    Instruction *instruction,
    Arena *arena,
    ActivationRecord **activation_record
) {
    u8 operator = instruction->operator;

    switch (operator) {
        case OPCODE_NO_OP: {
            return run_no_op(arena, activation_record);
        }
        case OPCODE_MOV: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            return run_mov(arena, activation_record, tr, r1);
        }
        case OPCODE_ASSERT_INT: {
            u8 r1 = instruction->operand1;
            return run_assert_int(arena, activation_record, r1);
        }
        case OPCODE_LOAD_INT: {
            u8 tr = instruction->operand1;
            u16 index = little_endian_u16(
                instruction->operand2,
                instruction->operand3
            );
            return run_load_int(arena, activation_record, tr, index);
        }
        case OPCODE_INT_ADD: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            u8 r2 = instruction->operand3;
            return run_int_add(arena, activation_record, tr, r1, r2);
        }
        case OPCODE_INT_NEG: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            return run_int_neg(arena, activation_record, tr, r1);
        }
        case OPCODE_INT_SUB: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            u8 r2 = instruction->operand3;
            return run_int_sub(arena, activation_record, tr, r1, r2);
        }
        case OPCODE_INT_MUL: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            u8 r2 = instruction->operand3;
            return run_int_mul(arena, activation_record, tr, r1, r2);
        }
        case OPCODE_INT_DIV: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            u8 r2 = instruction->operand3;
            return run_int_div(arena, activation_record, tr, r1, r2);
        }
        case OPCODE_INT_MOD: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            u8 r2 = instruction->operand3;
            return run_int_mod(arena, activation_record, tr, r1, r2);
        }
        case OPCODE_ASSERT_STR: {
            u8 r1 = instruction->operand1;
            return run_assert_str(arena, activation_record, r1);
        }
        case OPCODE_LOAD_STR: {
            u8 tr = instruction->operand1;
            u16 index = little_endian_u16(
                instruction->operand2,
                instruction->operand3
            );
            return run_load_str(arena, activation_record, tr, index);
        }
        case OPCODE_STRINGIFY: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            return run_stringify(arena, activation_record, tr, r1);
        }
        case OPCODE_STR_CONCAT: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            u8 r2 = instruction->operand3;
            return run_str_concat(arena, activation_record, tr, r1, r2);
        }
        case OPCODE_ASSERT_BOOL: {
            u8 r1 = instruction->operand1;
            return run_assert_bool(arena, activation_record, r1);
        }
        case OPCODE_LOAD_TRUE: {
            u8 tr = instruction->operand1;
            return run_load_true(arena, activation_record, tr);
        }
        case OPCODE_LOAD_FALSE: {
            u8 tr = instruction->operand1;
            return run_load_false(arena, activation_record, tr);
        }
        case OPCODE_BOOLIFY: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            return run_boolify(arena, activation_record, tr, r1);
        }
        case OPCODE_BOOL_NEG: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            return run_bool_neg(arena, activation_record, tr, r1);
        }
        case OPCODE_ASSERT_NONE: {
            u8 r1 = instruction->operand1;
            return run_assert_none(arena, activation_record, r1);
        }
        case OPCODE_LOAD_NONE: {
            u8 tr = instruction->operand1;
            return run_load_none(arena, activation_record, tr);
        }
        case OPCODE_CMP_LT: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            u8 r2 = instruction->operand3;
            return run_cmp_lt(arena, activation_record, tr, r1, r2);
        }
        case OPCODE_CMP_LE: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            u8 r2 = instruction->operand3;
            return run_cmp_le(arena, activation_record, tr, r1, r2);
        }
        case OPCODE_CMP_GT: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            u8 r2 = instruction->operand3;
            return run_cmp_gt(arena, activation_record, tr, r1, r2);
        }
        case OPCODE_CMP_GE: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            u8 r2 = instruction->operand3;
            return run_cmp_ge(arena, activation_record, tr, r1, r2);
        }
        case OPCODE_CMP_EQ: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            u8 r2 = instruction->operand3;
            return run_cmp_eq(arena, activation_record, tr, r1, r2);
        }
        case OPCODE_CMP_NE: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            u8 r2 = instruction->operand3;
            return run_cmp_ne(arena, activation_record, tr, r1, r2);
        }
        case OPCODE_ASSERT_ARRAY: {
            u8 r1 = instruction->operand1;
            return run_assert_array(arena, activation_record, r1);
        }
        case OPCODE_ARRAY_INIT: {
            u8 tr = instruction->operand1;
            u16 capacity = little_endian_u16(
                instruction->operand2,
                instruction->operand3
            );
            return run_array_init(arena, activation_record, tr, capacity);
        }
        case OPCODE_ARRAY_PUSH: {
            u8 r1 = instruction->operand1;
            u8 r2 = instruction->operand2;
            return run_array_push(arena, activation_record, r1, r2);
        }
        case OPCODE_ARRAY_GET: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            u8 r2 = instruction->operand3;
            return run_array_get(arena, activation_record, tr, r1, r2);
        }
        case OPCODE_ARRAY_SET: {
            u8 r1 = instruction->operand1;
            u8 r2 = instruction->operand2;
            u8 r3 = instruction->operand3;
            return run_array_set(arena, activation_record, r1, r2, r3);
        }
        case OPCODE_ARRAY_LEN: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            return run_array_len(arena, activation_record, tr, r1);
        }
        case OPCODE_ARRAY_CONCAT: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            u8 r2 = instruction->operand3;
            return run_array_concat(arena, activation_record, tr, r1, r2);
        }
        case OPCODE_ASSERT_FUNC: {
            u8 r1 = instruction->operand1;
            return run_assert_func(arena, activation_record, r1);
        }
        case OPCODE_FUNC_INIT: {
            u8 tr = instruction->operand1;
            u16 index = little_endian_u16(
                instruction->operand2,
                instruction->operand3
            );
            return run_func_init(arena, activation_record, tr, index);
        }
        case OPCODE_FUNC_CALL: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            u8 r2 = instruction->operand3;
            return run_func_call(arena, activation_record, tr, r1, r2);
        }
        case OPCODE_FUNC_TAILCALL: {
            u8 r1 = instruction->operand2;
            u8 r2 = instruction->operand3;
            return run_func_tailcall(arena, activation_record, r1, r2);
        }
        case OPCODE_ASSERT_MACRO: {
            u8 r1 = instruction->operand1;
            return run_assert_macro(arena, activation_record, r1);
        }
        case OPCODE_MACRO_INIT: {
            u8 tr = instruction->operand1;
            u16 index = little_endian_u16(
                instruction->operand2,
                instruction->operand3
            );
            return run_macro_init(arena, activation_record, tr, index);
        }
        case OPCODE_ENV_LOAD: {
            u8 tr = instruction->operand1;
            u8 ee = instruction->operand2;
            u8 oo = instruction->operand3;
            return run_env_load(arena, activation_record, tr, ee, oo);
        }
        case OPCODE_ENV_STORE: {
            u8 r1 = instruction->operand1;
            u8 ee = instruction->operand2;
            u8 oo = instruction->operand3;
            return run_env_store(arena, activation_record, r1, ee, oo);
        }
        case OPCODE_SYNTAX_NODE_CREATE: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            return run_syntax_node_create(arena, activation_record, tr, r1);
        }
        case OPCODE_INT_NODE_CREATE: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            return run_int_node_create(arena, activation_record, tr, r1);
        }
        case OPCODE_STR_NODE_CREATE: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            return run_str_node_create(arena, activation_record, tr, r1);
        }
        case OPCODE_BOOL_NODE_CREATE: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            return run_bool_node_create(arena, activation_record, tr, r1);
        }
        case OPCODE_PRINT: {
            u8 r1 = instruction->operand1;
            return run_print(arena, activation_record, r1);
        }
        case OPCODE_INPUT: {
            u8 tr = instruction->operand1;
            u8 r1 = instruction->operand2;
            return run_input(arena, activation_record, tr, r1);
        }
        case OPCODE_JUMP: {
            u16 offset = little_endian_u16(
                instruction->operand2,
                instruction->operand3
            );
            return run_jump(arena, activation_record, offset);
        }
        case OPCODE_JUMP_IF_TRUE: {
            u8 r1 = instruction->operand1;
            u16 offset = little_endian_u16(
                instruction->operand2,
                instruction->operand3
            );
            return run_jump_if_true(arena, activation_record, r1, offset);
        }
        case OPCODE_RETURN: {
            u8 r1 = instruction->operand1;
            return run_return(arena, activation_record, r1);
        }
        case OPCODE_PROC_EXIT: {
            u8 r1 = instruction->operand1;
            return run_proc_exit(arena, activation_record, r1);
        }
        default: {
            vm_crash(CRASH_INSTRUCTION_NOT_IMPLEMENTED);
        }
    }
}

