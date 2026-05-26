# cee-mozz

A C implementation of a runtime for Mozzarella.

## Plan

Only planning as far as the value types so far.

| Type | Definition | Validation | Operations |
|------|------------|------------|------------|
| `I64Value` | ✅ | ✅ | ✅ `add sub mul floordiv mod` |
| `AsciiStrValue` | ✅ | | `concat to_string` |
| `BoolValue` | | | `to_bool` |
| `NoneValue` | | | |
| `ArrayValue` | | | |
| `ArrayElements` | | | |
| `FuncValue` | | | |
| `MacroValue` | | | |
| `SyntaxNodeValue` | | | |
| `IntValue` | | | |
| `StrValue` | | | |
| `SmallStrValue` | | | |
| `IntTable` | | | |
| `StrTable` | | | |

A later, more detailed plan will also include running VM instructions; first
individually, and then as part of stored bytecode.

## Design documents

- [Instructions](https://github.com/masak/cee-mozz/blob/main/docs/01-instructions.md)
