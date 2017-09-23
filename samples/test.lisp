;;
(begin
	;; (def fib        #N::((if (< N 2) (1) ((+ (fib (- N 2)) (fib (- N 1)))))))
	(defiNe fib (lambda (N) (if (< N 2)  1   (+ (fib (- N 2)) (fib (- N 1))))))
	(timing true)
	(debug false)
	(print (fib 10) ", reds: " (reds) ", max depth: " (_depth_max))

	(debug false)
	(define foldl (lambda (Lst Acc Cb) (begin
		(print "(foldl " Lst Acc Cb ")")
		(if (= nil Lst) Acc (foldl (tail Lst) (Cb (head Lst) Acc) Cb))
	)))
	(define map (lambda (Lst Cb) (foldl Lst (list) (lambda (N Acc) (+ Acc (list (Cb N)))))))
	(define sum (lambda (Lst) (foldl Lst 0 (lambda (N Acc) (+ N Acc)))))
	(define prod (lambda (Lst) (foldl Lst 1 (lambda (N Acc) (* Acc N)))))
	(print "2 3 4: " (map (list 1 2 3) (lambda (N) (+ N 1))))
	(print "sum: " (sum (list 1 2 3)))
	(print "prod: " (prod (list 1 2 3)))
)
