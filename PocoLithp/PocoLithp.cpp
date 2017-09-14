// PocoLithp.cpp : Defines the entry point for the console application.
//

// TODO: Bignum support

#include "stdafx.h"

using namespace PocoLithp;

int main()
{
	std::cout << "Welcome to PocoLithp " PLITHP_VERSION " " << PLITHP_ARCH << std::endl;
	std::cout << "Type (q) to quit, (debug) to get / set state, (timing) to get / set state" << std::endl;
	LithpEnvironment global_env; add_globals(global_env);
	repl("plithp> ", &global_env);
	if(TIMING)
		std::cout << "Total eval time: " << evalTime << "ms, parse time: " << parseTime << "ms\n";
    return 0;
}

