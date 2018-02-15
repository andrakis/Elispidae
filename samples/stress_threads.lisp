;; This is part of the "stress testing" samples.
;; It attempts to showcase simple thread spawning, waiting for messages,
;; and sending replies.
(begin

	;; Number of threads to spawn - on or1k we spawn less threads
	;; because it is a much slower platform.
	(define ThreadCount
		(if (== or1k (arch))
			15
			50
		)
	)
	;; Uncomment for LOTS of debugging information
	;; (debug true)

	;; Helper function to spawn many threads with given Code and Argument.
	;; Returns list of thread references.
	(define spawn-many (# (Count Code Argument)
		(+ (list (spawn Code Argument)) (if (> Count 0) (spawn-many (- Count 1) Code Argument) (list)))
	))

	;; Thread code, make it do something a little intensive
	(define factorial (# (N) (fac2 N 1)))
	(define fac2 (# (N A) (if (= N 0) A (fac2 (- N 1) (* N A)))))
	(define ThreadCode (# (Parent) (begin
		(print (self) "ready for request")
		(receive
			(# (Message) (begin
				(print (self) "Received message: " Message)
				(define MsgBody (head Message))
				(define MsgSender (head (tail Message)))
				(send (list complete MsgBody (factorial MsgBody)) MsgSender)
			))
			after 2000 (# () (begin
				(print (self) "Timeout")
			))
		)
		(print (self) "exiting")
	)))
	;; Master thread send/receive loop
	(define message-loop (# (Threads N) (begin
		(if (> (length Threads) 0)
			;; Still have threads, send a request
			(begin
				(define Thread (head Threads))
				(print "Requesting factorial " N "from" Thread)
				(send (list N (self)) Thread)
				(message-loop (tail Threads) (+ N 1))
			)
			;; else, sent messages to all threads, get responses.
			;; we time out after 2s since we don't keep track of responses remaining.
			(begin
				(receive
					(# (Message) (begin
						(print "Got a response: " Message)
						(message-loop (list) N)
					))
					after 2000 (# () (begin
						(print "No message received after 2000ms")
					))
				)
			)
		)
	)))

	;; Spawn our threads
	(define Threads (spawn-many ThreadCount ThreadCode (self)))
	(print "Spawn many: " Threads)

	;; Enter message loop
	(message-loop Threads 2)

	(print "Master thread exiting")
)
