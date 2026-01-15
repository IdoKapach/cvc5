; COMMAND-LINE: --produce-models
; DISABLE-TESTER: model
; EXPECT: sat
; EXPECT: (
; EXPECT: (define-fun x () Int (- 1))
; EXPECT: (define-fun y () Int 0)
; EXPECT: (define-fun z () Int 1)
; EXPECT: (define-fun q () Int 2)
; EXPECT: (define-fun p () Int 3)
; EXPECT: (define-fun w () Bool true)
; EXPECT: (define-fun t () Bool false)
; EXPECT: )
; EXIT: 0

(set-logic ALL)
(set-info :status sat)
(declare-const x Int)
(declare-const y Int)
(declare-const z Int)
(declare-const q Int)
(declare-const p Int)
(declare-const w Bool)
(declare-const t Bool)
(assert (gor.< p q))
(assert (gor.< x y))
(assert (gor.< y z))
(assert (gor.< w t))
(assert (gor.< p y))
(assert (gor.< y q))
(assert (gor.< q z))
(check-sat)
(get-model)
