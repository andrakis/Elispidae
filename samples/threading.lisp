;; Microthreading example
;;
;; This example demonstrates:
;;  o  Starting microthreads with arguments
;;  o  Sending messages between microthreads
;;  o  Waiting for messages before code execution continues
;;  o  Callbacks for message wait timeouts
;;
;; Key types::
;;  o  thread_ref()    A reference to a thread
;; Key functions:
;;  o  (send Message::any() Destination::thread_ref()) -> bool()
;;     - (send "sending myself love" (self)) -> true
;;     - (send "not around anymore" NonExistantThread) -> false
;;  o  (spawn lambda(Args) Args::any()) -> thread_ref()
;;     - (spawn (# () (print "Short lived thread"))) -> thread_ref()
;;     - (spawn (# (Parent) (send short_lived Parent)) (self)) -> thread_ref()
;;  o  (receive OnMessage::lambda(Message) timeout()) -> lambda::result
;;     timeout() -> phrase 'infinity' |
;;                  phrase 'after' Timeout::int() OnTimeout::lambda()
;;     - (receive (# (Message) (print "Message:" Message))) -> nil
;;     - (receive (# (Number) (+ 1 Number))) -> int()
;;     - (receive (# (X Y) (+ X Y)) after 5000 (# () 0)) -> int()

(begin

	(define Self (self))
	(print "Master Thread" Self " started")

	(print "Master Thread: Waiting for a message for 2s")
	;; (print "BEING CHEEKY AND SENDING MYSELF A MESSAGE")
	;; (send cheeky (self))
	(receive
		(# (Message) (begin
			(print (self) " got message when shouldnt: " Message)
		))
		after 2000 (# () (begin
			(print (self) " timeout occurred, success")
		))
	)

	(print "Master thread exiting")
)