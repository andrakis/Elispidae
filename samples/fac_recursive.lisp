;; Tail recursive factorial
(begin
	;; Optional: enable debugging
	(debug #t)
	(define fac (lambda (n) (begin
		;; This should tail recurse
		(fac2 n 1)
	)))
	;; This is the tail recursive part
	(define fac2 (lambda (n a) (begin
		(if (= n 0) a (fac2 (- n 1) (* n a)))
	)))
	(define x 3)
	(print "Factorial of" x ":" (fac x))
	(print "Reductions:" (reds) " Max depth:" (_depth_max))

	;; (define map (lambda (l cb) (_map l (list) cb)))
	;; (define _map (lambda (l acc cb)
	;; 	(if (= 0 (length l)) acc (_map (tail l) (+ acc (list (cb (head l))))))
	;; ))
)
