;; Microthreading example

(begin

	(define Self (self))
	(print "Master Thread" Self " started")

	;; A cheap appromixation of receive.
	;; Actually, it's probably fairly CPU-intensive.
	(define recv (# (OnMessage) (begin
		;; Nice visualization of how message loop runs.
		(print "(" (self) ".recv)")
		(define Messages (flush (self) 1))
		;; TODO: implement (empty List) check
		(if (> (length Messages) 0)
			(OnMessage (head Messages))
			(recv OnMessage)
		)
	)))

	(define Thread1 (spawn (# (Parent) (begin
		(print "Thread1 starting up, self=" (self) ", parent=" Parent)
		(define Message (list (self) "sends its regards"))
		(send Message Parent)
		(print "Sent a message to " Parent)
		(print "Thread1 waiting for reply")
		(recv (# (Reply) (begin
			(print "Thread1 got reply: " Reply)
		)))
		(print "Thread1 exiting")
	)) Self))

	(print "Mater Thread: Waiting for message")
	(recv (# (Message) (begin
		(print (self) " got message: " Message)
		(define ReplyAddress (head Message))
		(print (self) " sending reply to " ReplyAddress)
		(send acknowledged ReplyAddress)
	)))
	(print "Master thread exiting")
)
