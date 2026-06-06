#ifndef TYPES_H
#define TYPES_H

#define TAG_I64             0x01
#define TAG_ASCII_STR       0x02
/* skipping bool 0x03, none 0x04, uninitialized 0x05 for now */
#define TAG_ARRAY           0x06
#define TAG_ARRAY_ELEMENTS  0x07
#define TAG_FUNC            0x08
/* 0x09 reserved for MacroValue */
#define TAG_ENVIRONMENT     0x0a
#define TAG_CODE_UNIT       0x0b

#endif

