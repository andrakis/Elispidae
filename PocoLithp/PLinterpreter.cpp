#include "stdafx.h"

namespace PocoLithp {
	bool DEBUG = false;
	bool TIMING = false;
	bool QUIT = false;

	UnsignedInteger parseTime = 0, evalTime = 0;
	UnsignedInteger reductions = 0;

	int LithpEnvironment::child_env_delete_depth = 0;

	LithpCell eval(LithpCell x, LithpEnvironment *env) {
		goto entry;
	reduce:
		reductions++;
	entry:
		if (x.tag == Atom) {
			LithpCell &r = env->find(x.atomid())[x.atomid()];
			if (DEBUG) std::cerr << to_string(x) << " => " << to_string(r) << "\n";
			return r;
		}
		// TODO: Variable lookup (must currently use Atom)

		// Vars can be returned as is
		if (x.tag == Var)
			return x;
		// TODO: Differentiate between list and opchain? Current implementation assumes list and
		// determines type
		if (!x.value.isArray() || x.value.size() == 0)
			return sym_nil;
		const LithpCells &xl = x.list();
		const LithpCell &xl0 = xl[0];
		if (xl0.tag == Atom) {
			if (xl0 == sym_quote)      // (quote exp)
				return xl[1];
			if (xl0 == sym_if) {       // (if test conseq [alt])
				x = eval(xl[1], env) == sym_false ? (xl.size() < 4 ? sym_nil : xl[3]) : xl[2];
				goto reduce;
				//return eval(eval(xl[1], env) == sym_false ? (xl.size() < 4 ? sym_nil : xl[3]) : xl[2], env);
			}
			if (xl0 == sym_set)        // (set! var exp)
				return env->find(xl[1].atomid())[xl[1].atomid()] = eval(xl[2], env);
			if (xl0 == sym_define)     // (define var exp)
				return (*env)[xl[1].atomid()] = eval(xl[2], env);
			if (xl0 == sym_lambda) {   // (lambda (var*) exp)
				x.tag = Lambda;
				// Keep a reference to the environment that exists now (when the
				// lambda is being defined) because that's the outer environment
				// we'll need to use when the lambda is executed
				x.env = env;
				return x;
			}
			if (xl0 == sym_begin) {     // (begin exp*)
				for (size_t i = 1; i < xl.size() - 1; ++i)
					eval(xl[i], env);
				x = xl.back();
				goto reduce;
				//return eval(xl.back(), env);
			}
		}
		// (proc exp*)
		LithpCell proc(eval(xl[0], env));
		LithpCells exps;
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
			env->remember_child_env(child_env);
			return eval(/*body*/proclist[2], child_env);
		} else if (proc.tag == Proc) {
			const LithpCell &r = proc.proc()(exps, env);
			if (DEBUG) std::cerr << "<Proc>" << to_string(LithpVar(List, exps)) << " => " << to_string(r) << "\n";
			return r;
		}
		throw InvalidArgumentException("Unhandled type");
	}
	LithpCell evalTimed(const LithpCell &x, LithpEnvironment *env) {
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
			const LithpCells &expl = exp.list();
			for (LithpCells::const_iterator e = expl.begin(); e != expl.end(); ++e)
				s += to_string(*e) + ' ';
			if (s[s.size() - 1] == ' ')
				s.erase(s.size() - 1);
			return s + ')';
		}
		else if (exp.tag == Lambda)
			return "<Lambda>";
		else if (exp.tag == Proc)
			return "<Proc>";
		return exp.str();
	}

	// the default read-eval-print-loop
	void repl(const std::string & prompt, LithpEnvironment *env)
	{
		std::string partialBuffer = "";

		while(!QUIT) {
			if (partialBuffer.length() == 0) {
				std::cerr << prompt;
			} else {
				std::cerr << "> ";
			}
			std::string line; std::getline(std::cin, line);
			try {
				auto start = std::chrono::steady_clock::now();
				if (partialBuffer.length() != 0) {
					line = partialBuffer + " " + line;
					partialBuffer = "";
				}
				std::cout << to_string(evalTimed(read(line), env)) << '\n';
				if (TIMING) {
					auto end = std::chrono::steady_clock::now();
					std::cerr << "evaluated in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
				}
			} catch (const SyntaxException) {
				partialBuffer += line;
			} catch (const std::exception &e) {
				std::cerr << "ERROR " << e.what() << std::endl;
			}
		}
	}
}
