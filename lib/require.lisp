(begin
	(define __module__ (macro () require))

	;; Lists module is required, so import it now and include it in the list
	(import "lists")
	(define __imports (list lists (__module__)))

	(define require (# (Path) (begin
		(if (! (member Path (get! __imports)))
			(begin
				(import Path)
				(set! __imports (+ (get! __imports) (list Path)))
				(__require_debug (+ "Imported " (str Path)))
				imported
			)
			;; else
			(begin
				(__require_debug (+ "Cached " (str Path)))
				not_imported
			)
		)
	)))
)
