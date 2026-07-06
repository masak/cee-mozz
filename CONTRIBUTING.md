Inspired by https://godotengine.org/article/contribution-policy-2026/ we fully
endorse the following human-centric priorities:

> Our focus will be on:
>
> * **Encouraging new contributors to become future maintainers, that involves
>   teaching and growing the understanding of new contributors.**
>     * LLMs can’t learn from specific feedback and thus can’t benefit from
>       maintainers providing feedback.
>
> * **Ensuring all contributions are made by humans who can take responsibility
>   for their code and be able and willing to fix it when needed.**
>     * AI cannot take responsibility, and we can’t trust heavy users of AI to
>       understand their code enough to fix it.

In short: feel free to use LLMs to help write implementation code and test
code, comments and documentation &mdash; but commits and pull requests can only
be made by humans. Please write commit messages and pull request descriptions
yourself, with your uniquely human voice. In online discussion (for example in
a pull request), please use your own human words, not those of an LLM.

The project uses spaces for indentation. The C code uses 4 spaces, and the Lean
code uses 2 spaces. Lines are kept at most 79 characters long.

Typedefs such as `u32` and `i64` are declared to avoid the platform-dependent
sizes of types such as `unsigned int` and `long`.

The typedef `Offset` is used for non-nullable offsets, and `MaybeOffset` for
nullable offsets. (In the case of non-nullable offsets, we often `assert()`
that the value is defined/set.) The value used for "null" is actually
`0xA3A3A3A3`.

Strings are in general handled with the `s8()` macro, which bundles the byte
payload with the length in bytes.

We do not use `const` in the code. Instead there is a ubiquitous focus on
preserving invariants and validity throughout.

In code in the value layer and the instruction layer, there are three types of
error handling, each with a different purpose:

* Invariants and postconditions can be indicated with `assert()` calls. This
  most severe level should only be used for code that is expected never to fail
  regardless of user input and function parameters.

* Function preconditions of `_new` functions and value-related functions are
  diligently tested by the functions themselves; if found to be false, the
  `vm_crash(reason)` function is called. Code in the instruction layer that
  calls these functions are expected to respect these preconditions, and
  failure to do so is considered an error in the caller code.

* When an instruction or a value-related function fails for reasons not related
  to the above two items &mdash; such as a type error, a division by zero, or
  an index out of bounds &mdash; the value-related function returns immediately
  with an enum value that indicates which type of runtime error occurred. In
  the instruction layer, this return value is diligently checked, and
  propagated upwards as a VM error, stopping execution.

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

