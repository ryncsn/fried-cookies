#lang planet neil/sicp
(define (tree-map tree f)
  (if (null? tree) nil
      (if (not (pair? tree)) (f tree)
          (cons (tree-map (car tree) f)
                (tree-map (cdr tree) f)))))