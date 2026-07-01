import Mathlib

def Offset := Nat
  deriving Repr, BEq, DecidableEq, Hashable, Inhabited, Ord

instance {n : Nat} : OfNat Offset n where
  ofNat := n

instance : HAdd Offset Nat Offset where hAdd := Nat.add

instance : LT Offset where
  lt := Nat.lt

def RegIdx := UInt8
  deriving Repr, BEq, DecidableEq, Inhabited, Ord

def TableIdx := UInt16
  deriving Repr, BEq, DecidableEq, Inhabited, Ord

def Addr := UInt16
  deriving Repr, BEq, DecidableEq, Inhabited, Ord

def EnvSteps := UInt8
  deriving Repr, BEq, DecidableEq, Inhabited, Ord

def EnvOffset := UInt8
  deriving Repr, BEq, DecidableEq, Inhabited, Ord

-- Capacity for array_init, fits in u16
def ArrayCap := UInt16
  deriving Repr, BEq, DecidableEq, Inhabited, Ord

inductive Instruction where
  | no_op
  | mov (tr : RegIdx) (r1 : RegIdx)
  | assert_int (r1 : RegIdx)
  | load_int (tr : RegIdx) (ii : TableIdx)
  | int_add (tr : RegIdx) (r1 : RegIdx) (r2 : RegIdx)
  | int_neg (tr : RegIdx) (r1 : RegIdx)
  | int_sub (tr : RegIdx) (r1 : RegIdx) (r2 : RegIdx)
  | int_mul (tr : RegIdx) (r1 : RegIdx) (r2 : RegIdx)
  | int_div (tr : RegIdx) (r1 : RegIdx) (r2 : RegIdx)
  | int_mod (tr : RegIdx) (r1 : RegIdx) (r2 : RegIdx)
  | assert_str (r1 : RegIdx)
  | load_str (tr : RegIdx) (ss : TableIdx)
  | stringify (tr : RegIdx) (r1 : RegIdx)
  | str_concat (tr : RegIdx) (r1 : RegIdx) (r2 : RegIdx)
  | assert_bool (r1 : RegIdx)
  | load_true (tr : RegIdx)
  | load_false (tr : RegIdx)
  | boolify (tr : RegIdx) (r1 : RegIdx)
  | bool_neg (tr : RegIdx) (r1 : RegIdx)
  | assert_none (r1 : RegIdx)
  | load_none (tr : RegIdx)
  | cmp_lt (tr : RegIdx) (r1 : RegIdx) (r2 : RegIdx)
  | cmp_le (tr : RegIdx) (r1 : RegIdx) (r2 : RegIdx)
  | cmp_gt (tr : RegIdx) (r1 : RegIdx) (r2 : RegIdx)
  | cmp_ge (tr : RegIdx) (r1 : RegIdx) (r2 : RegIdx)
  | cmp_eq (tr : RegIdx) (r1 : RegIdx) (r2 : RegIdx)
  | cmp_ne (tr : RegIdx) (r1 : RegIdx) (r2 : RegIdx)
  | assert_array (r1 : RegIdx)
  | array_init (tr : RegIdx) (pp : ArrayCap)
  | array_push (r1 : RegIdx) (r2 : RegIdx)
  | array_get (tr : RegIdx) (r1 : RegIdx) (r2 : RegIdx)
  | array_set (r1 : RegIdx) (r2 : RegIdx) (r3 : RegIdx)
  | array_len (tr : RegIdx) (r1 : RegIdx)
  | array_concat (tr : RegIdx) (r1 : RegIdx) (r2 : RegIdx)
  | assert_func (r1 : RegIdx)
  | func_init (tr : RegIdx) (cc : TableIdx)
  | func_call (tr : RegIdx) (r1 : RegIdx) (r2 : RegIdx)
  | func_tailcall (r1 : RegIdx) (r2 : RegIdx)
  | assert_macro (r1 : RegIdx)
  | macro_init (tr : RegIdx) (cc : TableIdx)
  | env_load (tr : RegIdx) (ee : EnvSteps) (oo : EnvOffset)
  | env_store (r1 : RegIdx) (ee : EnvSteps) (oo : EnvOffset)
  | syntax_node_create (tr : RegIdx) (r1 : RegIdx)
  | int_node_create (tr : RegIdx) (r1 : RegIdx)
  | str_node_create (tr : RegIdx) (r1 : RegIdx)
  | bool_node_create (tr : RegIdx) (r1 : RegIdx)
  | print (r1 : RegIdx)
  | input (tr : RegIdx) (r1 : RegIdx)
  | jump (aa : Addr)
  | jump_if_true (r1 : RegIdx) (aa : Addr)
  | return (r1 : RegIdx)
  | proc_exit (r1 : RegIdx)
  deriving Repr, BEq, DecidableEq, Inhabited

