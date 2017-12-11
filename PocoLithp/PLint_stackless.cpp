#include "stdafx.h"
#include "PLint_stackless.hpp"
#include <Stackless.hpp>

using namespace stackless;
using namespace stackless::microthreading;
using namespace stackless::timekeeping;

// TODO: this shouldn't be needed
unsigned stackless::microthreading::thread_counter = 0;

#if 1
#define DEBUG(m)
#else
#define DEBUG(m)   std::cerr << "! " << m << std::endl;
#endif

namespace PocoLithp {
	namespace Stackless {
		const LithpCell sym_receive(Atom, "receive");

		// Frame implementation
		void LithpFrame::execute(LithpImplementation &impl) {
			// TODO:HACK: Stop frames from completing before they're fully initialized
			if (wait_state == Initialize) {
				wait_state = Run;
				expressions.push_back(exp);
				exp_it = expressions.cbegin();
				setExpression(exp);
			}
			if (isResolved())
				return;
			if (isWaiting())
				return;
			if (subframe != nullptr) {
				subframe->execute(impl);
				if (subframe->isResolved()) {
					// Copy results
					LithpCell res(subframe->result);
					auto mode = subframe_mode;
					DEBUG("  subframe(" + std::string((mode == Argument ? "arg" : "proc")) + ") = " + to_string(res));
					// Clear subframe
					delete subframe;
					subframe = nullptr;
					subframe_mode = None;
					// Do something with result
					switch (mode) {
					case Argument:
						resolved_arguments.push_back(res);
						++arg_it;
						nextArgument();
						break;
					case Procedure:
						result = res;
						nextExpression();
						break;
					default:
						throw new RuntimeException("Invalid subframe mode None");
					}
				}
			} else if (arg_it == arguments.cend())
				dispatch();
			else
				nextArgument();
#if 0 && _DEBUG
			execute();
#endif
		}

		void LithpFrame::nextArgument() {
			if (arg_it == arguments.cend())
				dispatch();
			else if (resolveArgument(*arg_it, arg_id)) {
				++arg_it;
				++arg_id;
				nextArgument();
			}
		}

