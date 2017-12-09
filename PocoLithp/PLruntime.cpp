#include "stdafx.h"

using namespace PocoLithp;

// Runtime information
LithpCell proc_banner(const LithpCells &x) {
	return LithpCell(Var,
		"Welcome to " PLITHP_VERSION_INFO "\n"
		"Type (q) to quit, (debug) to get / set state, (timing) to get / set state\n"
		"  Additional useful functions: (tests) (str expr) (str expr true) (env) (_depth) (_max_depth)\n"
		"                             : (repl [prompt]) (_eval expr) (expl \"string\") \n"
	);
}

// File IO
LithpCell proc_readfile(const LithpCells &x) {
	if (x.size() < 1)
		throw InvalidArgumentException("No filename provided");
	std::ifstream f(x[0].str());
	std::stringstream ss;
	ss << f.rdbuf();
	f.close();
	return LithpCell(Var, ss.str());
}

// Convert a string to an atom
LithpCell proc_atom(const LithpCells &x) {
	if(x.size() == 0)
		return LithpCell(Atom, "atom");
	return LithpCell(Atom, x[0].str());
}

// Throw an InvalidArgument exception
LithpCell proc_invalid_arg(const LithpCells &c) {
	std::string message = "";
	if(c.size() > 0)
		message = c[0].str();
	throw InvalidArgumentException(message);
}

// Get thread id
LithpCell proc_self(const LithpCells &c, Env_p env) {
	LithpEnvironment::Microthread *thread = env->getThread();
	if (thread == nullptr)
		return LithpCell(Atom, "invalid_thread");
	return LithpCell(Var, thread->thread_id);
}

void PocoLithp::init_runtime() {
	add_environment_runtime([](LithpEnvironment &env) {
		env["banner"] = LithpCell(&proc_banner);
		env["readfile"] = LithpCell(&proc_readfile);
		env["atom"] = LithpCell(&proc_atom);
		env["invalid_arg"] = LithpCell(&proc_invalid_arg);
		env["self"] = LithpCell(&proc_self);
	});
}
