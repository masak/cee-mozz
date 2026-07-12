# cee-mozz

A C implementation of a runtime for Mozzarella.

## Building

```bash
$ make
$ make test
```

## Plan

See further planning among the documents below. Here is the current progress
defining the various types in the runtime:

| Type | Implemented | New error model |
|------|-------------|-----------------|
| `I64Value` | ✅ | ✅ |
| `AsciiStrValue` | ✅ | ✅ |
| `BoolValue` | | |
| `NoneValue` | | |
| `ArrayValue` | ✅ | ✅ |
| `ArrayElements` | ✅ | ✅ |
| `FuncValue` | ✅ | ✅ |
| `MacroValue` | ✅ | ✅ |
| `Environment` | ✅ | ✅ |
| `CodeUnit` | ✅ | ✅ |
| `SyntaxNodeValue` | ✅ | ✅ |
| `IntValue` | ✅ | ✅ |
| `StrValue` | ✅ | ✅ |
| `SmallStrValue` | ✅ | ✅ |
| `IntTable` | ✅ | |
| `StrTable` | ✅ | |
| `CodeTable` | ✅ | |

## Design documents

- [Value types](https://github.com/masak/cee-mozz/blob/main/docs/01-value-types.md)
- [Instructions](https://github.com/masak/cee-mozz/blob/main/docs/02-instructions.md)
- [Environments](https://github.com/masak/cee-mozz/blob/main/docs/03-environments.md)
- [Instruction semantics](https://github.com/masak/cee-mozz/blob/main/docs/04-instruction-semantics.md)
- [Execution of routines](https://github.com/masak/cee-mozz/blob/main/docs/05-execution-of-routines.md)
- Execution of a program (tbw)
- Converting AST to instructions (tbw)
- Three types of compilation unit (tbw)
- [Mark-and-compact garbage collection](https://github.com/masak/cee-mozz/blob/main/docs/09-garbage-collection.md)
- Representing immediate values using unaligned offsets (tbw)
- A small relational database (tbw)

