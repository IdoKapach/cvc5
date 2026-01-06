; COMMAND-LINE: --produce-models
; EXPECT: sat
; EXPECT: (
; EXPECT: ; cardinality of S is 3
; EXPECT: ; rep: (as @S_0 S)
; EXPECT: ; rep: (as @S_1 S)
; EXPECT: ; rep: (as @S_2 S)
; EXPECT: (define-fun x () S (as @S_0 S))
; EXPECT: (define-fun y () S (as @S_1 S))
; EXPECT: (define-fun z () S (as @S_2 S))
; EXPECT: (define-fun x0 ((BOUND_VARIABLE_523 Int)) Bool (= BOUND_VARIABLE_523 0))
; EXPECT: (define-fun x1 ((BOUND_VARIABLE_530 Int)) Bool false)
; EXPECT: (define-fun a () (Array Int Int) (store ((as const (Array Int Int)) 1) 0 (- 1)))
; EXPECT: (define-fun b () (Array Int Int) ((as const (Array Int Int)) 0))
; EXPECT: )
; EXIT: 0

(set-logic HO_ALL)
(set-info :status sat)
(declare-sort S 0)
(declare-const x S)
(declare-const y S)
(declare-const z S)
(declare-fun x0 (Int) Bool)
(declare-fun x1 (Int) Bool)
(declare-const a (Array Int Int))
(declare-const b (Array Int Int))
(assert (gor.< x y))
(assert (gor.< x z))
(assert (gor.< x0 x1))
(assert (gor.< a b))
(check-sat)
(get-model)
