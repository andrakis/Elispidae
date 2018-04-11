;; This is part of the "stress testing" samples.
;; It attempts to showcase simple thread spawning, waiting for messages,
;; and sending replies.
(begin

	;; Number of threads to spawn - on or1k we spawn less threads
	;; because it is a much slower platform.
	(define ThreadCount
		(if (== or1k (arch))
			10
			50
		)
	)
	;; Uncomment for LOTS of debugging information
	;; (debug true)

	;; Useful lists functions
	(define lists:foldl (# (Lst Acc Cb)
		(if (= 0 (length Lst)) Acc (lists:foldl (tail Lst) (Cb (head Lst) Acc) Cb))
	))
	;; Filter a list using a predicate. If Predicate returns true, it is kept.
	(define lists:filter (# (List Predicate)
		(lists:foldl List (list)
			(# (Ele Acc)
				(if (Predicate Ele) (+ Acc (list Ele)) Acc)))
	))

	;; Helper function to spawn many threads with given Code and Argument.
	;; Returns list of thread references.
	(define spawn-many (# (Count Code Argument)
		(spawn-many2 Count Code Argument (list))
	))
	(define spawn-many2 (# (Count Code Argument Acc) (begin
		(print "sm2: " Count Argument Acc)
		(if (= 0 Count)
			Acc
			;; else
			(spawn-many2 (- Count 1) Code Argument (+ Acc (list (spawn Code Argument))))
		)
	)))

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
				(send (list complete MsgBody (self) (factorial MsgBody)) MsgSender)
			))
			after 2000 (# () (begin
				(print (self) "Timeout")
			))
		)
		(print (self) "exiting")
	)))
	;; Master thread send/receive loop
	(define message-loop (# (ThreadsStarted ThreadsRunning N) (begin
		(if (> (length ThreadsStarted) 0)
			;; Still have threads, send a request
			(begin
				(define Thread (head ThreadsStarted))
				(print "Requesting factorial " N "from" Thread)
				(send (list (+ N 5) (self)) Thread)
				(message-loop (tail ThreadsStarted) (+ (list Thread) ThreadsRunning) (+ N 1))
			)
			;; else, sent messages to all threads, get responses.
			;; we time out after 2s since we don't keep track of responses remaining.
			(begin
				(if (= 0 (length ThreadsRunning))
					(print "All requests received, done.")
					;; else
					(receive
						(# (Message) (begin
							(print (self) "Got a response: " Message)
							(define MsgType (head Message))
							(define MsgParam (head (tail Message)))
							(define MsgSender (head (tail (tail Message))))
							(define Result (head (tail (tail (tail Message)))))
							(define Remaining (lists:drop ThreadsRunning MsgSender))
							(message-loop ThreadsStarted Remaining N)
						))
						after 2000 (# () (begin
							(print "No message received after 2000ms")
						))
					)
				)
			)
		)
	)))

	;; Spawn our threads
	(define Threads (spawn-many ThreadCount ThreadCode (self)))
	(print "Spawn many: " Threads)

	;; Enter message loop
	(message-loop Threads Threads 2)

	(print "Master thread exiting")
)
