Contributions from other humans are welcome.

The project uses spaces for indentation. The C code uses 4 spaces, and the Lean
code uses 2 spaces.

Typedefs such as `u32` and `i64` are declared to avoid the platform-dependent
sizes of types such as `unsigned int` and `long`.

The typedef `Offset` is used for non-nullable offsets, and `MaybeOffset` for
nullable offsets. (In the case of non-nullable offsets, we often `assert()`
that the value is defined/set.) The value used for "null" is actually
`0xA3A3A3A3`.

Strings are in general handled with the `s8()` macro, which bundles the byte
payload with the length in bytes.

The functions ending in `_new` which create new values tend to `assert()` any
inputs that can be invalid, preferring to panic rather than creating an invalid
value.

The functions ending in `_validate` return a `bool` to indicate value validity.
It is an invariant of the arena and all the values in it that a `_validate`
function should never return `false`.

Other functions reading or updating values only `assert()` things where
validity is already expected. In the case of inputs that _could_ be invalid in
an otherwise valid program (such as an index out of bounds), these functions
tend to return an error code upwards to the instruction layer instead. Again,
the distinction is whether a program that compiles could cause the error; in
that case, it should be a returned error code.

Function names are prefixed with their type. If a function dispatches on the
type of something, it's prefixed with `generic_`.

