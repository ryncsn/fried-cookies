#lang planet neil/sicp
(define (mod base div)
  (if (< base div)
      base
      (mod (- base div) div)))
(define (pow base exp)
  (if (= 0 exp)
      1
      (* base (pow base (- exp 1)))))
(define (cons a b)
  (* (pow 2 a)
     (pow 3 b)))
(define (car c)
  (if (not (even? c))
      0
      (+ 1 (car (/ c 2)))))

(define (cdr c)
  (if (not (= (mod c 3) 0))
      0
      (+ 1 (cdr (/ c 3)))))