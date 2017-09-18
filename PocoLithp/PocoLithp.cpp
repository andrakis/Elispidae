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
	LithpEnvironment *global_env = new LithpEnvironment();
	add_globals(*global_env);
	Env_p global_p(global_env);

	if (argc > 1) {
		std::ifstream userfile(argv[1]);
		if (!userfile.is_open()) {
			std::cerr << "Failed to open file: " << argv[1] << "\n";
			return ERR_FILE;
		}
		std::string buffer = "";
		std::string line;
		while (getline(userfile, line)) {
			buffer += "\n" + line;
		}
		userfile.close();
		try {
			const LithpCell &result = evalTimed(read(buffer), global_p);
			std::cout << to_string(result) << "\n";
		} catch (const SyntaxException) {
			std::cerr << "SYNTAX ERROR" << "\n";
			return ERR_SYNTAX;
		} catch (const std::exception &e) {
			std::cerr << "ERROR " << e.what() << "\n";
			return ERR_EXCEPTION;
		}
	} else {
		std::cerr << "Welcome to PocoLithp " PLITHP_VERSION " " << PLITHP_ARCH << " " << STATS_DESC << "\n";
		std::cerr << "Type (q) to quit, (debug) to get / set state, (timing) to get / set state" << "\n";
		std::cerr << "  Additional useful functions: (tests) (env) (_depth) (_max_depth)" << "\n";
		repl("plithp> ", global_p);
	}
	if(TIMING)
		std::cerr << "Total eval time: " << evalTime << "ms, parse time: " << parseTime << "ms\n";
    return ERR_NOERROR;
}

