(begin
	(define __module__ (macro () misc))
	(define compose (# (F G) (# (X) (F (G X)))))
	(define repeat (# (F) (compose F F)))
	;; Lookup a function or return a stub
	(define lookup (# (Symbol)
		;; Use eval to get Symbol's value at runtime
		(if (_eval (list defined Symbol))  ;; If symbol present
			(_eval (list get! Symbol))     ;; get symbol
		    (# () stub))                   ;; else return a stub
	))

	;; A macro that checks if Name is defined, and returns its value
	;; or the atom none.
	(define ifDefined (macro (Name Default) (begin
		;; Construct a manual AST of: (if (defined Name) (get! Name) none)
		(_eval (list if (list defined Name) (list get! Name) (if (defined Default) Default none)))
	)))
	;; A tuple
	(define tuple (# (A B)
		(list (ifDefined A) (ifDefined B))
	))

	;; Print out all environment items using str
	(define envstr (# (Mode Env) (begin
		(define ModeValue (if (defined Mode) Mode false))
		(if (defined Env) (begin
			(if (= list (tag Env))
				(begin
					(each (flatten Env) (# (Ele) (envstr Mode Ele)))
				) ;; else
				(begin
					(each (keys Env) (# (Func) (begin
						(print Func " => " (_eval (list str (list get! Func) ModeValue)))
					)))
				)
			)
		) ;; else
		(begin
			(envstr ModeValue (env))
		))
	)))

	(export compose repeat lookup ifDefined tuple envstr)
)
