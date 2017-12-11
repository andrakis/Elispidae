#include "stdafx.h"
#include "PLint_recursive.hpp"
#include "PLint_stackless.hpp"

namespace PocoLithp {
	UnsignedInteger parseTime = 0;

	Interpreter *interpreter = nullptr;
	void SetStandardInterpreter() {
#ifdef PLITHP_STACKLESS
		interpreter = new StacklessInterpreter();
#else
		interpreter = new RecursiveInterpreter();
#endif
	}
	Interpreter *StandardInterpreter() {
		return interpreter;
	}

	bool GetDEBUG() {
		return interpreter->DEBUG;
	}
	void SetDEBUG(const bool d) {
		interpreter->DEBUG = d;
	}
	bool GetTIMING() {
		return interpreter->TIMING;
	}
	void SetTIMING(const bool t) {
		interpreter->TIMING = t;
	}
	bool GetQUIT() {
		return interpreter->QUIT;
	}
	void SetQUIT(const bool q) {
		interpreter->QUIT = q;
	}

	UnsignedInteger GetEvalTime() {
		return interpreter->evalTime;
	}
	UnsignedInteger GetParseTime() {
		return parseTime;
	}
	UnsignedInteger GetReductions() {
		return interpreter->reductions;
	}
	UnsignedInteger GetDepth() {
		return interpreter->depth;
	}
	UnsignedInteger GetDepthMax() {
		return interpreter->depth_max;
	}

	LithpCell eval(LithpCell x, Env_p env) {
		if (interpreter == nullptr)
			SetStandardInterpreter();
		return interpreter->eval(x, env);
	}
	
	// Timed evaluation. For performance sake, we prefer this to be called once.
	LithpCell evalTimed(const LithpCell &x, Env_p env) {
		if (interpreter == nullptr)
			SetStandardInterpreter();
		return interpreter->evalTimed(x, env);
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
		else if (exp.tag == Thread)
			return exp.str();
		else if (!advanced && !repre && exp.tag == Macro)
			return "<Macro>";
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
	LithpCell repl(const std::string &prompt, Env_p env)
	{
		std::string partialBuffer = "";
		LithpCell result = sym_nil;

		while(!GetQUIT()) {
			// Continuation prompt if partialBuffer has content, else default prompt
			std::string currentPrompt = partialBuffer.length() > 0 ? "> " : prompt;
			std::string line = GETLINE(currentPrompt);
			// If EOF on input, quit now.
			if(GETLINE_EOF()) break;
			// Empty lines don't do anything
			if (line.length() == 0)
				continue;
			// Continue previous buffer
			if (partialBuffer.length() != 0) {
				line = partialBuffer + "\n" + line;
				partialBuffer = "";
			}
			// Start measurements
			try {
				auto prev = GetEvalTime();
				result = evalTimed(read(line), env);
				std::cout << to_string(result) << "\n";
				if (GetTIMING()) {
					auto end = GetEvalTime();
					std::cerr << "evaluated in " << (end - prev) << "ms\n";
				}
			} catch (const SyntaxException) {
				partialBuffer += line;
			} catch (const std::exception &e) {
				std::cerr << "ERROR " << e.what() << "\n";
			}
		}

		// Reset QUIT in case REPL was invoked from a script
		SetQUIT(false);
		return result;
	}
}
