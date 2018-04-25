#lang planet neil/sicp
(define (square a)
  (* a a))

(define (expmod base exp mod)
  (cond ((= exp 0) 1)
        ((even? exp)
         (remainder (square (expmod base (/ exp 2) mod)) mod))
        (else 
         (remainder (* base (expmod base (- exp 1) mod)) mod))))

(define (fermat-test n)
  (define (try a)
    (= (expmod a n n) a))
  (try (- (random (+ n 1)) 1)))

(define (fast-prime n count)
  (cond ((= 0 count) true)
        ((fermat-test n) (fast-prime n (- count 1)))
        (else false)))

(define (prime n)
  (fast-prime n 128))



(define (timed-prime-test n)
  (newline)
  (display n)
  (start-prime-test n (runtime)))
(define (start-prime-test n start-time)
  (if (prime n)
      (report-prime (- (runtime) start-time))))
(define (report-prime elapsed-time)
  (display "***")
  (display elapsed-time))

(define (search-for-prime range)
  (if (> range 0)
      ((timed-prime-test range)
       (search-for-prime (- range 1)))
      (display "end")))