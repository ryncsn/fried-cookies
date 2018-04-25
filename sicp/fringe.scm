#lang planet neil/sicp
(define x (list (list 1 2) (list 3 4)))
(define (append lh rh)
  (if (null? lh)
      rh
      (cons (car lh) (append (cdr lh) rh))))
(define (fringe x)
  (if (null? x)
      x
      (if (pair? (car x))
          (append (fringe (car x)) (fringe (cdr x)))
          (cons (car x) (fringe (cdr x))))))
          