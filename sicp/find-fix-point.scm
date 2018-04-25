#lang planet neil/sicp
(define (cube x) (* x x x))
(define (square x) (* x x))
(define tolerance 0.0000001)
(define (fixpoint f first-guess)
  (define (close-enough? a b)
    (< (abs ( - a b)) tolerance))
  (define (try guess)
    (let ((next (f guess)))
      (if (close-enough? guess next)
          guess
          (try next))))
  (try first-guess))

(define dx tolerance)
(define (deriv g)
  (lambda (x)
    (/ (- (g (+ x dx)) (g x)) dx)))

(define (newton-transform g)
  (lambda (x) (- x (/ (g x) ((deriv g) x)))))

(define (fixpoint-transform trans g guess)
  (fixpoint (trans g) guess))

(define (cubic a b c)
  (lambda (x) 
    (+ (cube x) (* a (square x)) (* b x) c)))
