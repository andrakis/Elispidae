;;
(begin
	;; (def fib        #N::((if (< N 2) (1) ((+ (fib (- N 2)) (fib (- N 1)))))))
	(define fib (lambda (n) (if (< n 2)  1   (+ (fib (- n 2)) (fib (- n 1))))))
	(timing #t)
	(debug #f)
	(print (fib 10) ", reds: " (reds) ", max depth: " (_depth_max))

	(debug #f)
	(define foldl (lambda (lst acc cb) (begin
		(print "(foldl " lst acc cb ")")
		(if (= nil lst) acc (foldl (tail lst) (cb (head lst) acc) cb))
	)))
	(define map (lambda (lst cb) (foldl lst (list) (lambda (n acc) (+ acc (list (cb n)))))))
	(define sum (lambda (lst) (foldl lst 0 (lambda (n acc) (+ n acc)))))
	(define prod (lambda (lst) (foldl lst 1 (lambda (n acc) (* acc n)))))
	(print "2 3 4: " (map (list 1 2 3) (lambda (n) (+ n 1))))
	(print "sum: " (sum (list 1 2 3)))
	(print "prod: " (prod (list 1 2 3)))
)
