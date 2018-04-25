#lang planet neil/sicp
(define (foreach do items)
  (if (null? items)
      (newline)
      ((do (car items))
       (foreach do (cdr items)))))