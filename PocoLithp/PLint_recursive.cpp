#include "stdafx.h"
#include "PLint_recursive.hpp"

namespace PocoLithp {
	#define INDENT()  std::string((depth * 2), ' ')
	// Eval inner-most loop that performs the interpretation.
	LithpCell RecursiveInterpreter::eval_inner(LithpCell x, Env_p env) {
		std::string debugstr = "";
		while (true) {
			if (x.tag == VariableReference) {
				return envLookup(x.atomid(), env);
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
				if (xl.size() < 1) throw InvalidArgumentException("Cannot quote nothing");
				return xl[1];
			} else if (xl0 == sym_if) {       // (if test conseq [alt])
				if (xl.size() < 3) throw InvalidArgumentException("Not enough arguments to if");
				// Tail recurse
				if (DEBUG) debugstr += INDENT() + to_string(x);
				x = eval(xl[1], env) == sym_false ? (xl.size() < 4 ? sym_nil : xl[3]) : xl[2];
				if (DEBUG) std::cerr << debugstr + " => " + to_string(x) + "\n";
				if (DEBUG) debugstr = "";
			} else if (xl0 == sym_get) {      // (get! var) - lookup by atom
				if (xl.size() < 2) throw InvalidArgumentException("Not enough arguments to get!");
				return envLookup(xl[1].atomid(), env);
			} else if (xl0 == sym_set) {      // (set! var exp)
				if (xl.size() < 3) throw InvalidArgumentException("Not enough arguments to set!");
				return env->find(xl[1].atomid())[xl[1].atomid()] = eval(xl[2], env);
			} else if (xl0 == sym_define) {   // (define var exp)
				if (xl.size() < 3) throw InvalidArgumentException("Not enough arguments to define");
				return (*env)[xl[1].atomid()] = eval(xl[2], env);
			} else if (xl0 == sym_defined) {  // (defined var)
				if (xl.size() < 2) return sym_false;
				return env->defined(xl[1].atomid()) ? sym_true : sym_false;
			} else if (xl0 == sym_lambda || xl0 == sym_lambda2) {   // (lambda (var*) exp), (# (var*) exp)
				x.tag = Lambda;
				// Keep a reference to the environment that exists now (when the
				// lambda is being defined) because that's the outer environment
				// we'll need to use when the lambda is executed
				x.env = env;
				return x;
			} else if (xl0 == sym_macro) {     // (macro (var*) exp)
				x.tag = Macro;
				x.env = nullptr;  // To be filled in later
				return x;
			} else if (xl0 == sym_begin) {     // (begin exp*)
				for (size_t i = 1; i < xl.size() - 1; ++i)
					eval(xl[i], env);
				// Tail recurse
				x = xl.back();
			} else {
				// (proc exp*)
				LithpCell proc = eval(xl[0], env);
				while (proc.tag == Atom || proc.tag == VariableReference || proc.tag == List) {
					if (proc.tag == Atom)
						proc = envLookup(proc.atomid(), env);
					else
						proc = eval(proc, env);
				}
				if (DEBUG) std::cerr << INDENT() << "  => " << to_string(proc) << "\n";
				if (DEBUG) debugstr = "";
				LithpCells exps;
				// Gather parameters
				bool isMacro = proc.tag == Macro;
				for (auto exp = xl.begin() + 1; exp != xl.end(); ++exp) {
					// If macro, skip eval
					exps.push_back(isMacro ? *exp : eval(*exp, env));
				}
				if (proc.tag == Lambda || proc.tag == Macro) {
					// Create an environment for the execution of this lambda function
					// where the outer environment is the one that existed* at the time
					// the lambda was defined and the new inner associations are the
					// parameter names with the given arguments.
					// *Although the environment existed at the time the lambda was defined
					// it wasn't necessarily complete - it may have subsequently had
					// more symbols defined in that environment.
					LithpCells proclist = proc.list();
					Env_p destEnv = proc.env;
					if (proc.tag == Macro)
						destEnv = env;
					LithpCells parms;
					// If argument is a variable, assign all arguments to that variable.
					// Otherwise, assign each variable to the appropriate argument.
					switch (proclist[1].tag) {
					case VariableReference:
					{
						parms.push_back(proclist[1]);
						// Move arguments to a single list
						LithpCells tmp = exps;
						exps.clear();
						exps.push_back(LithpCell(List, tmp));
						break;
					}
					case List:
						parms = proclist[1].list();
						break;
					}
					LithpEnvironment *child_env = new LithpEnvironment(/*parms*/parms, /*args*/exps, destEnv);
					if (proclist.size() < 3)
						return sym_nil;
					// Tail recurse
					x = proclist[2];
					env = Env_p(child_env);
				} else if (proc.tag == Proc) {
					return proc.proc()(exps);
				} else if (proc.tag == ProcExtended) {
					return proc.proc_extended()(exps, env);
				} else {
					throw RuntimeException("Unhandled type in eval_inner");
				}
			}
			reductions++;
		}
	}

	// Top-level eval function. Does depth tracking and exception management.
	LithpCell RecursiveInterpreter::eval(LithpCell x, Env_p env) {
		TRACK_STATS(++depth);
		try {
			const LithpCell &result = eval_inner(x, env);
			if (DEBUG) {
				std::cerr << INDENT() << "eval(" << to_string(x) << ") => " << to_string(result) << "\n";
			}
			TRACK_STATS(if (depth > depth_max) depth_max = depth; --depth;);
			return result;
		} catch (...) {
			TRACK_STATS(if (depth > depth_max) depth_max = depth; --depth;);
			throw;
		}
	}

	// Timed evaluation. For performance sake, we prefer this to be called once.
	LithpCell RecursiveInterpreter::evalTimed(const LithpCell &x, Env_p env) {
		auto start = std::chrono::steady_clock::now();
		const LithpCell &result = eval(x, env);
		auto end = std::chrono::steady_clock::now();
		evalTime += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		return result;
	}
}