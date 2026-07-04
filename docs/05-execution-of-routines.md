# Execution of routines

Functions and macros are similar. They have different tags but the same data
layout. Both are first-class values in Mozzarella. We write **routine** when we
want to talk about functions and macros as a unified category.

We distinguish between _internal_ and _external_ calls to a routine. An
internal call is made by a `func_call` or `func_tailcall` instruction in
bytecode. The routine containing the instruction becomes the caller.

An external call is initiated by the VM code itself. It is how any green thread
or task of execution begins; in fact, since any `func_call` or `func_tailcall`
instruction can only be executed from within an already-running routine, an
external call is the only way to sidestep the Catch-22 of how to even initiate
execution.

Only functions can be called internally; macros can't. Mozzarella code cannot
call a macro; only the macro expander can call macros, and it uses an external
call to do so.

We refer to the called routine as "the callee". The function, macro, or VM that
made the call is "the caller".

In order to represent the ongoing execution of the callee, there needs to be an
**activation record**, holding the program counter and the current state of the
registers, as well as a link back to the caller's activation record (if any).
For external calls, and internal calls using `func_call`, this activation
record is created fresh at the point of the call. For internal calls using
`func_tailcall`, the caller's activation record is transferred from caller to
callee (just like control is), making the caller effectively disappear, and
making the departed caller's caller the new callee's caller.

At any point during execution, therefore, the activation records for the
currently-active routines form a linked list. Only the first/outermost
activation record was created via an external call (and thus may be to either a
`FuncValue` or a `MacroValue`); the rest were created via internal calls (and
must be to a `FuncValue`). The situation is complicated somewhat by the fact
that any activation record created for a particular callee might have been
"hijacked" in-place into being an activation record for _its_ callee
(including, theoretically, replacing the outermost activation record for a call
to a macro with one for a call to a function).

The top level of a compilation unit has been compiled into a `CodeUnit` with
zero parameters, and with a non-identifier name such as "%top%" (which is not
registered in any environment). The external call that begins execution of the
entire program is to a `FuncValue` formed from this `CodeUnit`.

Calling a routine, whether an external or internal call, whether with
`func_call` or `func_tailcall`, starts with a **parameter binding phase**
before executing the routine's first instruction. Parameters are by VM
convention located in the first N registers of the newly-created activation
record.

In more detail:

* Both positional and named (not yet implemented) arguments are stored in
  registers, in the order corresponding to the order those parameters were
  declared in the callee.

* If there isn't a (positional or named) argument provided for a corresponding
  _required_ parameter, then parameter binding (and hence the whole call) fails
  at this point with a runtime error related to insufficient arguments.

* For any (positional or named) _optional_ parameters (not yet implemented)
  without a corresponding argument, parameter binding still proceeds without
  error. The register in question is left `UNSET`; code at the start of the
  routine body proper will initialize this register if it's `UNSET` (and leave
  it alone if it was already initialized). This initialization code also covers
  the case of a parameter with an initializer expression.

* In the absence of a positional _rest_ parameter (not yet implemented), if
  there's an excess of positional arguments, parameter binding fails with a
  runtime error about how there are too many positional arguments.

* If a positional rest parameter is present, an `ArrayValue` is created at this
  point, and the excess positional arguments (if any) are pushed to it, in
  order. No excess positional arguments just means an empty array.

* In the absence of a named rest parameter (not yet implemented), if there's
  an excess of named arguments, parameter binding fails with a runtime error
  about how there are named arguments without a corresponding parameter, either
  "regular" or rest.

* If a named rest parameter is present, a `DictValue` is created at this point,
  and the excess named arguments (if any) are added as key/value entries to it,
  the keys being the names and the values being the argument values. The order
  in which these entries are added corresponds to the order of the named
  arguments in the argument list. No excess named arguments just means an empty
  dictionary.

