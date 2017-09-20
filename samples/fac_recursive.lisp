;; Tail recursive factorial
(begin
	;; Optional: enable debugging
	(debug #t)
	(define fac (lambda (N) (begin
		;; This should tail recurse
		(fac2 N 1)
	)))
	;; This is the tail recursive part
	(define fac2 (lambda (N A) (begin
		(if (= N 0) a (fac2 (- N 1) (* N A)))
	)))
	(define X 3)
	(print "Factorial of" X ":" (fac X))
	(print "Reductions:" (reds) " Max depth:" (_depth_max))

	;; (define map (lambda (L Cb) (_map L (list) Cb)))
	;; (define _map (lambda (L Acc Cb)
	;; 	(if (= 0 (length L)) Acc (_map (tail L) (+ Acc (list (Cb (head L))))))
	;; ))
)
