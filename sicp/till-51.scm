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

(define (double f)
  (lambda (x) (f (f x))))

(define (compose f g)
  (lambda (x) (f (g x))))

(define (repeated f times)
  (if (= 1 times)
      (lambda (x) (f x))
      (lambda (x) (f ((repeated f (- times 1)) x)))))

(define (smooth f)
  (lambda (x) (/ (+ (f x) (f (+ x dx)) (f (- x dx))) 3)))

(define (average-damp f)
  (lambda (x) (/ (+ x (f x)) 2)))

(define (mults y n)
  (if (= n 0)
      1
      (* y (mults y (- n 1)))))