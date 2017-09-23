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
	return ss.str();
}

size_t _ = add_environment_runtime([](LithpEnvironment &env) {
	env["banner"] = LithpCell(&proc_banner);
	env["readfile"] = LithpCell(&proc_readfile);
});