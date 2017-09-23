#include "stdafx.h"

namespace PocoLithp {
	bool DEBUG = false;
	bool TIMING = false;
	bool QUIT = false;

	UnsignedInteger parseTime = 0, evalTime = 0;
	UnsignedInteger reductions = 0, depth = 0, depth_max = 0;

	int LithpEnvironment::child_env_delete_depth = 0;

#define INDENT()  std::string((depth * 2), ' ')

	inline LithpCell envLookup(atomId id, Env_p env) {
		return env->find(id)[id];
	}

	// Eval inner-most loop that performs the interpretation.
	inline LithpCell eval_inner(LithpCell x, Env_p env) {
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
				return xl[1];
			} else if (xl0 == sym_if) {       // (if test conseq [alt])
				// Tail recurse
				if (DEBUG) debugstr += INDENT() + to_string(x);
				x = eval(xl[1], env) == sym_false ? (xl.size() < 4 ? sym_nil : xl[3]) : xl[2];
				if (DEBUG) std::cerr << debugstr + " => " + to_string(x) + "\n";
				if (DEBUG) debugstr = "";
			} else if (xl0 == sym_get) {      // (get! var) - lookup by atom
				return envLookup(xl[1].atomid(), env);
			} else if (xl0 == sym_set) {      // (set! var exp)
				return env->find(xl[1].atomid())[xl[1].atomid()] = eval(xl[2], env);
			} else if (xl0 == sym_define) {   // (define var exp)
				return (*env)[xl[1].atomid()] = eval(xl[2], env);
			} else if (xl0 == sym_defined) {  // (defined var)
				return env->defined(xl[1].atomid()) ? sym_true : sym_false;
			} else if (xl0 == sym_lambda || xl0 == sym_lambda2) {   // (lambda (var*) exp), (# (var*) exp)
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
				LithpCell proc = eval(xl[0], env);
				while (proc.tag == Atom || proc.tag == VariableReference || proc.tag == List) {
					if (proc.tag == Atom)
						proc = envLookup(proc.atomid(), env);
					else
						proc = eval(proc, env);
				}
				if (DEBUG) std::cerr << debugstr << " => " << to_string(proc) << "\n";;
				if (DEBUG) debugstr = "";
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
	inline LithpCell eval(LithpCell x, Env_p env) {
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
	LithpCell evalTimed(const LithpCell &x, Env_p env) {
		auto start = std::chrono::steady_clock::now();
		const LithpCell &result = eval(x, env);
		auto end = std::chrono::steady_clock::now();
		evalTime += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		return result;
	}

	template<typename T>
	std::string rawstr(const T &v) {
		std::stringstream ss;
		ss << v;
		return ss.str();
	}

	// convert given LithpCell to a Lisp-readable string
	std::string to_string(const LithpCell &exp)
	{
		return to_string(exp, false, false);
	}

	// convert given LithpCell to a Lisp-readable string
	// param repre: true to return as underlying representation
	std::string to_string(const LithpCell &exp, bool advanced, bool repre)
	{
		if (exp.tag == Var || exp.tag == Atom || exp.tag == VariableReference) {
			if (!repre)
				return exp.str();
			else {
				if (exp.value.isString())
					return "\"" + exp.value.toString() + "\"";
				else
					return exp.value.toString();
			}
		} else if (!advanced && !repre && exp.tag == Lambda)
			return "<Lambda>";
		else if (exp.tag == Proc)
			return repre ? rawstr(exp.proc()) : "<Proc>";
		else if (exp.tag == ProcExtended)
			return repre ? rawstr(exp.proc_extended()) : "<ProcExtended>";
		else if (exp.tag == Dict) {
			std::string s("{");
			const LithpDict &dict = exp.dict();
			bool first = true;
			for (LithpDict::const_iterator it = dict.begin(); it != dict.end(); ++it) {
				if (first)
					first = false;
				else
					s += ", ";
				// In repre mode, just get integer value as string. Otherwise, get atom name.
				std::string v1 = (repre ? std::to_string(it->first) : getAtomById(it->first));
				s += v1 + ": " + to_string(it->second, advanced, repre);
			}
			return s + "}";
		}

		// A list, or repre is true and we want the list representation
		std::string s("(");
		const LithpCells &list = exp.list();
		bool first = true;
		for (LithpCells::const_iterator it = list.begin(); it != list.end(); ++it) {
			if (first)
				first = false;
			else
				s += " ";
			s += to_string(*it, advanced, repre);
		}
		return s + ')';
	}

	// the default read-eval-print-loop
	LithpCell repl(const std::string & prompt, Env_p env)
	{
		std::string partialBuffer = "";
		LithpCell result = sym_nil;

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
				result = evalTimed(read(line), env);
				std::cout << to_string(result) << "\n";
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

		// Reset QUIT in case REPL was invoked from a script
		QUIT = false;
		return result;
	}
}
