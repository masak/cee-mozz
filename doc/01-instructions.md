Instruction format: `xx yy zz ww`; four bytes (32 bits)

`tr` = target register
`ii` = index in Int table
`ss` = index in Str table
`aa` = address
`--` = unused

* `00 tr r1 --` assign register
* `01 tr ii ii` assign Int constant
* `02 tr ss ss` assign Str constant
* `03 tr -- --` assign true
* `04 tr -- --` assign false
* `05 tr -- --` assign none

* `06 r1 -- --` assert Int
* `07 tr r1 r2` add (Int, Int)
* `08 tr t1 --` negate (Int)
* `09 tr r1 r2` subtract (Int, Int)
* `0a tr r1 r2` multiply (Int, Int)
* `0b tr r1 r2` flooring divide (Int, Int) [error on division by 0]
* `0c tr r2 r2` remainder (Int, Int) [error on division by 0]

* `0d r1 -- --` assert Str
* `0e tr r1 --` stringify (any)
* `0f tr r1 r2` concatenate (any, any)

* `10 r1 -- --` assert Bool
* `11 tr r1 --` boolify (any)
* `12 tr r1 --` boolean negate (any)

* `13 r1 -- --` assert None

* `14 tr r1 r2` compare (<)
* `15 tr r1 r2` compare (<=)
* `16 tr r1 r2` compare (>)
* `17 tr r1 r2` compare (>=)
* `18 tr r1 r2` compare (==)
* `19 tr r1 r2` compare (!=)

* `1a tr r1 --` initialize array from Int size
* `1b tr tr r1` fill in array at index; increment index
* `1c r1 r2 --` index (Array, Int)
* `1d tr r1 r2` assign index (Array, Int)
* `1e r1 -- --` get length of Array

* `1f r1 r2 --` call Func with Array of arguments

* `20 tr ss r1` assign SyntaxNode (Str index, Array)
* `21 tr r1 --` assign IntNode (Int)
* `22 tr r1 --` assign StrNode (Str)
* `23 tr r1 --` assign BoolNode (Bool)

* `80 -- aa aa` unconditional jump
* `81 r1 aa aa` conditional jump

* `82 r1 -- --` return

