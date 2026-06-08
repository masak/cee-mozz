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
| `aa` | instruction address |
| `pp` | u16 capacity        |
| `--` | unused              |

## Assign

| bytes         | instruction   | description         |
|---------------|---------------|---------------------|
| `00 tr r1 --` | `<tr> = <r1>` | assign register     |

## Int

| bytes         | instruction                   | description               |
|---------------|-------------------------------|---------------------------|
| `01 r1 -- --` | `Int.assert(<r1>)`            | assert type Int           |
| `02 tr ii ii` | `<tr> = Int.const(iiii)`      | assign Int constant       |
| `03 tr r1 r2` | `<tr> = Int.add(<r1>, <r2>)`  | add (Int, Int)            |
| `04 tr r1 --` | `<tr> = Int.negate(<r1>)`     | negate (Int)              |
| `05 tr r1 r2` | `<tr> = Int.sub(<r1>, <r2>)`  | subtract (Int, Int)       |
| `06 tr r1 r2` | `<tr> = Int.mul(<r1>, <r2>)`  | multiply (Int, Int)       |
| `07 tr r1 r2` | `<tr> = Int.idiv(<r1>, <r2>)` | divide (Int, Int) [error] |
| `08 tr r1 r2` | `<tr> = Int.mod(<r1>, <r2>)`  | modulo (Int, Int) [error] |

## Str

| bytes         | instruction                     | description            |
|---------------|---------------------------------|------------------------|
| `09 r1 -- --` | `Str.assert(<r1>)`              | assert type Str        |
| `0a tr ss ss` | `<tr> = Str.const(ssss)`        | assign Str constant    |
| `0b tr r1 --` | `<tr> = stringify(<r1>)`        | stringify (any)        |
| `0c tr r1 r2` | `<tr> = Str.concat(<r1>, <r2>)` | concatenate (any, any) |

## Bool

| bytes         | instruction            | description          |
|---------------|------------------------|----------------------|
| `0d r1 -- --` | `Bool.assert(<r1>)`    | assert type Bool     |
| `0e tr -- --` | `<tr> = true`          | assign true          |
| `0f tr -- --` | `<tr> = false`         | assign false         |
| `10 tr r1 --` | `<tr> = boolify(<r1>)` | boolify (any)        |
| `11 tr r1 --` | `<tr> = not(<r1>)`     | boolean negate (any) |

## None

| bytes         | instruction         | description      |
|---------------|---------------------|------------------|
| `12 r1 -- --` | `None.assert(<r1>)` | assert type None |
| `13 tr -- --` | `<tr> = none`       | assign none      |

## Comparison

| bytes         | instruction                | description  |
|---------------|----------------------------|--------------|
| `14 tr r1 r2` | `<tr> = cmpLt(<r1>, <r2>)` | compare (<)  |
| `15 tr r1 r2` | `<tr> = cmpLe(<r1>, <r2>)` | compare (<=) |
| `16 tr r1 r2` | `<tr> = cmpGt(<r1>, <r2>)` | compare (>)  |
| `17 tr r1 r2` | `<tr> = cmpGe(<r1>, <r2>)` | compare (>=) |
| `18 tr r1 r2` | `<tr> = cmpEq(<r1>, <r2>)` | compare (==) |
| `19 tr r1 r2` | `<tr> = cmpNe(<r1>, <r2>)` | compare (!=) |

## Array

| bytes         | instruction                 | description                 |
|---------------|-----------------------------|-----------------------------|
| `1a r1 -- --` | `Array.assert(<r1>)`        | assert type Array           |
| `1b tr pp pp` | `<tr> = Array.init(pppp)`   | initialize array, u16 cap   |
| `1c tr r1 --` | `<tr>.push(<r1>)`           | add element to end of array |
| `1d tr r1 r2` | `<tr> = <r1>[<r2>]`         | get element [error]         |
| `1e tr r1 r2` | `<tr>[<r1>] = <r2>`         | set element [error]         |
| `1f tr r1 --` | `<tr> = Array.length(<r1>)` | get number of elements      |
| `20 tr r1 r2` | `<tr> = Array.concat(<r1>, <r2>)` | concatenate arrays    |

## Func

| bytes         | instruction              | description                    |
|---------------|--------------------------|--------------------------------|
| `21 tr cc cc` | `<tr> = Func.init(cccc)` | initialize Func                |
| `22 tr r1 r2` | `<tr> = <r1>(...<r2>)`   | call Func with arguments Array |
| `23 -- r1 r2` | `tailcall <r1>(...<r2>)` | tailcall                       |

## Macro

| bytes         | instruction               | description      |
|---------------|---------------------------|------------------|
| `24 tr cc cc` | `<tr> = Macro.init(cccc)` | initialize Macro |

## Environment

| bytes         | instruction               | description                    |
|---------------|---------------------------|--------------------------------|
| `25 tr ee oo` | `<tr> = load(<ee>, <oo>)` | load from environment [error]  |
| `26 r1 ee oo` | `store(<r1>, <ee>, <oo>)` | store into environment [error] |

## SyntaxNode

| bytes         | instruction                           | description        |
|---------------|---------------------------------------|--------------------|
| `27 tr r1 --` | `<tr> = SyntaxNode.init(...<r1>)`     | assign SyntaxNode  |
| `28 tr r1 --` | `<tr> = SyntaxNode.IntNode(...<r1>)`  | ...IntNode (Int)   |
| `29 tr r1 --` | `<tr> = SyntaxNode.StrNode(...<r1>)`  | ...StrNode (Str)   |
| `2a tr r1 --` | `<tr> = SyntaxNode.BoolNode(...<r1>)` | ...BoolNode (Bool) |

## Control

| bytes         | instruction          | description           |
|---------------|----------------------|-----------------------|
| `80 -- aa aa` | `jmp <aaaa>`         | unconditional jump    |
| `81 r1 aa aa` | `jmp <aaaa> if <r1>` | conditional jump      |
| `82 r1 -- --` | `ret <r1>`           | return                |
| `83 r1 -- --` | `Proc.exit(<r1>)`    | exit with status code |

## Input/output

| bytes         | instruction           | description            |
|---------------|-----------------------|------------------------|
| `84 r1 -- --` | `print(<r1>)`         | print string + newline |
| `85 tr r1 --` | `<tr> = prompt(<r1>)` | prompt and input       |

