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
	LithpCells list;
	return sym_nil; // FIXME
	/*auto threads = LithpThreadMan.getThreads();
	for (auto it = threads.cbegin(); it != threads.cend(); ++it)
		list.push_back(LithpCell(Thread, LithpThreadReference(it->thread_id)));
	return LithpCell(List, list);*/
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
	return sym_nil; // FIXME
	// Todo: lookup by thread reference
	//LithpThreadManager &man = LithpThreadMan;
	//return man.send(message, threadref.thread_ref()) ? sym_true : sym_false;
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
	return sym_nil; // FIXME
	// Todo: lookup by thread reference
	/*LithpThreadManager &man = LithpThreadMan;
	LithpCells result;
	for(;;) {
		LithpCell message;
		if (man.receive(message, threadref.thread_ref())) {
			result.push_back(LithpCell(message));
		} else {
			break;
		}
		++count;
		if (limit != 0 && count == limit)
			break;
	}
	return LithpCell(List, result);*/
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
	return sym_nil; // FIXME
	/*
	LithpThreadManager &tm = LithpThreadMan;
	// Create thread
	ThreadId thread = tm.start([&lambda, &args, env]() {
		// Create new environment for thread
		Env_p new_env(new LithpEnvironment(env));
		LithpCells ins_members;
		ins_members.push_back(lambda);
		ins_members.insert(ins_members.end(), args.begin(), args.end());
		LithpCell ins(List, ins_members);
		LithpThreadManager::impl_p impl(new LithpImplementation(ins, env));
		return impl;
	});
	// Create thread reference
	return LithpCell(Thread, LithpThreadReference(thread));*/
}

void Elispidae::Threads::init_threads() {
	add_environment_runtime([](LithpEnvironment &env) {
		env["self"] = LithpCell(&proc_self);
		env["threads"] = LithpCell(&proc_threads);
		env["send"] = LithpCell(&proc_send);
		env["flush"] = LithpCell(&proc_flush);
		env["spawn"] = LithpCell(&proc_spawn);
	});
}