inductive SyntaxNodePayload where
  | int (n : Offset)
  | str (s : Offset)
  | bool (b : Offset)

inductive Value where
  | I64Value (payload : Int64)
  | AsciiStrValue (length : UInt32) (payload : List UInt8)
  | BoolValue (payload : Bool)
  | NoneValue
  | IntValue (sign : Bool) (nlimbs : UInt32) (limbs : List UInt32)
  | StrValue (length_bytes : UInt32) (length_codepoints : UInt32)
      (payload : List UInt8)
  | SmallStrValue (payload : List UInt8)
  | ArrayValue (length : UInt32) (elements : Offset)
  | FuncValue (env : Offset) (codeunit : Offset)
  | MacroValue (env : Offset) (codeunit : Offset)
  | SyntaxNodeValue (kind : Offset) (payload : Option SyntaxNodePayload)
      (child_count : UInt32) (children : List Offset)
  | ArrayElements (capacity : UInt32) (elements : List (Option Offset))
  | Environment (outer_env : Option Offset) (entry_count : UInt32)
      (entries : List (Bool × Option Offset))
  | CodeUnit (name : Option Offset) (outer_codeunit : Option Offset)
      (parameters : Offset) (register_count : UInt32) (env_length : UInt32)
      (inttable : Option Offset) (strtable : Option Offset)
      (codetable : Option Offset) (instr_count : UInt32)
      (instructions : List Instruction)
  | Parameters (param_count : UInt32) (entries : List Offset)
  | IntTable (length : UInt32) (entries : List Offset)
  | StrTable (length : UInt32) (entries : List Offset)
  | CodeTable (length : UInt32) (entries : List Offset)

structure Arena where
  next : Offset
  resolve : Offset → Option Value

namespace Arena

def empty : Arena := ⟨0, fun _ => none⟩

