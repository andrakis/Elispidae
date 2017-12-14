;; Entry point for standard REPL environment
(begin
	;; Show a little progress when running on slow systems like OR1K.
	(print "Elispidae starting up...")

	(define DefaultLibraries (list lists numeric string misc cosmos))

	;; (timing true)
	;; (debug true)
	(define __module__ (macro () init))
	(define __init_debug true)
	(define __require_debug (# (Message) (begin
		(if (= true (get! __init_debug)) (print Message))
	)))
	(define import (# (Path) (begin
		(define Content (readfile (+ "lib/" (str Path) ".lisp")))
		(if (> (length Content) 0)
			(begin
				;; Got file contents, can import
				;; Run in topmost context
				(_eval_ctx (_tokenize Content))
			)
			;; else
			(begin
				;; Failed to get contents, throw error
				(invalid_arg (+ "Unable to import module: " (str Path)))
			)
		)
	)))
	(import "require")

	;; Import default libraries
	(each DefaultLibraries (# (Lib) (require Lib)))

	(print (banner))
	(repl)
)
