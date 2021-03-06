;; Entry point for standard REPL environment
(begin
	;; Define some standard library functions
	(define import (macro (Path) (_eval (_tokenize (readfile (+ "lib/" Path ".lisp"))))))
	(define foldl (# (Lst Acc Cb)
		(if (= 0 (length Lst)) Acc (foldl (tail Lst) (Cb (head Lst) Acc) Cb))
	))
	(define map (# (Lst Cb) (foldl Lst (list) (# (N Acc) (+ Acc (list (Cb N)))))))
	(define sum (# (Lst) (foldl Lst 0 (# (N Acc) (+ N Acc)))))
	(define prod (# (Lst) (foldl Lst 1 (# (N Acc) (* Acc N)))))

	(define compose (# (F G) (# (X) (F (G X)))))
	(define repeat (# (F) (compose F F)))
	(define abs (# (N) ((if (> N 0) + -) 0 N)))
	(define combine (# (F)
		(# (X Y)
			(if (null? X) (quote ())
				(F (list (head X) (head Y)) ((combine F) (tail X) (tail Y))))
		)
	))
	(define zip (combine cons))
	(define take (# (N Seq) (begin
		(if (<= N 0) (quote ()) (cons (head Seq) (take (- N 1) (tail Seq))))
	)))
	(define drop (# (N Seq) (begin
		(if (<= N 0) Seq (drop (- N 1) (tail Seq)))
	)))
	(define mid (# (Seq) (/ (length Seq) 2)))
	(define riff-shuffle (# (Deck) (begin
		((combine (get! append)) (take (mid Deck) Deck) (drop (mid Deck) Deck))
	)))

	;; Runtime functions

	;; Lookup a function or return a stub
	(define lookup (# (Symbol)
		;; Use eval to get Symbol's value at runtime
		(if (_eval (list defined Symbol))  ;; If symbol present
			(_eval (list get! Symbol))     ;; get symbol
		    (# () stub))                   ;; else return a stub
	))

	;; A macro that checks if Name is defined, and returns its value
	;; or the atom none.
	(define ifDefined (macro (Name)
		;; Construct a manual AST of: (if (defined Name) (get! Name) none)
		(_eval (list if (list defined Name) (list get! Name) none))))

	;; Reverse of expl
	(define unexpl (# (List) (foldl List "" (# (Char Acc) (+ Acc Char)))))

	;; Play functions
		;; The factorial function
		(define fac (# (N) (begin
			;; This should tail recurse
			(fac2 N 1)
		)))
		;; This is the tail recursive part
		(define fac2 (# (N A) (begin
			(if (<= N 0) A (fac2 (- N 1) (* N A)))
		)))

		;; A tuple
		(define tuple (# (A B) (begin
			(list (ifDefined A) (ifDefined B))
		)))
		(define dict (# (Members) Members))
	
		(define with (# (A Cb) (# () (Cb A))))
		(define triple (# (A) ((with A (# (X) (list X X X))))))

	(print (banner))

	;; Invoke the repl
	(repl)
)
