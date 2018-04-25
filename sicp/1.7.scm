#lang ptlanet neil/sicp
(define (good-enough guess next-guess)
  (< (abs (- next-guess guess)) 0.00000000000001))
(define (better-guess guess x)
  (/ (+ guess (/ x guess)) 2.0 ))
(define (sqrt guess x)
  (if (good-enough (better-guess guess x) guess)
    guess
    (sqrt (better-guess guess x) x)))
  