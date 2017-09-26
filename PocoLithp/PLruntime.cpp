#include "stdafx.h"

using namespace PocoLithp;

// Runtime information
LithpCell proc_banner(const LithpCells &x) {
	return LithpCell(Var,
		"Welcome to PocoLithp " PLITHP_VERSION " " + PLITHP_ARCH + " " + STATS_DESC + "\n"
		"Type (q) to quit, (debug) to get / set state, (timing) to get / set state" + "\n"
		"  Additional useful functions: (tests) (str expr) (str expr true) (env) (_depth) (_max_depth)\n"
		"                             : (repl [prompt]) (_eval expr)\n"
	);
}

// File IO
LithpCell proc_readfile(const LithpCells &x) {
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

void PocoLithp::init_runtime() {
    add_environment_runtime([](LithpEnvironment &env) {
            env["banner"] = LithpCell(&proc_banner);
            env["readfile"] = LithpCell(&proc_readfile);
            env["atom"] = LithpCell(&proc_atom);
    });
}
