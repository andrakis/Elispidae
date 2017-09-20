(begin

	(define eval (lambda (X Env) (begin
		(define T (tag X))
		(if (=  #atom T) (_find X Env)
		(if (!= #list T) X
		(if (= 0 (length X)) sym_nil
		;; else
		(begin
			(define Xl0 (head X))
			(define Xl1 (head (tail X)))
			(if (= quote Xl0) Xl1
			(if (= if Xl0) (begin
				(define Tmp (eval Xl1 Env))
				(define ValTrue (index X 2))
				(define ValFalse (? (< (length X) 4) nil (index X 3)))
				(eval (? (= Tmp false) ValFalse ValTrue) Env)
			)
			(if (= define ))
		))))
	)))

)