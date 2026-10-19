; EXPECT: unsat
(set-logic ALL)

(declare-const x InfString)

(assert (= x (inf_str.const "" "ab")))
(assert (= x x))

(check-sat)