		void LithpFrame::nextExpression() {
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

		void LithpFrame::setExpression(const LithpCell &value) {
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
			arg_id = 0;
			nextArgument();
		}

		bool LithpFrame::resolveArgument(const LithpCell &value, const unsigned &id) {
			if (exp.tag == Macro) {
				// Macro mode, arguments not evaluated
				if (id > 0) {
					resolved_arguments.push_back(value);
					return true;
				}
			}
			switch (value.tag) {
			case VariableReference:
				resolved_arguments.push_back(lookup(value));
				DEBUG(std::string("  resolveArgument(") + to_string(value) + std::string(") = ") + to_string(lookup(value)));
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

		bool LithpFrame::resolveExpression(LithpCell &value) {
			switch (value.tag) {
			case VariableReference:
				result = lookup(value);
				return true;
			case Var:
			case Atom:
				result = LithpCell(value);
				return true;
			case List:
			{
				if (value.list().empty()) {
					result = value;
					return true;
				}
				// iterator skips first item
				const LithpCells::const_iterator begin = value.list().cbegin();
				auto first = *begin;
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
						if (it != end)
							resolved_arguments.push_back(*it);
						else
							resolved_arguments.push_back(sym_nil);
						return false;
					} else if (first == sym_get) {   // (get! var)
						// var
						ARG("get: requires var name");
						//resolved_arguments.push_back(*it); ++it;
						result = LithpCell(lookup(*it));
						return true;
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
						resolved_arguments = LithpCells(begin + 1, end);
						return false;
					} else if (first == sym_receive) { // (receive (# (Message::any)) [timeout [timeout callback]]}
						resolved_arguments = LithpCells(begin + 1, end);
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

		// Dispatcher implementation
		bool LithpDispatcher<instruction::If>::dispatch(LithpFrame &frame, const LithpCells::const_iterator &args) {
			LithpCells::const_iterator it(args);
			const LithpCell &conseq = *it; ++it;
			const LithpCell &alt = *it; ++it;
			const LithpCell &test = *it; ++it;
			const LithpCell &if_result = (test == sym_true) ? conseq : alt;
			frame.setExpression(LithpCell(if_result));
			// Don't move exp_it
			return false;
		}

		bool LithpDispatcher<instruction::Begin>::dispatch(LithpFrame &frame, const LithpCells::const_iterator &args) {
			// Update expressions to call list
			frame.expressions.swap(frame.resolved_arguments);
			frame.resolved_arguments.clear();
			frame.exp_it = frame.expressions.cbegin();
			frame.setExpression(*frame.exp_it);
			// Don't move exp_it
			return false;
		}

		bool LithpDispatcher<instruction::Get>::dispatch(LithpFrame &frame, const LithpCells::const_iterator &args) {
			LithpCells::const_iterator it(args);
			const LithpCell &var = *it; ++it;
			frame.result = frame.lookup(var);
			return true;
		}

		bool LithpDispatcher<instruction::Set>::dispatch(LithpFrame &frame, const LithpCells::const_iterator &args) {
			LithpCells::const_iterator it(args);
			const LithpCell &var = *it; ++it;
			const LithpCell &val = *it; ++it;
			frame.env->operator[](var.atomid()) = frame.result = val;
			return true;
		}

		bool LithpDispatcher<instruction::Define>::dispatch(LithpFrame &frame, const LithpCells::const_iterator &args) {
			LithpCells::const_iterator it(args);
			const LithpCell &var = *it; ++it;
			const LithpCell &val = *it; ++it;
			frame.env->operator[](var.atomid()) = frame.result = val;
			return true;
		}

		bool LithpDispatcher<instruction::Defined>::dispatch(LithpFrame &frame, const LithpCells::const_iterator &args) {
			LithpCells::const_iterator it(args);
			const LithpCell &var = *it; ++it;
			frame.result = frame.env->defined(var.atomid()) ? sym_true : sym_false;
			return true;
		}

		bool LithpDispatcher<instruction::Proc>::dispatch(LithpFrame &frame, const LithpCells::const_iterator &args) {
			LithpCells::const_iterator it(args);

			// TODO: This is a hack to allow atoms to be returned as is
			if (it == frame.resolved_arguments.cend()) {
				frame.result = LithpCell(frame.exp);
				return true;
			}

			LithpCell proc(*it); ++it;
			LithpCells arguments(it, frame.resolved_arguments.cend());

			// Secondary proc evaluation
			switch (proc.tag) {
			case Atom:
			case VariableReference:
			case Var:
				proc = LithpCell(frame.lookup(proc));
				break;
			default:
				// Do nothing
				break;
			}

			// Primary proc evaluation
			switch (proc.tag) {
				// Proc: a builtin procedure in C++ that needs no environment.
			case Proc:
				// Copy the rest of the resolved arguments to a new list,
				// that list is passed as the arguments.
				frame.result = proc.proc()(arguments);
				return true;
				// ProcExtended: a builtin procedure in C++ that passes the frame environment.
			case ProcExtended:
				// Copy the rest of the resolved arguments to a new list,
				// that list is passed as the arguments.
				frame.result = proc.proc_extended()(arguments, frame.env);
				return true;
				// A Lisp procedure or macro.
			case Lambda:
			case Macro:
			{
				// (lambda|macro ArgList::list()|var() Body::list())
				LithpCells arglist;
				const LithpCell &arglist_cell = proc.list()[1];
				switch (arglist_cell.tag) {
				case VariableReference:
					// Single argument, assign all args as list
					arglist.push_back(arglist_cell);
					break;
				case List:
					// List of arguments
					arglist = LithpCells(arglist_cell.list());
					break;
				default:
					throw new RuntimeException("Unknown arguments type");
				}
				// Body
				const LithpCell body = proc.list()[2];
				// Create environment parented to lambda/macro env
				Env_p new_env(new LithpEnvironment(proc.env));
				auto env_arg_it = arguments.cbegin();
				// assign remaining arguments to our list of argument names in
				// new environment.
				new_env->update(arglist, arguments);
				// create subframe
				frame.subframe_mode = LithpFrame::Procedure;
				frame.subframe = new LithpFrame(body, new_env);
				// don't move exp_it
				return false;
			}
			default:
				throw new RuntimeException("Unknown proc type");
			}
		}

		bool LithpDispatcher<instruction::Receive>::dispatch(LithpFrame &frame, const LithpCells::const_iterator &args) {
			std::cout << "receive!? lol\n";
			frame.result = LithpCell(Atom, "lol");
			return true;
		}

		bool LithpFrame::dispatchCall() {
			LithpCells::const_iterator it = resolved_arguments.cbegin();
			instruction::instruction ins = LithpInstructionConverter::convert(exp);
			switch (ins) {
			case instruction::If:
				return LithpDispatcher<instruction::If>::dispatch(*this, it);
			case instruction::Begin:
				return LithpDispatcher<instruction::Begin>::dispatch(*this, it);
			case instruction::Get:
				return LithpDispatcher<instruction::Get>::dispatch(*this, it);
			case instruction::Set:
				return LithpDispatcher<instruction::Set>::dispatch(*this, it);
			case instruction::Define:
				return LithpDispatcher<instruction::Define>::dispatch(*this, it);
			case instruction::Defined:
				return LithpDispatcher<instruction::Defined>::dispatch(*this, it);
			case instruction::Proc:
				return LithpDispatcher<instruction::Proc>::dispatch(*this, it);
			case instruction::Receive:
				return LithpDispatcher<instruction::Receive>::dispatch(*this, it);
			default:
				return LithpDispatcher<instruction::Invalid>::dispatch(*this, it);
			}
		}

		// Thread implementation
		LithpThreadManager LithpThreadMan;
		LithpCell eval_thread(LithpThreadManager &tm, const LithpCell &ins, Env_p env) {
			// create thread
			ThreadId thread = tm.start([&ins, env](MicrothreadBase *thread_ptr) {
				LithpThreadManager::impl_p impl(new LithpImplementation(thread_ptr, ins, env));
				return impl;
			});
			// execute multithreading until thread resolved
			tm.runThreadToCompletion(thread);
			// return frame result
			LithpCell result(tm.getThread(thread).getResult());
			// remove thread
			tm.remove_thread(thread);
			return result;
		}
	}

	#define INDENT()  std::string((depth * 2), ' ')
	// Eval inner-most loop that performs the interpretation.
	LithpCell StacklessInterpreter::eval_inner(LithpCell x, Env_p env) {
		// This could be more efficient
		return Stackless::eval_thread(Stackless::LithpThreadMan, x, env);
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
