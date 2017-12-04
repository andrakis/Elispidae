#include "stdafx.h"
#include "PLint_stackless.hpp"
#include <Stackless.hpp>

using namespace stackless;
using namespace stackless::microthreading;
using namespace stackless::timekeeping;

#if 1
#define DEBUG(m)
#else
#define DEBUG(m)   std::cerr << "! " << m << std::endl;
#endif

namespace PocoLithp {
	namespace Stackless {
		namespace instruction {
			enum instruction {
				Quote,
				If,
				Set,
				Get,
				Define,
				Begin,
				Proc,    // handles Lambda, Macro, Proc, ProcExtended
				Lambda,
				Macro,
				Invalid
			};
		}

		struct LithpInstructionConverter
			: public InstructionConverter<LithpCell, typename instruction::instruction> {
			static _instruction_type convert(_cell_type value) {
				switch (value.tag) {
				case Lambda:
				case Proc:
				case ProcExtended:
				case Macro:
					return instruction::Proc;
				case Atom:
					if (value == sym_quote) return instruction::Quote;
					if (value == sym_if) return instruction::If;
					if (value == sym_set) return instruction::Set;
					if (value == sym_define) return instruction::Define;
					if (value == sym_get) return instruction::Get;
					if (value == sym_lambda || value == sym_lambda2) return instruction::Lambda;
					if (value == sym_macro) return instruction::Macro;
					// Fall through
				case List:
					return instruction::Proc;
				default:
					return instruction::Invalid;
				}
			}
		};

		// Frame implementation
		struct LithpFrame : public Frame<LithpCell, typename instruction::instruction, LithpEnvironment> {
		private:
			LithpFrame(env_p environment)
				: Frame(environment),
				exp(sym_nil),
				expressions(),
				arguments(),
				resolved_arguments(),
				exp_it(expressions.cbegin()),
				arg_it(arguments.cbegin()),
				resolved(false),
				subframe(nullptr),
				subframe_mode(None)
			{
			}
		public:
			LithpFrame(const LithpCell &expression, env_p environment)
				: LithpFrame(environment) {

			}

			enum SubframeMode {
				None,
				Argument,
				Procedure
			};

			bool isResolved() const { return resolved; }
			// TODO: no longer used
			bool isArgumentsResolved() const { return true; }

			void execute() {
				if (isResolved())
					return;

			}

			void nextArgument() {
				if (arg_it == arguments.cend())
					dispatch();
				else if (resolveArgument(*arg_it)) {
					++arg_it;
					nextArgument();
				}
			}

			void nextExpression() {
				if (exp_it != expressions.cend()) {
					++exp_it;
					if (exp_it != expressions.cend()) {
						setExpression(*exp_it);
						resolved = false;
					} else {
						resolved = true;
						exp = sym_nil;
					}
				} else {
					resolved = true;
					exp = sym_nil;
				}
			}

			void setExpression(const LithpCell &value) {
				DEBUG("setExpression(" + to_string(value) + ")");
				resolved = false;
				arguments.clear();
				resolved_arguments.clear();
				LithpCell val(value);
				if (resolveExpression(val)) {
					arg_it = arguments.cbegin();
					resolved_arguments.clear();
					resolved = true;
					return;
				}
				arg_it = arguments.cbegin();
				nextArgument();
			}

			bool resolveArgument(const LithpCell &value) {
				switch (value.tag) {
				case VariableReference:
					resolved_arguments.push_back(lookup(value));
					DEBUG(std::string("  resolveArgument(") + to_string(value) + std::string(") = ") + to_string(lookup(value.val)));
					return true;
				case List:
					if (value.list().empty()) {
						resolved_arguments.push_back(value);
						return true;
					}
					// Function call. Create frame to execute it.
					subframe = new LithpFrame(value, env);
					subframe_mode = Argument;
					return false;
				default:
					DEBUG(std::string("  resolveArgument(") + to_string(value) + std::string(") = ") + to_string(value));
					resolved_arguments.push_back(value);
					return true;
				}
			}

