; COMMAND-LINE: --produce-models
; DISABLE-TESTER: model
; EXPECT: sat
; EXPECT: (
; EXPECT: ; cardinality of S is 3
; EXPECT: ; rep: (as @S_0 S)
; EXPECT: ; rep: (as @S_1 S)
; EXPECT: ; rep: (as @S_2 S)
; EXPECT: (define-fun x () S (as @S_0 S))
; EXPECT: (define-fun y () S (as @S_1 S))
; EXPECT: (define-fun z () S (as @S_2 S))
; EXPECT: (define-fun w () Int 0)
; EXPECT: (define-fun t () Int 1)
; EXPECT: (define-fun a () Bool true)
; EXPECT: (define-fun b () Bool true)
; EXPECT: (define-fun c () Bool false)
; EXPECT: )
; EXIT: 0

(set-logic ALL)
(set-info :status sat)
(declare-sort S 0)
(declare-const x S)
(declare-const y S)
(declare-const z S)
(declare-const w Int)
(declare-const t Int)
(declare-const a Bool)
(declare-const b Bool)
(declare-const c Bool)
(assert (gor.< x y))
(assert (gor.< y z))
(assert (gor.< x z))
(assert (gor.< w t))
(assert (gor.< a c))
(assert (gor.< b c))
(check-sat)
(get-model)
