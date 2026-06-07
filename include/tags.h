#ifndef TAGS_H
#define TAGS_H

#define TAG_I64             0x01
#define TAG_ASCII_STR       0x02
/* skipping bool 0x03, none 0x04, uninitialized 0x05 for now */
#define TAG_ARRAY           0x06
#define TAG_ARRAY_ELEMENTS  0x07
#define TAG_FUNC            0x08
#define TAG_MACRO           0x09
#define TAG_ENVIRONMENT     0x0a
#define TAG_CODE_UNIT       0x0b
/* skipping syntaxnode 0x0c, int 0x0d, str 0x0e, smallstr 0x0f */
#define TAG_INT_TABLE       0x10
#define TAG_STR_TABLE       0x11
#define TAG_CODE_TABLE      0x12

#endif

