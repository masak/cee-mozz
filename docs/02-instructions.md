# Instructions

Instruction format: `xx yy zz ww`; four bytes (32 bits)

| abbr | meaning             |
|------|---------------------|
| `tr` | target register     |
| `rN` | source register `N` |
| `ii` | index in Int table  |
| `ss` | index in Str table  |
| `cc` | index in Code table |
| `ee` | "outer env" steps   |
| `oo` | offset in env       |
| `pp` | u16 capacity        |
| `aa` | instruction address |
| `--` | unused              |

## General

| bytes         | instruction   | description |
|---------------|---------------|-------------|
| `00 -- -- --` | `no_op`       | do nothing  |
| `01 tr r1 --` | `mov tr, r1`  | assign      |

## Int

| bytes         | instruction          | description               |
|---------------|----------------------|---------------------------|
| `02 r1 -- --` | `assert_int r1`      | assert type Int           |
| `03 tr ii ii` | `load_int tr, ^iiii` | assign Int constant       |
| `04 tr r1 r2` | `int_add tr, r1, r2` | add (Int, Int)            |
| `05 tr r1 --` | `int_neg tr, r1`     | negate (Int)              |
| `06 tr r1 r2` | `int_sub tr, r1, r2` | subtract (Int, Int)       |
| `07 tr r1 r2` | `int_mul tr, r1, r2` | multiply (Int, Int)       |
| `08 tr r1 r2` | `int_div tr, r1, r2` | divide (Int, Int) [error] |
| `09 tr r1 r2` | `int_mod tr, r1, r2` | modulo (Int, Int) [error] |

## Str

| bytes         | instruction             | description            |
|---------------|-------------------------|------------------------|
| `0a r1 -- --` | `assert_str r1`         | assert type Str        |
| `0b tr ss ss` | `load_str tr, ^ssss`    | assign Str constant    |
| `0c tr r1 --` | `stringify tr, r1`      | stringify (any)        |
| `0d tr r1 r2` | `str_concat tr, r1, r2` | concatenate (any, any) |

## Bool

| bytes         | instruction       | description          |
|---------------|-------------------|----------------------|
| `0e r1 -- --` | `assert_bool r1`  | assert type Bool     |
| `0f tr -- --` | `load_true tr`    | assign true          |
| `10 tr -- --` | `load_false tr`   | assign false         |
| `11 tr r1 --` | `boolify tr, r1`  | boolify (any)        |
| `12 tr r1 --` | `bool_neg tr, r1` | boolean negate (any) |

## None

| bytes         | instruction      | description      |
|---------------|------------------|------------------|
| `13 r1 -- --` | `assert_none r1` | assert type None |
| `14 tr -- --` | `load_none tr`   | assign none      |

## Comparison

| bytes         | instruction         | description  |
|---------------|---------------------|--------------|
| `15 tr r1 r2` | `cmp_lt tr, r1, r2` | compare (<)  |
| `16 tr r1 r2` | `cmp_le tr, r1, r2` | compare (<=) |
| `17 tr r1 r2` | `cmp_gt tr, r1, r2` | compare (>)  |
| `18 tr r1 r2` | `cmp_ge tr, r1, r2` | compare (>=) |
| `19 tr r1 r2` | `cmp_eq tr, r1, r2` | compare (==) |
| `1a tr r1 r2` | `cmp_ne tr, r1, r2` | compare (!=) |

## Array

| bytes         | instruction               | description                 |
|---------------|---------------------------|-----------------------------|
| `1b r1 -- --` | `assert_array r1`         | assert type Array           |
| `1c tr pp pp` | `array_init tr, +pppp`    | initialize array, u16 cap   |
| `1d r1 r2 --` | `array_push r1, r2`       | add element to end of array |
| `1e tr r1 r2` | `array_get tr, r1, r2`    | `tr = r1[t2]` [error]       |
| `1f r1 r2 r3` | `array_set r1, r2, r3`    | `r1[r2] = r3` [error]       |
| `20 tr r1 --` | `array_len tr, r1`        | get number of elements      |
| `21 tr r1 r2` | `array_concat tr, r1, r2` | concatenate arrays          |

## Func

| bytes         | instruction            | description      |
|---------------|------------------------|------------------|
| `22 r1 -- --` | `assert_func r1`       | assert type Func |
| `23 tr cc cc` | `func_init tr, ^cccc`  | initialize Func  |
| `24 tr r1 r2` | `func_call tr, r1, r2` | `tr = r1(...r2)` |
| `25 -- r1 r2` | `func_tailcall r1, r2` | tailcall         |

## Macro

| bytes         | instruction            | description       |
|---------------|------------------------|-------------------|
| `26 r1 -- --` | `assert_macro r1`      | assert type Macro |
| `27 tr cc cc` | `macro_init tr, ^cccc` | initialize Macro  |

## Environment

| bytes         | instruction              | description                    |
|---------------|--------------------------|--------------------------------|
| `28 tr ee oo` | `env_load tr, ^ee, ^oo`  | load from environment [error]  |
| `29 r1 ee oo` | `env_store r1, ^ee, ^oo` | store into environment [error] |

## SyntaxNode

| bytes         | instruction                 | description            |
|---------------|-----------------------------|------------------------|
| `2a tr r1 --` | `syntax_node_create tr, t1` | assign SyntaxNode      |
| `2b tr r1 --` | `int_node_create tr, r1`    | assign IntNode (Int)   |
| `2c tr r1 --` | `str_node_create tr, r1`    | assign StrNode (Str)   |
| `2d tr r1 --` | `bool_node_create tr, r1`   | assign BoolNode (Bool) |

## Input/output

| bytes         | instruction    | description            |
|---------------|----------------|------------------------|
| `2e r1 -- --` | `print r1`     | print string + newline |
| `2f tr r1 --` | `input tr, r1` | prompt and input       |

## Control

| bytes         | instruction              | description           |
|---------------|--------------------------|-----------------------|
| `30 -- aa aa` | `jump ^aaaa`             | unconditional jump    |
| `31 r1 aa aa` | `jump_if_true r1, ^aaaa` | conditional jump      |
| `32 r1 -- --` | `return r1`              | return                |
| `33 r1 -- --` | `proc_exit r1`           | exit with status code |

