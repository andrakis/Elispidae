;; Entry point for standard REPL environment
(begin
	;; Define some standard library functions
	(define foldl (lambda (Lst Acc Cb)
		(if (= nil Lst) Acc (foldl (tail Lst) (Cb (head Lst) Acc) Cb))
	))
	(define map (lambda (Lst Cb) (foldl Lst (list) (lambda (N Acc) (+ Acc (list (Cb N)))))))
	(define sum (lambda (Lst) (foldl Lst 0 (lambda (N Acc) (+ N Acc)))))
	(define prod (lambda (Lst) (foldl Lst 1 (lambda (N Acc) (* Acc N)))))

	(print (banner))

	;; Invoke the repl
	(repl)
)
