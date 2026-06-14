# cee-mozz

A C implementation of a runtime for Mozzarella.

## Building

```bash
$ make
$ make test
```

## Plan

See further planning among the documents below. Here is the current progress
defining the various types of the runtime:

| Type | Definition | Validation | Operations |
|------|------------|------------|------------|
| `I64Value` | ✅ | ✅ | ✅ `add sub mul floordiv mod` |
| `AsciiStrValue` | ✅ | ✅ | ✅ `concat to_string` |
| `BoolValue` | | | `to_bool` |
| `NoneValue` | | | |
| `ArrayValue` | ✅ | ✅ | ✅ `push get set length concat` |
| `ArrayElements` | ✅ | ✅ | ✅ |
| `FuncValue` | ✅ | (✅) | ✅ |
| `MacroValue` | ✅ | (✅) | ✅ |
| `Environment` | ✅ | (✅) | ✅ |
| `CodeUnit` | ✅ | (✅) | ✅ |
| `SyntaxNodeValue` | | | |
| `IntValue` | ✅ | ✅ | ✅ `add sub mul floordiv mod` |
| `StrValue` | | | `concat` |
| `SmallStrValue` | | | `concat` |
| `IntTable` | ✅ | ✅ | ✅ |
| `StrTable` | ✅ | ✅ | ✅ |
| `CodeTable` | ✅ | ✅ | ✅ |

## Design documents

- [Value types](https://github.com/masak/cee-mozz/blob/main/docs/01-value-types.md)
- [Instructions](https://github.com/masak/cee-mozz/blob/main/docs/02-instructions.md)
- [Environments](https://github.com/masak/cee-mozz/blob/main/docs/03-environments.md)
- [Instruction semantics](https://github.com/masak/cee-mozz/blob/main/docs/04-instruction-semantics.md)
- Execution of functions (tbw)
- Execution of a program (tbw)
- Converting AST to instructions (tbw)
- Three types of compilation unit (tbw)
- Mark-and-sweep garbage collection (tbw)