The parameter binding phase might fail; if it does, no instructions at all are
executed in the callee. If the parameter binding phase succeeds, all the
registers corresponding to parameters will now have well-defined values, except
for the optional parameters that did not have a corresponding argument. It is a
VM convention that all these are treated (in declaration order) in an
unskippable "prelude" in the instructions, such that after this prelude, all
the parameter registers are guaranteed to be initialized. ("Convention",
because this depends on the compiler actually generating the bytecode that way;
there are certainly valid units of bytecode that do not work this way, but the
compiler will always produce code that does work this way.)

At this point, execution of the instructions begins. (Including, if it is
present, the above-mentioned parameter-initialization prelude.)

Instructions take up four bytes each. The "program counter" (stored in the
activation record) starts at 0, and typically increases by 4 after each
executed instruction. The exceptions to this are the instructions `jump` and
`jump_if_true`, `return`, and `proc_exit`; in the case of `jump_if_true`, the
program counter might be incremented by 4 (if the register was falsy); in the
other cases, something else happens (described below) and the program counter
is not incremented.

Instructions normally read or copy offsets/values in the arena, or in
associated inttables/strtables/codetables. Some instructions will issue a
runtime error and stop the VM. (At this point, we do not specify a mechanism to
catch any such runtime errors.)

For the `jump ^aaaa` instruction, the program counter is set to the address
`aaaa` (which must be a multiple of 4). The program counter is not subsequently
incremented.

For the `jump_if_true r1, ^aaaa` instruction, the value `r1` is checked for
truthiness. If it's truthy, the program counter is set to the address `aaaa`
(which must be a multiple of 4), and then not incremented. If `r1` is falsy,
the program counter is incremented by 4.

For the `func_call tr, r1, r2` instruction, we verify that `r1` indeed refers
to a `FuncValue` (just as `assert_func r1` would, signaling an error if it does
not) and that `r2` refers to an `ArrayValue`; then we create a new activation
record for the callee (based on details from `r1`); we proceed through
parameter binding between `r2` and the new activation record; finally we
execute the instructions in the callee. When after possibly quite a while when
the callee issues a `return` with a value, we store that value into `tr`, and
increment the program counter by 4. (At this point, we can also discard the
callee's activation record.) This process is also described below from the
point of view of the callee's `return` instruction.

For the `func_tailcall r1, r2` instruction, we verify that `r1` refers to a
`FuncValue` and that `r2` refers to an `ArrayValue`; if needed, we extend the
size of the register bank to be the size required by the callee (based on
details from the `CodeUnit` stored in the `FuncValue` referred to by `r1`); we
don't shrink the register bank if the callee requires fewer registers; we
clear all the registers in the callee to `UNSET`; we perform parameter binding 
between `r2` and the (pre-existing) activation record; finally we execute the
instructions in the callee. There is no future point where we come back to the
`func_tailcall` instruction in the caller; when the callee returns, it is to
the caller's caller.

Finally, the two instructions that can end the life cycle of an activation
record are `return`, and `proc_exit`.

In the case of `return r1`, we look up the caller's activation frame (if there
is one). If there is no caller activation frame, the value `r1` is returned as
the result of the original external call (and execution stops). If there is a
caller activation frame, we find in the caller's activation frame's current
instruction (the one pointed to by its program counter) (which will be a
`func_call tr, r1, r2`) the register `tr` to which to assign the result of the
call, and we assign our `r1` to its `tr`. (At this point, we can also discard
the current activation record, as we make the caller's activation record the
current one.) Execution then continues after the caller's `func_call`
instruction.

The `proc_exit r1` instruction requires of the entire VM to return to its
invoking process with the status code in `r1` (which needs to be confimed to
be a small-enough integer). Conceptually, this would also involve unwinding the
current call stack, looking for finalization-like code (such as `LEAVE` or
`END` blocks) to run; but currently we have no such features, and so we can
skip this process-ending bookkeeping for the moment.

