The project is divided into several layers. Higher layers can call into lower
layers, but not vice versa.

The **arena layer** declares the `Arena` type. An arena holds all values of
interest at runtime. There's also a `SeenSet` which helps avoid cycles during
value graph traversals.

The **value layer** declares all the `Value` types, as well as functions to
create new values from old ones, or select existing values. These functions
tend to `vm_crash()` on things related to value validity (so that a VM crash
indicates a failure of the caller to uphold preconditions); for value functions
that can lead to runtime errors, an `Outcome` is returned which is either
`OUTCOME_OK` or a specific `Outcome` representing a runtime error type.

The **bytecode layer** declares all instruction opcodes, along with code for
how to interpret instructions for each opcode. The main runloop runs code in
`CodeUnit`s and handles calls between `CodeUnit`s. It also propagate errors
from the value functions, handling them appropriately.

