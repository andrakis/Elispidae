;; Tail recursive factorial
(begin
	(define fac (lambda (n) (begin
		;; This should tail recurse
		(fac2 n 1)
	)))
	;; This is the tail recursive part
	(define fac2 (lambda (n a) (begin
		(if (= n 0) a (fac2 (- n 1) (* n a)))
	)))
	(fac 10)
)
