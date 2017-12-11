#include "stdafx.h"

#include "PLint_stackless.hpp"
#include "ELisp.hpp"

using namespace PocoLithp;
using namespace PocoLithp::Stackless;

// Get thread reference to current running thread
LithpCell proc_self(const LithpCells &c, Env_p env) {
	auto thread = LithpThreadMan.getCurrentThread();
	LithpThreadReference thread_ref(thread->thread_id);
	return LithpCell(Thread, thread_ref);
}

// Get a list of references to all threads
LithpCell proc_threads(const LithpCells &c) {
	LithpCells list;
	auto threads = LithpThreadMan.getThreads();
	for (auto it = threads.cbegin(); it != threads.cend(); ++it)
		list.push_back(LithpCell(Thread, LithpThreadReference(it->thread_id)));
	return LithpCell(List, list);
}

void Elispidae::Threads::init_threads() {
	add_environment_runtime([](LithpEnvironment &env) {
		env["self"] = LithpCell(&proc_self);
		env["threads"] = LithpCell(&proc_threads);
	});
}
