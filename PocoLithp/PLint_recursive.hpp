#pragma once

#include "stdafx.h"

namespace PocoLithp {
	class RecursiveInterpreter : public Interpreter {
	public:
		RecursiveInterpreter() : Interpreter() { }
	private:
#define INDENT()  std::string((depth * 2), ' ')

		LithpCell envLookup(atomId id, Env_p env) {
			return env->find(id)[id];
		}

		// Eval inner-most loop that performs the interpretation.
		LithpCell eval_inner(LithpCell x, Env_p env);

	public:
		// Top-level eval function. Does depth tracking and exception management.
		LithpCell eval(LithpCell x, Env_p env);
		// Timed evaluation. For performance sake, we prefer this to be called once.
		LithpCell evalTimed(const LithpCell &x, Env_p env);
		static RecursiveInterpreter *Instantiate() {
			return new RecursiveInterpreter();
		}
	};
}
