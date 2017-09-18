#include "stdafx.h"

namespace PocoLithp {
	bool DEBUG = false;
	bool TIMING = false;
	bool QUIT = false;

	UnsignedInteger parseTime = 0, evalTime = 0;
	UnsignedInteger reductions = 0, depth = 0, depth_max = 0;

	int LithpEnvironment::child_env_delete_depth = 0;

#define INDENT()  std::string((depth * 2), ' ')

	// Eval inner-most loop that performs the interpretation.
	inline LithpCell eval_inner(LithpCell x, Env_p env) {
		while (true) {
			// TODO: Allow VariableReference too
			if (x.tag == Atom) {
				LithpCell &r = env->find(x.atomid())[x.atomid()];
				return r;
			} else if (x.tag != List) {
				// Anything not a list can be returned as is
				return x;
			} else if (!x.value.isArray() || x.value.size() == 0) {
				// Empty list is invalid to eval
				return sym_nil;
			}

			// (op [args...])
			LithpCells xl = x.list();
			const LithpCell &xl0 = xl[0];
			if (xl0 == sym_quote) {           // (quote exp)
				return xl[1];
			} else if (xl0 == sym_if) {       // (if test conseq [alt])
				// Tail recurse
				x = eval(xl[1], env) == sym_false ? (xl.size() < 4 ? sym_nil : xl[3]) : xl[2];
			} else if (xl0 == sym_set) {      // (set! var exp)
				return env->find(xl[1].atomid())[xl[1].atomid()] = eval(xl[2], env);
			} else if (xl0 == sym_define) {   // (define var exp)
				return (*env)[xl[1].atomid()] = eval(xl[2], env);
			} else if (xl0 == sym_lambda) {   // (lambda (var*) exp)
				x.tag = Lambda;
				// Keep a reference to the environment that exists now (when the
				// lambda is being defined) because that's the outer environment
				// we'll need to use when the lambda is executed
				x.env = env;
				return x;
			} else if (xl0 == sym_begin) {     // (begin exp*)
				for (size_t i = 1; i < xl.size() - 1; ++i)
					eval(xl[i], env);
				// Tail recurse
				x = xl.back();
			} else {
				// (proc exp*)
				LithpCell proc(eval(xl[0], env));
				LithpCells exps;
				// Gather parameters
				for (auto exp = xl.begin() + 1; exp != xl.end(); ++exp)
					exps.push_back(eval(*exp, env));
				if (proc.tag == Lambda) {
					// Create an environment for the execution of this lambda function
					// where the outer environment is the one that existed* at the time
					// the lambda was defined and the new inner associations are the
					// parameter names with the given arguments.
					// *Although the environmet existed at the time the lambda was defined
					// it wasn't necessarily complete - it may have subsequently had
					// more symbols defined in that environment.
					LithpCells proclist = proc.list();
					LithpEnvironment *child_env = new LithpEnvironment(/*parms*/proclist[1].list(), /*args*/exps, proc.env);
					// Tail recurse
					x = proclist[2];
					env = Env_p(child_env);
				} else if (proc.tag == Proc) {
					const LithpCell &r = proc.proc()(exps, env.get());
					return r;
				}
			}
			reductions++;
		}
	}

	// Top-level eval function. Does depth tracking and exception management.
	inline LithpCell eval(LithpCell x, Env_p env) {
		TRACK_STATS(++depth);
		try {
			const LithpCell &result = eval_inner(x, env);
			if (DEBUG) {
				std::cerr << INDENT() << to_string(x) << " => " << to_string(result) << "\n";
			}
			TRACK_STATS(if (depth > depth_max) depth_max = depth; --depth;);
			return result;
		} catch (...) {
			TRACK_STATS(if (depth > depth_max) depth_max = depth; --depth;);
			throw;
		}
	}
	
	// Timed evaluation. For performance sake, we prefer this to be called once.
	LithpCell evalTimed(const LithpCell &x, Env_p env) {
		auto start = std::chrono::steady_clock::now();
		const LithpCell &result = eval(x, env);
		auto end = std::chrono::steady_clock::now();
		evalTime += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		return result;
	}

	// convert given LithpCell to a Lisp-readable string
	std::string to_string(const LithpCell & exp)
	{
		if (exp.tag == List) {
			std::string s("(");
			const LithpCells &list = exp.list();
			bool first = true;
			for (LithpCells::const_iterator it = list.begin(); it != list.end(); ++it) {
				if (first)
					first = false;
				else
					s += " ";
				s += to_string(*it);
			}
			return s + ')';
		} else if (exp.tag == Dict) {
			std::string s("{");
			const LithpDict &dict = exp.dict();
			bool first = true;
			for (LithpDict::const_iterator it = dict.begin(); it != dict.end(); ++it) {
				if (first)
					first = false;
				else
					s += ", ";
				s += getAtomById(it->first) + ": " + to_string(it->second);
			}
			return s + "}";
		} else if (exp.tag == Lambda)
			return "<Lambda>";
		else if (exp.tag == Proc)
			return "<Proc>";
		return exp.str();
	}

	// the default read-eval-print-loop
	void repl(const std::string & prompt, Env_p env)
	{
		std::string partialBuffer = "";

		while(!QUIT) {
			if (partialBuffer.length() == 0) {
				std::cerr << prompt;
			} else {
				std::cerr << "> ";
			}
			std::string line; std::getline(std::cin, line);
			if (line.length() == 0)
				continue;
			try {
				auto start = std::chrono::steady_clock::now();
				if (partialBuffer.length() != 0) {
					line = partialBuffer + "\n" + line;
					partialBuffer = "";
				}
				std::cout << to_string(evalTimed(read(line), env)) << "\n";
				if (TIMING) {
					auto end = std::chrono::steady_clock::now();
					std::cerr << "evaluated in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << "\n";
				}
			} catch (const SyntaxException) {
				partialBuffer += line;
			} catch (const std::exception &e) {
				std::cerr << "ERROR " << e.what() << "\n";
			}
		}
	}
}
