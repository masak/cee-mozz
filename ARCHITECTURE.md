The project is divided into several layers. Higher layers can call into lower
layers, but not vice versa.

The **arena layer** declares the `Arena` type. An arena holds all values of
interest at runtime. There's also a `SeenSet` which helps avoid cycles during
value graph traversals.

The **value layer** declares all the `Value` types, as well as functions to
create new values from old ones, or select existing values. These functions
tend to `assert()` things related to value validity (so these asserts should
never fail), but for things that can lead to runtime errors, an enum value
which is either `Ok` or one of the runtime exception types.

The **bytecode layer** declares all instruction opcodes, along with code for
how to interpret instructions for each opcode. The main runloop runs code in
CodeUnits and handles calls between CodeUnits. It also propagate errors from
the value functions, handling them appropriately.

