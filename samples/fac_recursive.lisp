(begin
	(define fac (lambda (n) (begin
		(define fac2 (lambda (n a) (begin
			(if (= n 0) a (fac2 (- n 1) (* n a)))
		)))
		(fac2 n 1)
	)))
	(fac 10)
)
