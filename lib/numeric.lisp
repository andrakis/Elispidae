(begin
	(define __module__ (macro () numeric))
	(define % (# (A B) (- A (* B (/ A B)))))
	(define abs (# (N) ((if (> N 0) + -) 0 N)))

	;; These rely on the lists module
	(require "lists")
	(define sum (# (Lst) (foldl Lst 0 (# (N Acc) (+ N Acc)))))
	(define prod (# (Lst) (foldl Lst 1 (# (N Acc) (* Acc N)))))

	;; The factorial function
	(define fac (# (N) (begin
		;; This should tail recurse
		(fac2 N 1)
	)))
	;; This is the tail recursive part
	(define fac2 (# (N A) (begin
		(if (<= N 0) A (fac2 (- N 1) (* N A)))
	)))

	(export % abs sum prod fac)
)
