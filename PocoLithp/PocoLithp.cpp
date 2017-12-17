// PocoLithp.cpp : Defines the entry point for the console application.
//

// TODO: Bignum support

#include "stdafx.h"

using namespace PocoLithp;

const int ERR_NOERROR = 0;
const int ERR_FILE = 1;
const int ERR_SYNTAX = 2;
const int ERR_EXCEPTION = 3;

std::string stdin_getline(const std::string &prompt) {
	std::string line;
	std::cout << prompt;
	std::getline(std::cin, line);
	return line;
}
bool stdin_eof() {
	return std::cin.eof();
}

#ifdef READLINE_NG
void linenoise_init () {
	const char *history = "./history";
	const char *prompt = "\x1b[1;32mplithp\x1b[0m> ";
	linenoiseInstallWindowChangeHandler();
	linenoiseHistoryLoad(history);
	//linenoiseSetCompletionCallback(completionHook);
}

bool linenoise_eof_flag = false;
bool linenoise_eof () { return linenoise_eof_flag; }
std::string linenoise_getline(const std::string &prompt) {
	std::string result = "";
	char *buf = linenoise(prompt.c_str());
	linenoise_eof_flag = (NULL == buf);
	if(buf && *buf != '\0') {
		result = std::string(buf);
		linenoiseHistoryAdd(buf);
	}
	free(buf);
	return result;
}
#endif

int main(int argc, char *argv[])
{
	GETLINE_INIT();
	LithpEnvironment *global_env = new LithpEnvironment();
	SetStandardInterpreter();
	PocoLithp::init_runtime();
	Elispidae::init_runtime();
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
		evalTimed(read(std::string("(begin (print (banner)))")), global_p);
		repl("Elisp> ", global_p);
	}

	// Run until all threads are finished
#ifdef ELISP_STACKLESS
	auto &tm = PocoLithp::Stackless::LithpThreadMan;
	if (GetDEBUG())
		std::cerr << "Waiting for " << tm.threadCount() << " threads to exit..." << std::endl;
	while (tm.hasThreads())
		tm.executeThreads();
#endif

	if(GetTIMING())
		std::cerr << "Total eval time: " << GetEvalTime() << "ms, parse time: " << parseTime << "ms\n";
    return ERR_NOERROR;
}

