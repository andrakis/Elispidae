#include <cstdlib>      // exit()
#include "stdafx.h"
#include "../PLint_stackless.hpp"

using namespace PocoLithp;
using namespace PocoLithp::Stackless;

// Get thread reference to current running thread
LithpCell proc_self(const LithpCells &c, Env_p env, const LithpImplementation &impl) {
	return LithpCell(Thread, LithpThreadReference(impl));
}

// Get a list of references to all threads
LithpCell proc_threads(const LithpCells &c) {
	return LithpProcessMan.getThreadReferences();
}

// Send a message to a thread
// (send message thread) => boolean()
LithpCell proc_send(const LithpCells &c) {
	auto it = c.cbegin();
	if (it == c.cend())
		throw InvalidArgumentException("(send: requires a message)");
	const LithpCell &message = *it; ++it;
	if (it == c.cend())
		throw InvalidArgumentException("(send: requires a thread)");
	const LithpCell &threadref = *it; ++it;
	if (threadref.tag != Thread)
		throw InvalidArgumentException("(send: thread should be a threadref)");
	return LithpProcessMan.send(message, threadref.thread_ref()) ? sym_true : sym_false;
}

// Flush messages for a given thread and return as list
// (flush thread [max_messages]) => list(message())
LithpCell proc_flush(const LithpCells &c) {
	auto it = c.cbegin();
	if (it == c.cend())
		throw InvalidArgumentException("(flush: requires a thread)");
	const LithpCell &threadref = *it; ++it;
	if (threadref.tag != Thread)
		throw InvalidArgumentException("(flush: thread should be a threadref)");
	UnsignedInteger limit = 0, count = 0;
	if (it != c.cend())
		limit = it->value.convert<UnsignedInteger>();
	LithpCells result;
	for(;;) {
		LithpCell message;
		if (LithpProcessMan.receive(message, threadref.thread_ref())) {
			result.push_back(LithpCell(message));
		} else {
			break;
		}
		++count;
		if (limit != 0 && count == limit)
			break;
	}
	return LithpCell(List, result);
}

// Spawn a new microthread.
// (spawn Lambda Arguments) => thread_ref()
LithpCell proc_spawn(const LithpCells &c, Env_p env) {
	auto it = c.cbegin();
	if (it == c.cend())
		throw new InvalidArgumentException("(spawn: requires a lambda)");
	// Get lambda
	const LithpCell &lambda = *it; ++it;
	// Get arguments (may be an empty list)
	const LithpCells args(it, c.cend());
	// Create new environment for thread
	Env_p new_env(new LithpEnvironment(env));
	// Create instruction
	LithpCells ins_members;
	//   lambda (instruction to execute)
	ins_members.push_back(lambda);
	//   arguments (may be none)
	ins_members.insert(ins_members.end(), args.begin(), args.end());
	// Final instruction
	LithpCell ins(List, ins_members);
	// Create thread
	LithpThreadReference thread = LithpProcessMan.start(ins, new_env);
	// Create thread reference
	return LithpCell(Thread, thread);
}

// Check if a thread is still alive
LithpCell proc_alive(const LithpCells &c) {
	auto it = c.cbegin();
	if (it == c.cend())
		return LithpCell(Var, sym_false);
	const LithpCell thread_ref = *it; ++it;
	if (thread_ref.tag != Thread)
		throw InvalidArgumentException("(alive?: thread should be a threadref)");
	const bool alive = LithpProcessMan.isThreadAlive(thread_ref.thread_ref());
	return alive ? sym_true : sym_false;
}

// Force an exit and return an exit code from the application.
// (exit Code::integer(Default=1))
LithpCell proc_exit(const LithpCells &c) {
	int status = 1;
	auto it = c.cbegin();
	if (it != c.cend())
		status = it->value.convert<int>();
	if (GetDEBUG())
		GETLINE("Press ENTER to quit>");
	LithpProcessMan.force_exit();
	exit(status);
}

void Elispidae::Threads::init_threads() {
	add_environment_runtime([](LithpEnvironment &env) {
		env["self"] = LithpCell(&proc_self);
		env["threads"] = LithpCell(&proc_threads);
		env["send"] = LithpCell(&proc_send);
		env["flush"] = LithpCell(&proc_flush);
		env["spawn"] = LithpCell(&proc_spawn);
		env["exit"] = LithpCell(&proc_exit);
		env["alive?"] = LithpCell(proc_alive);
	});
}
