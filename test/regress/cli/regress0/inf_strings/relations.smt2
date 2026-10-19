; EXPECT: unsat
(set-logic ALL)

(declare-const x InfString)
(declare-const y InfString)
(declare-const z String)

(assert (= x (inf_str.const "aa" "b")))
(assert (= y (inf_str.const "a" "ab")))
(assert (= z "aaa"))

(assert (or
  (inf_str.contains x z)
  (inf_str.contains x y)
  (inf_str.prefix x z)
  (inf_str.lt x y)
  (inf_str.leq x y)))
(check-sat)