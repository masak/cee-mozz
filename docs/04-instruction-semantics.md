# Instructions

We assume there's a `reg` array of N registers; each register holds an offset
into the arena (unless its unset). We do bounds checking when we read and write
from this array, but ideally this should never give an error, assuming code
from validated `CodeUnit`s. We also check that a register is not unset before
reading from it, but this should also have been ruled out by validation.

We also assume there's an `int_table`, a `str_table`, and a `code_table`, all
associated with the currently executing routine. We perform overly-cautious
checking of the tags of the data we read from these tables; again, this should
never give an error, assuming these tables have been validated.

For all the arithmetic operations on `Int`, the code does a bit more than shown
here: it checks whether one or both of the operands is an `I64Value`. If both
are `I64Value`s, it attempts to carry out the operation as an i64 operation.
If this succeeds without overflow, then the resulting `I64Value` is the final
result. Otherwise, or if one or both of the original operands was an
`IntValue`, the appropriate operands are widened to `IntValue`s, and then the
operation is carried out as an `IntValue` operation, yielding the result. For
simplicity in the below code, we only show the `IntValue` operation.

The value layer deals in total functions, by sometimes expecting an additional
"fallback offset" parameter which forces the caller (the instruction layer) to
consider what should be a reasonable fallback. In case of division, for
example, the instruction code checks the denominator for zero up-front, and
if zero, an error is issued without ever calling the division function; if not
zero, the denominator is supplied as a fallback, but this fallback is never
used, since in this branch the denominator is known not to be zero. This
pattern, tentatively named "cynical error handling", ought to be more widely
known than it is.

## Assign register

Instruction: `<tr> = <r1>`

```c
reg[tr] = reg[r1];
```

## Assert type Int

Instruction: `Int.assert(<r1>)`

```c
u64 INT_TAGS[] = { TAG_I64, TAG_INT };

assert_value_tag_is_one_of(INT_TAGS, reg[t1]);
```

## Assign Int constant

Instruction: `<tr> = Int.const(iiii)`

```c
reg[tr] = int_table_lookup(int_table, iiii);
```

## Add (Int, Int)

Instruction: `<tr> = Int.add(<r1>, <r2>)`

```c
reg[tr] = int_add(reg[r1], reg[r2]);
```

## Negate (Int)

Instruction: `<tr> = Int.negate(<r1>)`

```c
Offset zero = /* initialize zero */;
reg[tr] = int_subtract(zero, reg[r1]);
```

## Subtract (Int, Int)

Instruction: `<tr> = Int.sub(<r1>, <r2>)`

```c
reg[tr] = int_subtract(reg[r1], reg[r2]);
```

## Multiply (Int, Int)

Instruction: `<tr> = Int.mul(<r1>, <r2>)`

```c
reg[tr] = int_multiply(reg[r1], reg[r2]);
```

## Divide (Int, Int) [error]

Instruction: `<tr> = Int.idiv(<r1>, <r2>)`

```c
Offset denominator = reg[r2];
if (int_is_zero(denominator)) {
    error("Division by zero (//)");
}
else {
    reg[tr] = int_divide(reg[r1], denominator, denominator);
}
```

## Modulo (Int, Int) [error]

Instruction: `<tr> = Int.mod(<r1>, <r2>)`

```c
Offset denominator = reg[r2];
if (int_is_zero(denominator)) {
    error("Division by zero (%)");
}
else {
    reg[tr] = int_modulo(reg[r1], denominator, denominator);
}
```

## Assert type Str

Instruction: `Str.assert(<r1>)`

## Assign Str constant

Instruction: `<tr> = Str.const(ssss)`

## Stringify (any)

Instruction: `<tr> = stringify(<r1>)`

## Concatenate (any, any)

Instruction: `<tr> = Str.concat(<r1>, <r2>)`

## Assert type Bool

Instruction: `Bool.assert(<r1>)`

## Assign true

Instruction: `<tr> = true`

## Assign false

Instruction: `<tr> = false`

## Boolify (any)

Instruction: `<tr> = boolify(<r1>)`

## Boolean negate (any)

Instruction: `<tr> = not(<r1>)`

## Assert type None

Instruction: `None.assert(<r1>)`

## Assign none

Instruction: `<tr> = none`

## Compare (<)

Instruction: `<tr> = cmpLt(<r1>, <r2>)`

## Compare (<=)

Instruction: `<tr> = cmpLe(<r1>, <r2>)`

## Compare (>)

Instruction: `<tr> = cmpGt(<r1>, <r2>)`

## Compare (>=)

Instruction: `<tr> = cmpGe(<r1>, <r2>)`

## Compare (==)

Instruction: `<tr> = cmpEq(<r1>, <r2>)`

## Compare (!=)

Instruction: `<tr> = cmpNe(<r1>, <r2>)`

## Assert type Array

Instruction: `Array.assert(<r1>)`

## Initialize array, u16 cap

Instruction: `<tr> = Array.init(pppp)`

## Add element to end of array

Instruction: `<tr>.push(<r1>)`

## Index (Array, Int)

Instruction: `<tr> = <r1>[<r2>]`

## Assign index (Array, Int)

Instruction: `<tr>[<r1>] = <r2>`

## Get number of elements

Instruction: `<tr> = Array.length(<r1>)`

## Concatenate (Array, Array)

Instruction: `<tr> = Array.concat(<r1>, <r2>)`

## Initialize Func

Instruction: `<tr> = Func.init(cccc)`

## Call Func with arguments Array

Instruction: `<tr> = <r1>(...<r2>)`

## Tailcall

Instruction: `tailcall <r1>(...<r2>)`

## Initialize Macro

Instruction: `<tr> = Macro.init(cccc)`

## Load from environment

Instruction: `<tr> = load(<ee>, <oo>)`

## Store into environment

Instruction: `store(<r1>, <ee>, <oo>)`

## Assign SyntaxNode

Instruction: `<tr> = SyntaxNode.init(...<r1>)`

## ...IntNode (Int)

Instruction: `<tr> = SyntaxNode.IntNode(...<r1>)`

## ...StrNode (Str)

Instruction: `<tr> = SyntaxNode.StrNode(...<r1>)`

## ...BoolNode (Bool)

Instruction: `<tr> = SyntaxNode.BoolNode(...<r1>)`

## Unconditional jump

Instruction: `jmp <aaaa>`

## Conditional jump

Instruction: `jmp <aaaa> if <r1>`

## Return

Instruction: `ret <r1>`

## Exit with status code

Instruction: `Proc.exit(<r1>)`

## Print string + newline

Instruction: `print(<r1>)`

## Prompt and input

Instruction: `<tr> = prompt(<r1>)`

