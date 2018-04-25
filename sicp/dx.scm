#lang racket
(define dx 0.001)
(define (close? a b)
  (< (- b a) 0.003))
(define (calc f start end)
  (if (close? start end)
      0
      (+ (* (f start) dx) (calc f (+ dx start) end))))
(define (sx x)
  (/ 1000 (* x x)))
(define (tex x)
  (* 12 x (* ( - 1 x) ( - 1 x))))
