;; Tail recursive factorial
(begin
	;; Optional: enable debugging
	(debug false)
	(define fac (# (N) (begin
		;; This should tail recurse
		(fac2 N 1)
	)))
	;; This is the tail recursive part
	(define fac2 (# (N A) (begin
		(if (= N 0) A (fac2 (- N 1) (* N A)))
	)))
	(define X 5)
	(print "Factorial of" X ":" (fac X))
	(print "Reductions:" (reds) " Max depth:" (_depth_max))

	(define foldl (# (Lst Acc Cb) (begin
		(if (= nil Lst) Acc (foldl (tail Lst) (Cb (head Lst) Acc) Cb))
	)))
	(define map (# (Lst Cb) (foldl Lst (list) (# (N Acc) (+ Acc (list (Cb N)))))))
	(define sum (# (Lst) (foldl Lst 0 (# (N Acc) (+ N Acc)))))
	(define prod (# (Lst) (foldl Lst 1 (# (N Acc) (* Acc N)))))

	(print "2 3 4: " (map (list 1 2 3) (# (N) (+ N 1))))
	(print "sum: " (sum (list 1 2 3)))
	(print "prod: " (prod (list 1 2 3)))
	(print "Reductions:" (reds) " Max depth:" (_depth_max))

	(define add (# (X Y)
		(if (defined Y) (+ X Y)
			(if (defined X) (# (Y) (+ X Y))
				(# (X Y) (+ X Y))
			)
		)
	))
	(debug true)
	(print "Add:" (add 1 2))
	(print "Add:" ((add 1) 2))
	(print "Add:" ((add) 1 2))
)
