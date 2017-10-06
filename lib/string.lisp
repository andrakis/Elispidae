(begin
	(define __module__ (macro () string))
	;; Reverse of expl
	(define unexpl (# (List) (foldl List "" (# (Char Acc) (+ Acc Char)))))
)