			LithpCell &lookup(const LithpCell &symbol) {
				return env->find(symbol.atomid())[symbol.atomid()];
			}

			bool dispatchCall();
			void dispatch() {
				if (dispatchCall())
					nextExpression();
			}

			inline bool checkArgument(const LithpCells::const_iterator &it, const LithpCells::const_iterator &end, const std::string &message) const {
				if (it == end) throw InvalidArgumentException(message);
				return true;
			}

			bool resolveExpression(LithpCell &value) {
				switch (value.tag) {
				case VariableReference:
					result = lookup(value);
					return true;
				case Var:
					result = value;
					return true;
				case List:
				{
					if (value.list().empty()) {
						result = value;
						return true;
					}
					auto first = value.list()[0];
					// iterator skips first item
					const LithpCells::const_iterator begin = value.list().cbegin();
					LithpCells::const_iterator it = begin + 1;
					const LithpCells::const_iterator end = value.list().cend();
#define ARG(message) checkArgument(it, end, message)
					arguments = LithpCells();
					exp = first;
					if (exp.tag == Atom) {
						if (first == sym_quote) {         // (quote exp)
							ARG("Cannot quote nothing");
							result = *it;
							return true;
						} else if (first == sym_if) {     // (if test conseq [alt])
							// becomes (if conseq alt test)
							// test
							ARG("if: requires a test");
							arguments.push_back(*it); ++it;
							// conseq
							ARG("if: requires a conseq");
							resolved_arguments.push_back(*it); ++it;
							// [alt]
							if (it != value.list().cend())
								resolved_arguments.push_back(*it);
							else
								resolved_arguments.push_back(sym_nil);
							return false;
						} else if (first == sym_set) {   // (set! var exp)
							// var
							ARG("set: requires var name");
							resolved_arguments.push_back(*it); ++it;
							// exp
							ARG("set: requires expression value");
							arguments.push_back(*it); ++it;
							return false;
						} else if (first == sym_define) { // (define var exp)
							// var
							ARG("define: requires var name");
							resolved_arguments.push_back(*it); ++it;
							// exp
							ARG("define: requires expression value");
							arguments.push_back(*it); ++it;
							return false;
						} else if (first == sym_defined) { // (defined var) => bool()
							// var
							ARG("defined: requires var name");
							LithpCell defined = env->defined(it->atomid()) ? sym_true : sym_false;
							result = defined;
							return true;
						} else if (first == sym_lambda || first == sym_lambda2) { // (lambda (var*) exp)
							result = LithpCell(value);
							result.tag = Lambda;
							result.env = env;
							return true;
						} else if(first == sym_macro) { // (macro (var*) exp)
							result = LithpCell(value);
							result.tag = Macro;
							result.env = env;
							return true;
						} else if (first == sym_begin) {  // (begin exp*)
							resolved_arguments = LithpCells(begin, end);
							return false;
						}
					}
					// (proc exp*)
					arguments = LithpCells(begin, end);
					return false;
				}
				default:
					// Other sort of value
					result = value;
					return true;
				}
			}

			LithpCell exp;
			LithpCells expressions;
			LithpCells arguments;
			LithpCells resolved_arguments;
			typename LithpCells::const_iterator exp_it;
			typename LithpCells::const_iterator arg_it;
			bool resolved;
			LithpFrame *subframe;
			SubframeMode subframe_mode;
		};
	}

	#define INDENT()  std::string((depth * 2), ' ')
	// Eval inner-most loop that performs the interpretation.
	LithpCell StacklessInterpreter::eval_inner(LithpCell x, Env_p env) {
		return sym_nil;
	}

	// Top-level eval function. Does depth tracking and exception management.
	LithpCell StacklessInterpreter::eval(LithpCell x, Env_p env) {
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
	LithpCell StacklessInterpreter::evalTimed(const LithpCell &x, Env_p env) {
		auto start = std::chrono::steady_clock::now();
		const LithpCell &result = eval(x, env);
		auto end = std::chrono::steady_clock::now();
		evalTime += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		return result;
	}
}
