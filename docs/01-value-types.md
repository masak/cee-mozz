# Value types

Listed in approximate order from small/simple to large/complex.

First-class types (which are allowed in registers) have names ending in
"Value". The rest are second-class types.

# I64Value

A signed 64-bit integer. (See also `IntValue` below for arbitrary-precision
integers.)

```
.------.-------------.
| 0x01 | i64 payload |
'------'-------------'
```

# AsciiStrValue

A sequence of ASCII characters. (See also `StrValue` and `SmallStrValue`
below.)

```
.------.----------------.---------------.
| 0x02 | length (bytes) | bytes payload |
'------'----------------'---------------'
```

# BoolValue

A single boolean value, `true` or `false`.

```
.------.-------------.
| 0x03 | i32 payload |
'------'-------------'
```

# NoneValue

A unique singleton value, `none`.

```
.------.
| 0x04 |
'------'
```

# UninitializedValue

Mozzarella, the TypeScript implementation, has an `UninitializedValue` type,
which qualifies as a `Value` everywhere, but which generates a runtime error
whenever it is read; this is to simulate using a variable which has not yet
been initialized.

In `cee-mozz`, we can avoid this value type; instead whenever we would have
pointed an offset (in the arena, or in a register) to an `UninitializedValue`,
we instead let that offset have the special value `0xA3A3A3A3`, which
designates the state of being unset or uninitialized.

# ArrayValue

A sequence of (mutable) cells. The sequence can grow or shrink while retaining
its "object identity".

```
.------.-------------------.--------------------------.
| 0x06 | length (elements) | pointer to ArrayElements |
'------'-------------------'--------------------------'
```

# ArrayElements

A sequence of (mutable) cells. This sequence can not grow or shrink; instead
of growing it when needed, we allocate a new one, and reassign the ArrayValue's
pointer to it.

```
.------.---------------------.----------------------------.
| 0x07 | capacity (elements) | (element) pointers payload |
'------'---------------------'----------------------------'
```

# FuncValue

A callable function. Current design follows Mozzarella v1.0 conventions,
without support for optional parameters, rest parameters, parameter defaults,
or named parameters.

```
.------.-----.----------.
| 0x08 | env | codeunit |
'------'-----'----------'
```

Each instruction is 4 bytes long. The instruction format is described in
`docs/02-instructions.md`.

# MacroValue

A callable macro.

```
.------.-----.----------.
| 0x09 | env | codeunit |
'------'-----'----------'
```

# Environment

```
.------.-----------.-------------.---------.
| 0x0a | outer env | entry count | entries |
'------'-----------'-------------'---------'
```

Each entry is worth two u32s:

```
.-----------------.------------.
| entry writable? | entry cell |
'-----------------'------------'
```

# CodeUnit

```
.------.--------------------.----------------.----------------.------------.
| 0x0b | outer_codeunit ptr | parameters ptr | register count | env length |
'------+--------------+-----+----------------+-------+--------+------------'
       | inttable ptr | strtable ptr | codetable ptr |
       +--------------+--------------+---------------'
       | instr count  | instructions |
       '--------------'--------------'
```

# Parameters

```
.------.-------------.---------.
| 0x13 | param count | entries |
'------'-------------'---------'
```

Each entry is worth one u32:

```
.---------.
| str ptr |
'---------'
```

# SyntaxNodeValue

Represents a syntax node. In cee-mozz, we use this value both internally in
the compiler, and as reflected "runtime" values.

```
.------.----------------.------------------------.--------.----------.
| 0x0c | kind (str ptr) | payload (int/str/bool) | child# | children |
'------'----------------'------------------------'--------'----------'
```

# IntValue

An arbitrary-precision integer. (See also `I64Value` above.)

```
.------.------.---------------.--------------.
| 0x0d | sign | length (u64s) | u64s payload |
'------'------.---------------'--------------'
```

# StrValue

A sequence of UTF-8-encoded Unicode code points. (See also `AsciiStrValue`
above and `SmallStrValue` below.)

```
.------.----------------.----------------------.--------------.
| 0x0e | length (bytes) | length (code points) | utf8 payload |
'------'----------------'----------------------'--------------'
```

# SmallStrValue

A sequence of UTF-8-encoded Unicode code points. At most 8 bytes in length.
Padded with `\0` bytes at the start. (See also `AsciiStrValue` and `StrValue`
above.)

```
.------.---------------------.
| 0x0f | 8-byte utf8 payload |
'------'---------------------'
```

# IntTable

An indexable sequence of integers. (`IntValue` or `I64Value`.) Used in
`CodeUnit` when instructions need to load integer values from somewhere.

```
.------.---------------.------------------------.
| 0x10 | length (ints) | (int) pointers payload |
'------'---------------'------------------------'
```

# StrTable

An indexable sequence of strings. (`AsciiStrValue`, `StrValue`, or
`SmallStrValue`.) Used in `CodeUnit` when instructions need to load string
values from somewhere.

```
.------.---------------.------------------------.
| 0x11 | length (strs) | (str) pointers payload |
'------'---------------'------------------------'
```

# CodeTable

An indexable sequence of code units. Used in `CodeUnit` when instructions need
to create a new `FuncValue` or `MacroValue`.

```
.------.--------------------.-----------------------------.
| 0x12 | length (codeunits) | (codeunit) pointers payload |
'------'--------------------'-----------------------------'
```

