# Instructions

Instruction format: `xx yy zz ww`; four bytes (32 bits)

| abbr | meaning            |
|------|--------------------|
| `tr` | target register    |
| `ii` | index in Int table |
| `ss` | index in Str table |
| `aa` | address            |
| `--` | unused             |

## Assign

| bytes         | instruction              | description         |
|---------------|--------------------------|---------------------|
| `00 tr r1 --` | `<tr> = <r1>`            | assign register     |
| `01 tr ii ii` | `<tr> = Int_index(iiii)` | assign Int constant |
| `02 tr ss ss` | `<tr> = Str_index(ssss)` | assign Str constant |
| `03 tr -- --` | `<tr> = true`            | assign true         |
| `04 tr -- --` | `<tr> = false`           | assign false        |
| `05 tr -- --` | `<tr> = none`            | assign none         |

## Int

| bytes         | instruction                   | description               |
|---------------|-------------------------------|---------------------------|
| `06 r1 -- --` | `Int.assert(<r1>)`            | assert type Int           |
| `07 tr r1 r2` | `<tr> = Int.add(<r1>, <r2>)`  | add (Int, Int)            |
| `08 tr r1 --` | `<tr> = Int.negate(<r1>)`     | negate (Int)              |
| `09 tr r1 r2` | `<tr> = Int.sub(<r1>, <r2>)`  | subtract (Int, Int)       |
| `0a tr r1 r2` | `<tr> = Int.mul(<r1>, <r2>)`  | multiply (Int, Int)       |
| `0b tr r1 r2` | `<tr> = Int.idiv(<r1>, <r2>)` | divide (Int, Int) [error] |
| `0c tr r2 r2` | `<tr> = Int.mod(<r1>, <r2>)`  | modulo (Int, Int) [error] |

## Str

| bytes         | instruction                     | description            |
|---------------|---------------------------------|------------------------|
| `0d r1 -- --` | `Str.assert(<r1>)`              | assert type Str        |
| `0e tr r1 --` | `<tr> = stringify(<r1>)`        | stringify (any)        |
| `0f tr r1 r2` | `<tr> = Str.concat(<r1>, <r2>)` | concatenate (any, any) |

## Bool

| bytes         | instruction            | description          |
|---------------|------------------------|----------------------|
| `10 r1 -- --` | `Bool.assert(<r1>)`    | assert type Bool     |
| `11 tr r1 --` | `<tr> = boolify(<r1>)` | boolify (any)        |
| `12 tr r1 --` | `<tr> = not(<r1>)`     | boolean negate (any) |

## None

| bytes         | instruction         | description      |
|---------------|---------------------|------------------|
| `13 r1 -- --` | `None.assert(<r1>)` | assert type None |

## Comparison

| bytes         | insturction                | description  |
|---------------|----------------------------|--------------|
| `14 tr r1 r2` | `<tr> = cmpLt(<r1>, <r2>)` | compare (<)  |
| `15 tr r1 r2` | `<tr> = cmpLe(<r1>, <r2>)` | compare (<=) |
| `16 tr r1 r2` | `<tr> = cmpGt(<r1>, <r2>)` | compare (>)  |
| `17 tr r1 r2` | `<tr> = cmpGe(<r1>, <r2>)` | compare (>=) |
| `18 tr r1 r2` | `<tr> = cmpEq(<r1>, <r2>)` | compare (==) |
| `19 tr r1 r2` | `<tr> = cmpNe(<r1>, <r2>)` | compare (!=) |

## Array

| bytes         | instruction               | description                    |
|---------------|---------------------------|--------------------------------|
| `1a r1 -- --` | `Array.assert(<r1>)`      | assert type Array              |
| `1b tr r1 --` | `<tr> = Array.init(<r1>)` | initialize array, Int capacity |
| `1c tr r1 --` | `<tr>.push(<r1>)`         | add element to end of array    |
| `1d tr r1 r2` | `<tr> = <r1>[<r2>]`       | index (Array, Int)             |
| `1e tr r1 r2` | `<tr>[<r1>] = <r2>`       | assign index (Array, Int)      |
| `1f r1 -- --` | `<r1>.length()`           | get length of Array            |

## Func

| bytes         | instruction              | description                    |
|---------------|--------------------------|--------------------------------|
| `20 tr r1 r2` | `<tr> = <r1>(...<r2>)`   | call Func with arguments Array |
| `21 -- r1 r2` | `tailcall <r1>(...<r2>)` | tailcall                       |

## SyntaxNode

| bytes         | instruction                           | description        |
|---------------|---------------------------------------|--------------------|
| `22 tr r1 --` | `<tr> = SyntaxNode.init(...<r1>)`     | assign SyntaxNode  |
| `23 tr r1 --` | `<tr> = SyntaxNode.IntNode(...<r1>)`  | ...IntNode (Int)   |
| `24 tr r1 --` | `<tr> = SyntaxNode.StrNode(...<r1>)`  | ...StrNode (Str)   |
| `25 tr r1 --` | `<tr> = SyntaxNode.BoolNode(...<r1>)` | ...BoolNode (Bool) |

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

