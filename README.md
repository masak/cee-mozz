# cee-mozz

A C implementation of a runtime for Mozzarella.

## Plan

See further planning among the documents below. Here is the current progress
defining the various types of the runtime:

| Type | Definition | Validation | Operations |
|------|------------|------------|------------|
| `I64Value` | ✅ | ✅ | ✅ `add sub mul floordiv mod` |
| `AsciiStrValue` | ✅ | | `concat to_string` |
| `BoolValue` | | | `to_bool` |
| `NoneValue` | | | |
| `UninitializedValue` | | | |
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

## Design documents

- Value types (not yet written)
- [Instructions](https://github.com/masak/cee-mozz/blob/main/docs/01-instructions.md)
- Instruction semantics (not yet written)
- Execution (not yet written)

