; EXPECT: sat
(set-logic ALL)

(declare-const x InfString)
(declare-const i Int)
(declare-const j Int)
(declare-const y InfString)
(declare-const z String)

(assert (= (inf_str.substr x i j) z))
(assert (= (inf_str.update x i z) y))
(assert (= (inf_str.charat x i) (str.charat z 0)))
(assert (= (inf_str.indexof y z i) i))
(assert (= x y))

(check-sat)