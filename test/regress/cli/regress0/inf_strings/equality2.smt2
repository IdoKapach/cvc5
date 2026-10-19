; EXPECT: unsat
(set-logic ALL)

(declare-const x InfString)
(declare-const y InfString)

(assert (= x (inf_str.const "aa" "ab")))
(assert (= y (inf_str.const "aa" "bb")))
(assert (= x y))
(check-sat)