def alloc (a : Arena) (v : Value) : Arena × Offset :=
  let o := a.next
  (⟨o + 1, fun o' => if o' = o then some v else a.resolve o'⟩, o)

def update (a : Arena) (o : Offset) (v : Value) : Arena :=
  ⟨a.next, fun o' => if o' = o then some v else a.resolve o'⟩

end Arena

def ArenaWellFormed (a : Arena) : Prop :=
  ∀ o, a.resolve o ≠ none → o < a.next

theorem empty_wellFormed : ArenaWellFormed Arena.empty := by
  intro o h
  simp [Arena.empty] at h

theorem alloc_preserves_wellFormed {a : Arena} {v : Value}
  (h_wf : ArenaWellFormed a) :
  ArenaWellFormed (a.alloc v).1 := by
  intro o h
  simp [Arena.alloc] at h ⊢
  by_cases h_eq : o = a.next
  . rw [h_eq]
    exact Nat.lt_succ_self a.next
  . simp [h_eq] at h
    have h_o : o < a.next := h_wf o h
    exact Nat.lt_trans h_o (Nat.lt_succ_self a.next)

theorem update_preserves_wellFormed {a : Arena} {o : Offset} {v : Value}
  (h_wf : ArenaWellFormed a) (h_o : o < a.next) :
  ArenaWellFormed (a.update o v) := by
  intro o' h
  simp [Arena.update] at h ⊢
  by_cases h_eq : o' = o
  . rw [h_eq]
    exact h_o
  . simp [h_eq] at h
    exact h_wf o' h

inductive ArrayElemIsSome (elems : List (Option Offset)) : Fin elems.length → Prop
  | mk (i : Fin elems.length) (o : Offset) (h : List.get elems i = some o) : ArrayElemIsSome elems i

mutual

/-- `ValidValue arena seen offset` means that the value stored at `offset` is
structurally valid, assuming all offsets in `seen` have already been validated.

This is the Lean analogue of the C `*_validate` functions that take a
`SeenSet*` to detect cycles. The `seen` list grows as we traverse pointers,
ensuring that cyclic structures (e.g. a function closing over an environment
that refers back to the function) do not cause infinite regress.

An important side invariant (tracked via `h_seen_valid` in every constructor)
is that every offset recorded in `seen` is actually allocated in the arena.
This lets us prove that every validated offset is below `next`. -/

inductive ValidValue (arena : Arena) (h_wf : ArenaWellFormed arena) : List Offset → Offset → Prop
  | alreadySeen (seen : List Offset) (offset : Offset)
      (h_mem : offset ∈ seen)
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | I64Value (seen : List Offset) (offset : Offset) (payload : Int64)
      (h_cell : arena.resolve offset = some (Value.I64Value payload))
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | AsciiStrValue (seen : List Offset) (offset : Offset) (length: UInt32) (payload : List UInt8)
      (h_cell : arena.resolve offset = some (Value.AsciiStrValue length payload))
      (h_length : length.toNat = payload.length)
      (h_ascii : ∀ b ∈ payload, b < 128)
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | BoolValue (seen : List Offset) (offset : Offset) (payload : Bool)
      (h_cell : arena.resolve offset = some (Value.BoolValue payload))
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | NoneValue (seen : List Offset) (offset : Offset)
      (h_cell : arena.resolve offset = some Value.NoneValue)
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | IntValue (seen : List Offset) (offset : Offset) (sign : Bool)
      (nlimbs : UInt32) (limbs : List UInt32)
      (h_cell : arena.resolve offset = some (Value.IntValue sign nlimbs limbs))
      (h_nlimbs : nlimbs.toNat = limbs.length)
      (h_last_limb_nonzero : ∀ n, limbs.getLast? = some n → n ≠ 0)
      (h_zero_sign : limbs = [] → sign = false)
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | StrValue (seen : List Offset) (offset : Offset) (length_bytes : UInt32)
      (length_codepoints : UInt32) (payload : List UInt8)
      (h_cell : arena.resolve offset = some (Value.StrValue length_bytes length_codepoints payload))
      (h_length_bytes : length_bytes.toNat = payload.length)
      (h_utf8 : ∃ s : String, s.toUTF8.data.toList = payload ∧ s.length = length_codepoints.toNat)
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | SmallStrValue (seen : List Offset) (offset : Offset) (payload : List UInt8)
      (h_cell : arena.resolve offset = some (Value.SmallStrValue payload))
      (h_length : payload.length = 8)
      (h_utf8 : String.fromUTF8? ⟨payload.toArray⟩ ≠ none)
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | ArrayValue (seen : List Offset) (offset : Offset) (length : UInt32) (elements : Offset)
      (capacity : UInt32) (elems : List (Option Offset))
      (h_cell : arena.resolve offset = some (Value.ArrayValue length elements))
      (h_elements_cell : arena.resolve elements = some (Value.ArrayElements capacity elems))
      (h_length_le_capacity : length.toNat ≤ capacity.toNat)
      (h_set : ∀ (i : Fin elems.length), i.val < length.toNat → ArrayElemIsSome elems i)
      (h_unset : ∀ (i : Fin elems.length), length.toNat ≤ i.val → List.get elems i = none)
      (h_elements_valid : ValidValue arena h_wf (offset :: seen) elements)
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | ArrayElements (seen : List Offset) (offset : Offset) (capacity : UInt32)
      (elems : List (Option Offset))
      (h_cell : arena.resolve offset = some (Value.ArrayElements capacity elems))
      (h_capacity : capacity.toNat = elems.length)
      (h_elements_valid : ∀ (i : Fin elems.length), ∀ o, List.get elems i = some o → ValidValue arena h_wf (offset :: seen) o)
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | SyntaxNodeValue (seen : List Offset) (offset : Offset) (kind : Offset) (payload : Option SyntaxNodePayload)
      (child_count : UInt32) (children : List Offset)
      (h_cell : arena.resolve offset = some (Value.SyntaxNodeValue kind payload child_count children))
      (h_kind_valid : ValidValue arena h_wf (offset :: seen) kind)
      (h_payload_valid : ValidOptionSyntaxNodePayload arena h_wf (offset :: seen) payload)
      (h_child_count : child_count.toNat = children.length)
      (h_children_valid : ∀ (i : Fin children.length), ValidValue arena h_wf (offset :: seen) (children.get i))
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | FuncValue (seen : List Offset) (offset : Offset) (env : Offset) (codeunit : Offset)
      (h_cell : arena.resolve offset = some (Value.FuncValue env codeunit))
      (env_outer_env : Option Offset) (entry_count : UInt32) (env_entries : List (Bool × Option Offset))
      (h_env_cell : arena.resolve env = some (Value.Environment env_outer_env entry_count env_entries))
      (cu_name : Option Offset) (cu_outer_codeunit : Option Offset) (cu_parameters : Offset)
      (cu_register_count : UInt32) (env_length : UInt32)
      (cu_inttable : Option Offset) (cu_strtable : Option Offset) (cu_codetable : Option Offset)
      (cu_instr_count : UInt32) (cu_instructions : List Instruction)
      (h_codeunit_cell : arena.resolve codeunit = some (Value.CodeUnit cu_name cu_outer_codeunit cu_parameters cu_register_count env_length cu_inttable cu_strtable cu_codetable cu_instr_count cu_instructions))
      (h_env_length : entry_count = env_length)
      (h_env_valid : ValidValue arena h_wf (offset :: seen) env)
      (h_codeunit_valid : ValidValue arena h_wf (offset :: seen) codeunit)
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | MacroValue (seen : List Offset) (offset : Offset) (env : Offset) (codeunit : Offset)
      (h_cell : arena.resolve offset = some (Value.MacroValue env codeunit))
      (env_outer_env : Option Offset) (entry_count : UInt32) (env_entries : List (Bool × Option Offset))
      (h_env_cell : arena.resolve env = some (Value.Environment env_outer_env entry_count env_entries))
      (cu_name : Option Offset) (cu_outer_codeunit : Option Offset) (cu_parameters : Offset)
      (cu_register_count : UInt32) (env_length : UInt32)
      (cu_inttable : Option Offset) (cu_strtable : Option Offset) (cu_codetable : Option Offset)
      (cu_instr_count : UInt32) (cu_instructions : List Instruction)
      (h_codeunit_cell : arena.resolve codeunit = some (Value.CodeUnit cu_name cu_outer_codeunit cu_parameters cu_register_count env_length cu_inttable cu_strtable cu_codetable cu_instr_count cu_instructions))
      (h_env_length : entry_count = env_length)
      (h_env_valid : ValidValue arena h_wf (offset :: seen) env)
      (h_codeunit_valid : ValidValue arena h_wf (offset :: seen) codeunit)
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | Environment (seen : List Offset) (offset : Offset) (outer_env : Option Offset) (entry_count : UInt32)
      (entries : List (Bool × Option Offset))
      (h_cell : arena.resolve offset = some (Value.Environment outer_env entry_count entries))
      (h_entry_count : entry_count.toNat = entries.length)
      (h_outer_env_valid : ValidOptionEnvironmentValue arena h_wf (offset :: seen) outer_env)
      (h_entries_valid : ∀ (i : Fin entries.length), ValidOptionValue arena h_wf (offset :: seen) (entries.get i).2)
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | CodeUnit (seen : List Offset) (offset : Offset) (name : Option Offset) (outer_codeunit : Option Offset)
      (parameters : Offset) (register_count : UInt32) (env_length : UInt32)
      (inttable : Option Offset) (strtable : Option Offset) (codetable : Option Offset)
      (instr_count : UInt32) (instructions : List Instruction)
      (h_cell : arena.resolve offset = some (Value.CodeUnit name outer_codeunit parameters register_count env_length inttable strtable codetable instr_count instructions))
      (h_name_valid : ValidOptionValue arena h_wf (offset :: seen) name)
      (h_outer_valid : ValidOptionValue arena h_wf (offset :: seen) outer_codeunit)
      (params_param_count : UInt32) (params_entries : List Offset)
      (h_params_cell : arena.resolve parameters = some (Value.Parameters params_param_count params_entries))
      (h_register_count_ge_param_count : register_count.toNat ≥ params_param_count.toNat)
      (h_params_valid : ValidValue arena h_wf (offset :: seen) parameters)
      (h_inttable_valid : ValidOptionValue arena h_wf (offset :: seen) inttable)
      (h_strtable_valid : ValidOptionValue arena h_wf (offset :: seen) strtable)
      (h_codetable_valid : ValidOptionValue arena h_wf (offset :: seen) codetable)
      (h_instr_count : instr_count.toNat = instructions.length)
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | Parameters (seen : List Offset) (offset : Offset) (param_count : UInt32) (entries : List Offset)
      (h_cell : arena.resolve offset = some (Value.Parameters param_count entries))
      (h_param_count : param_count.toNat = entries.length)
      (h_entries_valid : ∀ (i : Fin entries.length), ValidStringValue arena h_wf (offset :: seen) (entries.get i))
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | IntTable (seen : List Offset) (offset : Offset) (length : UInt32) (entries : List Offset)
      (h_cell : arena.resolve offset = some (Value.IntTable length entries))
      (h_length : length.toNat = entries.length)
      (h_entries_valid : ∀ (i : Fin entries.length), ValidIntValue arena h_wf (offset :: seen) (entries.get i))
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | StrTable (seen : List Offset) (offset : Offset) (length : UInt32) (entries : List Offset)
      (h_cell : arena.resolve offset = some (Value.StrTable length entries))
      (h_length : length.toNat = entries.length)
      (h_entries_valid : ∀ (i : Fin entries.length), ValidStringValue arena h_wf (offset :: seen) (entries.get i))
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

  | CodeTable (seen : List Offset) (offset : Offset) (length : UInt32) (entries : List Offset)
      (h_cell : arena.resolve offset = some (Value.CodeTable length entries))
      (h_length : length.toNat = entries.length)
      (h_entries_valid : ∀ (i : Fin entries.length), ValidCodeUnitValue arena h_wf (offset :: seen) (entries.get i))
      (h_seen_valid : ∀ o ∈ seen, o < arena.next ∧ arena.resolve o ≠ none) :
      ValidValue arena h_wf seen offset

inductive ValidSyntaxNodePayload (arena : Arena) (h_wf : ArenaWellFormed arena) : List Offset → SyntaxNodePayload → Prop
  | int_i64 (seen : List Offset) (n : Offset) (payload : Int64)
      (h_cell : arena.resolve n = some (Value.I64Value payload))
      (h_valid : ValidValue arena h_wf seen n) :
      ValidSyntaxNodePayload arena h_wf seen (SyntaxNodePayload.int n)

  | int_big (seen : List Offset) (n : Offset) (sign : Bool) (nlimbs : UInt32) (limbs : List UInt32)
      (h_cell : arena.resolve n = some (Value.IntValue sign nlimbs limbs))
      (h_valid : ValidValue arena h_wf seen n) :
      ValidSyntaxNodePayload arena h_wf seen (SyntaxNodePayload.int n)

  | str_ascii (seen : List Offset) (s : Offset) (l : UInt32) (payload : List UInt8)
      (h_cell : arena.resolve s = some (Value.AsciiStrValue l payload))
      (h_valid : ValidValue arena h_wf seen s) :
      ValidSyntaxNodePayload arena h_wf seen (SyntaxNodePayload.str s)

  | str_full (seen : List Offset) (s : Offset) (lb : UInt32) (lc : UInt32) (payload : List UInt8)
      (h_cell : arena.resolve s = some (Value.StrValue lb lc payload))
      (h_valid : ValidValue arena h_wf seen s) :
      ValidSyntaxNodePayload arena h_wf seen (SyntaxNodePayload.str s)

  | str_small (seen : List Offset) (s : Offset) (payload : List UInt8)
      (h_cell : arena.resolve s = some (Value.SmallStrValue payload))
      (h_valid : ValidValue arena h_wf seen s) :
      ValidSyntaxNodePayload arena h_wf seen (SyntaxNodePayload.str s)

  | bool (seen : List Offset) (b : Offset) (payload : Bool)
      (h_cell : arena.resolve b = some (Value.BoolValue payload))
      (h_valid : ValidValue arena h_wf seen b) :
      ValidSyntaxNodePayload arena h_wf seen (SyntaxNodePayload.bool b)

inductive ValidOptionSyntaxNodePayload (arena : Arena) (h_wf : ArenaWellFormed arena) : List Offset → Option SyntaxNodePayload → Prop
  | none (seen : List Offset) : ValidOptionSyntaxNodePayload arena h_wf seen none
  | some (seen : List Offset) (p : SyntaxNodePayload) (h : ValidSyntaxNodePayload arena h_wf seen p) : ValidOptionSyntaxNodePayload arena h_wf seen (some p)
inductive ValidIntValue (arena : Arena) (h_wf : ArenaWellFormed arena) : List Offset → Offset → Prop
  | i64 (seen : List Offset) (o : Offset) (payload : Int64)
      (h_cell : arena.resolve o = some (Value.I64Value payload))
      (h_valid : ValidValue arena h_wf seen o) :
      ValidIntValue arena h_wf seen o
  | int (seen : List Offset) (o : Offset) (sign : Bool) (nlimbs : UInt32) (limbs : List UInt32)
      (h_cell : arena.resolve o = some (Value.IntValue sign nlimbs limbs))
      (h_valid : ValidValue arena h_wf seen o) :
      ValidIntValue arena h_wf seen o

inductive ValidStringValue (arena : Arena) (h_wf : ArenaWellFormed arena) : List Offset → Offset → Prop
  | ascii (seen : List Offset) (o : Offset) (l : UInt32) (payload : List UInt8)
      (h_cell : arena.resolve o = some (Value.AsciiStrValue l payload))
      (h_valid : ValidValue arena h_wf seen o) :
      ValidStringValue arena h_wf seen o
  | full (seen : List Offset) (o : Offset) (lb : UInt32) (lc : UInt32) (payload : List UInt8)
      (h_cell : arena.resolve o = some (Value.StrValue lb lc payload))
      (h_valid : ValidValue arena h_wf seen o) :
      ValidStringValue arena h_wf seen o
  | small (seen : List Offset) (o : Offset) (payload : List UInt8)
      (h_cell : arena.resolve o = some (Value.SmallStrValue payload))
      (h_valid : ValidValue arena h_wf seen o) :
      ValidStringValue arena h_wf seen o

inductive ValidCodeUnitValue (arena : Arena) (h_wf : ArenaWellFormed arena) : List Offset → Offset → Prop
  | codeunit (seen : List Offset) (o : Offset) (name : Option Offset) (outer_codeunit : Option Offset) (parameters : Offset)
      (register_count : UInt32) (env_length : UInt32)
      (inttable : Option Offset) (strtable : Option Offset) (codetable : Option Offset)
      (instr_count : UInt32) (instructions : List Instruction)
      (h_cell : arena.resolve o = some (Value.CodeUnit name outer_codeunit parameters register_count env_length inttable strtable codetable instr_count instructions))
      (h_valid : ValidValue arena h_wf seen o) :
      ValidCodeUnitValue arena h_wf seen o

inductive ValidEnvironmentValue (arena : Arena) (h_wf : ArenaWellFormed arena) : List Offset → Offset → Prop
  | env (seen : List Offset) (o : Offset) (outer_env : Option Offset) (entry_count : UInt32) (entries : List (Bool × Option Offset))
      (h_cell : arena.resolve o = some (Value.Environment outer_env entry_count entries))
      (h_valid : ValidValue arena h_wf seen o) :
      ValidEnvironmentValue arena h_wf seen o

inductive ValidOptionEnvironmentValue (arena : Arena) (h_wf : ArenaWellFormed arena) : List Offset → Option Offset → Prop
  | none (seen : List Offset) : ValidOptionEnvironmentValue arena h_wf seen none
  | some (seen : List Offset) (o : Offset) (h : ValidEnvironmentValue arena h_wf seen o) : ValidOptionEnvironmentValue arena h_wf seen (some o)

inductive ValidOptionValue (arena : Arena) (h_wf : ArenaWellFormed arena) : List Offset → Option Offset → Prop
  | none (seen : List Offset) : ValidOptionValue arena h_wf seen none
  | some (seen : List Offset) (o : Offset) (h : ValidValue arena h_wf seen o) : ValidOptionValue arena h_wf seen (some o)

end

