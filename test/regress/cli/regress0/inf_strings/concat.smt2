; EXPECT: sat
(set-logic ALL)

(declare-const x InfString)
(declare-const y String)
(declare-const z InfString)

(assert (= x (inf_str.const "aa" "b")))
(assert (= y "aa"))
(assert (= z (inf_str.const "aaaa" "b")))
(assert (= (inf_str.concat x y) z))

(check-sat)