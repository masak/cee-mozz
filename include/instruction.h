#ifndef INSTRUCTION_H
#define INSTRUCTION_H

enum {
    OPCODE_NO_OP              = 0x00,
    OPCODE_MOV                = 0x01,
    OPCODE_ASSERT_INT         = 0x02,
    OPCODE_LOAD_INT           = 0x03,
    OPCODE_INT_ADD            = 0x04,
    OPCODE_INT_NEG            = 0x05,
    OPCODE_INT_SUB            = 0x06,
    OPCODE_INT_MUL            = 0x07,
    OPCODE_INT_DIV            = 0x08,
    OPCODE_INT_MOD            = 0x09,
    OPCODE_ASSERT_STR         = 0x0A,
    OPCODE_LOAD_STR           = 0x0B,
    OPCODE_STRINGIFY          = 0x0C,
    OPCODE_STR_CONCAT         = 0x0D,
    OPCODE_ASSERT_BOOL        = 0x0E,
    OPCODE_LOAD_TRUE          = 0x0F,
    OPCODE_LOAD_FALSE         = 0x10,
    OPCODE_BOOLIFY            = 0x11,
    OPCODE_BOOL_NEG           = 0x12,
    OPCODE_ASSERT_NONE        = 0x13,
    OPCODE_LOAD_NONE          = 0x14,
    OPCODE_CMP_LT             = 0x15,
    OPCODE_CMP_LE             = 0x16,
    OPCODE_CMP_GT             = 0x17,
    OPCODE_CMP_GE             = 0x18,
    OPCODE_CMP_EQ             = 0x19,
    OPCODE_CMP_NE             = 0x1A,
    OPCODE_ASSERT_ARRAY       = 0x1B,
    OPCODE_ARRAY_INIT         = 0x1C,
    OPCODE_ARRAY_PUSH         = 0x1D,
    OPCODE_ARRAY_GET          = 0x1E,
    OPCODE_ARRAY_SET          = 0x1F,
    OPCODE_ARRAY_LEN          = 0x20,
    OPCODE_ARRAY_CONCAT       = 0x21,
    OPCODE_ASSERT_FUNC        = 0x22,
    OPCODE_FUNC_INIT          = 0x23,
    OPCODE_FUNC_CALL          = 0x24,
    OPCODE_FUNC_TAILCALL      = 0x25,
    OPCODE_ASSERT_MACRO       = 0x26,
    OPCODE_MACRO_INIT         = 0x27,
    OPCODE_ENV_LOAD           = 0x28,
    OPCODE_ENV_STORE          = 0x29,
    OPCODE_SYNTAX_NODE_CREATE = 0x2A,
    OPCODE_INT_NODE_CREATE    = 0x2B,
    OPCODE_STR_NODE_CREATE    = 0x2C,
    OPCODE_BOOL_NODE_CREATE   = 0x2D,
    OPCODE_PRINT              = 0x2E,
    OPCODE_INPUT              = 0x2F,
    OPCODE_JUMP               = 0x30,
    OPCODE_JUMP_IF_TRUE       = 0x31,
    OPCODE_RETURN             = 0x32,
    OPCODE_PROC_EXIT          = 0x33
};

typedef struct {
    u8 operator;
    u8 operand1;
    u8 operand2;
    u8 operand3;
} Instruction;

#endif
