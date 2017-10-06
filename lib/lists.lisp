(begin
	(define __module__ (macro () lists))
	;; Fold left
	(define foldl (# (List Acc Callback)
		(if (= 0 (length List)) Acc (foldl (tail List) (Callback (head List) Acc) Callback))
	))
	(define each (# (List Callback)
		(if (> (length List) 0)
			;; Call callback as we tail recurse
			(each (tail List) Callback (Callback (head List)))
		)
	))
	(define map (# (List Callback) (foldl List (list) (# (N Acc) (+ Acc (list (Callback N)))))))
	;; Filter a list using a predicate. If Predicate returns true, it is kept.
	(define filter (# (List Predicate)
			(foldl List (list)
					(# (Ele Acc)
							(if (Predicate Ele) (+ Acc (list Ele)) Acc)))
	))
	(define member (# (Item List) (begin
		(if (= 0 (length List))
			false
			(begin
				(if (= (head List) Item)
					true
					(member Item (tail List))
				)
			)
		)
	)))
	(define combine (# (Function)
		(# (X Y)
			(if (null? X) (quote ())
				(Function (list (head X) (head Y)) ((combine Function) (tail X) (tail Y))))
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

	(define flatten (# (List) (begin
		(foldl List (list) (# (Element Acc) (begin
			(+ Acc (if (= list (tag Element)) (flatten Element) (list Element)))
		)))
	)))
	(print (flatten (list 1 2 3 (list 4 5 6) 7 8)))
)

