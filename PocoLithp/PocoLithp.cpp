// PocoLithp.cpp : Defines the entry point for the console application.
//

// TODO: Bignum support

#include "stdafx.h"

using namespace PocoLithp;

const int ERR_NOERROR = 0;
const int ERR_FILE = 1;
const int ERR_SYNTAX = 2;
const int ERR_EXCEPTION = 3;

int main(int argc, char *argv[])
{
	LithpEnvironment global_env; add_globals(global_env);
	//Test::RunTests();
	if (argc <= 1) {
		std::cerr << "Welcome to PocoLithp " PLITHP_VERSION " " << PLITHP_ARCH << std::endl;
		std::cerr << "Type (q) to quit, (debug) to get / set state, (timing) to get / set state, (tests) to run unit tests" << std::endl;
		repl("plithp> ", &global_env);
	} else {
		std::ifstream userfile(argv[1]);
		if (!userfile.is_open()) {
			std::cerr << "Failed to open file: " << argv[1] << std::endl;
			return ERR_FILE;
		}
		std::string buffer = "";
		std::string line;
		while (getline(userfile, line)) {
			buffer += "\n" + line;
		}
		userfile.close();
		try {
			const LithpCell &result = evalTimed(read(buffer), &global_env);
			std::cout << to_string(result) << std::endl;
		} catch (const SyntaxException) {
			std::cerr << "SYNTAX ERROR" << std::endl;
			return ERR_SYNTAX;
		} catch (const std::exception &e) {
			std::cerr << "ERROR " << e.what() << std::endl;
			return ERR_EXCEPTION;
		}
	}
	if(TIMING)
		std::cerr << "Total eval time: " << evalTime << "ms, parse time: " << parseTime << "ms\n";
    return ERR_NOERROR;
}

