# Instructions

We assume there's a `reg` array of N registers; each register holds an offset
into the arena (unless its unset). We do bounds checking when we read and write
from this array, but ideally this should never give an error, assuming code
from validated `CodeUnit`s. We also check that a register is not unset before
reading from it, but this should also have been ruled out by validation.

We also assume there's an `inttable`, a `strtable`, and a `codetable`, all
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

## Do nothing

Instruction: `no_op`

```python
# do nothing
```

## Assign register

Instruction: `mov tr, r1`

```python
reg[tr] = reg[r1]
```

## Assert type Int

Instruction: `assert_int r1`

```python
# int compatible tags: TAG_I64, TAG_INT
assert_int_compatible(reg[r1])
```

## Assign Int constant

Instruction: `load_int tr, ^iiii`

```python
reg[tr] = inttable[iiii]
```

## Add (Int, Int)

Instruction: `int_add tr, r1, r2`

```python
assert_int_compatible(reg[r1])
assert_int_compatible(reg[r2])
reg[tr] = int_add(reg[r1], reg[r2])
```

## Negate (Int)

Instruction: `int_neg tr, r1`

```python
assert_int_compatible(reg[r1])
reg[tr] = int_negate(zero, reg[r1])
```

## Subtract (Int, Int)

Instruction: `int_sub tr, r1, r2`

```python
assert_int_compatible(reg[r1])
assert_int_compatible(reg[r2])
reg[tr] = int_subtract(reg[r1], reg[r2])
```

## Multiply (Int, Int)

Instruction: `int_mul tr, r1, r2`

```python
assert_int_compatible(reg[r1])
assert_int_compatible(reg[r2])
reg[tr] = int_multiply(reg[r1], reg[r2])
```

## Divide (Int, Int) [error]

Instruction: `int_div tr, r1, r2`

```python
assert_int_compatible(reg[r1])
assert_int_compatible(reg[r2])
if int_is_zero(reg[r2]):
    raise "Division by zero (//)"
else:
    reg[tr] = int_divide(reg[r1], reg[r2], reg[r2])
```

## Modulo (Int, Int) [error]

Instruction: `int_mod tr, r1, r2`

```python
assert_int_compatible(reg[r1])
assert_int_compatible(reg[r2])
if int_is_zero(reg[r2]):
    raise "Division by zero (%)"
else:
    reg[tr] = int_modulo(reg[r1], reg[r2], reg[r2])
```

## Assert type Str

Instruction: `assert_str r1`

```python
# str compatible tags: TAG_ASCII_STR, TAG_STR, TAG_SMALL_STR
assert_str_compatible(reg[r1])
```

## Assign Str constant

Instruction: `load_str tr, ^ssss`

```python
reg[tr] = strtable[iiii]
```

## Stringify (any)

Instruction: `stringify tr, r1`

```python
reg[tr] = generic_to_string(reg[r1])
```

## Concatenate (any, any)

Instruction: `str_concat tr, r1, r2`

```python
reg[tr] = generic_str_concat(reg[r1], reg[r2])
```

## Assert type Bool

Instruction: `assert_bool r1`

```python
assert_bool(reg[r1])
```

## Assign true

Instruction: `load_true tr`

```python
reg[tr] = bool_true()
```

## Assign false

Instruction: `load_false tr`

```python
reg[tr] = bool_false()
```

## Boolify (any)

Instruction: `boolify tr, r1`

```python
reg[tr] = generic_to_bool(reg[r1])
```

## Boolean negate (any)

Instruction: `bool_neg tr, r1`

```python
reg[tr] = generic_bool_negate(reg[r1])
```

## Assert type None

Instruction: `assert_none r1`

```python
assert_none(reg[r1])
```

## Assign none

Instruction: `load_none tr`

```python
reg[tr] = none_none()
```

## Compare (<)

Instruction: `cmp_lt tr, r1, r2`

```python
reg[tr] = generic_compare_lt(reg[r1], reg[r2])
```

## Compare (<=)

Instruction: `cmp_le tr, r1, r2`

```python
reg[tr] = generic_compare_le(reg[r1], reg[r2])
```

## Compare (>)

Instruction: `cmp_gt tr, r1, r2`

```python
reg[tr] = generic_compare_gt(reg[r1], reg[r2])
```

## Compare (>=)

Instruction: `cmp_ge tr, r1, r2`

```python
reg[tr] = generic_compare_ge(reg[r1], reg[r2])
```

## Compare (==)

Instruction: `cmp_eq tr, r1, r2`

```python
reg[tr] = generic_compare_eq(reg[r1], reg[r2])
```

## Compare (!=)

Instruction: `cmp_ne tr, r1, r2`

```python
reg[tr] = generic_compare_ne(reg[r1], reg[r2])
```

## Assert type Array

Instruction: `assert_array r1`

```python
assert_array(reg[r1])
```

## Initialize array, u16 cap

Instruction: `array_init tr, +pppp`

```python
reg[tr] = array_init(pppp)
```

## Add element to end of array

Instruction: `array_push r1, r2`

```python
assert_array(reg[r1])
array_push(reg[r1], reg[r2])
```

## Index (Array, Int)

Instruction: `array_get tr, r1, r2`

```python
assert_array(reg[r1])
assert_int_compatible(reg[r2])
reg[tr] = array_get_index(reg[r1], reg[r2])
```

## Assign index (Array, Int)

Instruction: `array_set r1, r2, r3`

```python
assert_array(reg[r1])
assert_int_compatible(reg[r2])
array_set(reg[r1], reg[r2], reg[r3])
```

## Get number of elements

Instruction: `array_len tr, r1`

```python
assert_array(reg[r1])
reg[tr] = array_len(reg[r1])
```

## Concatenate (Array, Array)

Instruction: `array_concat tr, r1, r2`

```python
assert_array(reg[r1])
assert_array(reg[r2])
reg[tr] = array_concat(reg[r1], reg[r2])
```

## Assert type Func

Instruction: `assert_func r1`

## Initialize Func

Instruction: `func_init tr, ^cccc`

```python
reg[tr] = func_new(env, codetable[cccc])
```

## Call Func with arguments Array

Instruction: `func_call tr, r1, r2`

```python
reg[tr] = func_new(env, codetable[cccc])
```

## Tailcall

Instruction: `func_tailcall r1, r2`

## Assert type Macro

Instruction: `assert_macro r1`

## Initialize Macro

Instruction: `macro_init tr, ^cccc`

```python
reg[tr] = macro_new(env, codetable[cccc])
```

## Load from environment

Instruction: `env_load tr, ^ee, ^oo`

## Store into environment

Instruction: `env_store r1, ^ee, ^oo`

## Assign SyntaxNode

Instruction: `syntax_node_create tr, t1`

## Assign IntNode (Int)

Instruction: `int_node_create tr, r1`

## Assign StrNode (Str)

Instruction: `str_node_create tr, r1`

## Assign BoolNode (Bool)

Instruction: `bool_node_create tr, r1`

## Print string + newline

Instruction: `print r1`

## Prompt and input

Instruction: `input tr, r1`

## Unconditional jump

Instruction: `jump ^aaaa`

## Conditional jump

Instruction: `jump_if_true r1, ^aaaa`

## Return

Instruction: `return r1`

## Exit with status code

Instruction: `proc_exit r1`

