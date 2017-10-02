;; Tail recursive factorial
(begin
	;; Optional: enable debugging
	(debug true)
	(define fac (# (N) (begin
		;; This should tail recurse
		(fac2 N 1)
	)))
	;; This is the tail recursive part
	(define fac2 (# (N A) (begin
		(if (= N 0) A (fac2 (- N 1) (* N A)))
	)))
	(define X 15)
	(print "Factorial of" X ":" (fac X))
	(print "Reductions:" (reds) " Max depth:" (_depth_max))
)
