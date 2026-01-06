; COMMAND-LINE: --produce-models
; EXPECT: sat
; EXPECT: (
; EXPECT: (define-fun x () Int (- 1))
; EXPECT: (define-fun y () Int (- 2))
; EXPECT: (define-fun z () Int (- 1))
; EXPECT: (define-fun a () Int 0)
; EXPECT: (define-fun b () Int 0)
; EXPECT: (define-fun w () Int 0)
; EXPECT: (define-fun t () Int 0)
; EXPECT: (define-fun p () Int 0)
; EXPECT: (define-fun q () Int 0)
; EXPECT: )
; EXIT: 0

(set-logic ALL)
(set-info :status sat)
(declare-const x Int)
(declare-const y Int)
(declare-const z Int)
(declare-const a Int)
(declare-const b Int)
(declare-const w Int)
(declare-const t Int)
(declare-const p Int)
(declare-const q Int)

(assert (gor.< x (+ x (* 2 y))))
(assert (gor.< y 0))
(assert (gor.< (* x (* y z)) 0))
(assert (gor.< (* x (* y z)) (+ x (* 2 y))))
(assert (gor.< 0 -1))
(assert (gor.< -1 (- a x)))

(assert (not (gor.< x (- a x))))
(check-sat)
(get-model)
