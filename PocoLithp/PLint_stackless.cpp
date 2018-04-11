#include "stdafx.h"
#include "PLint_stackless.hpp"
#include <Stackless.hpp>

using namespace stackless;
using namespace stackless::microthreading;
using namespace stackless::timekeeping;

#define DEBUG(m)   if(GetDEBUG()) std::cerr << "! " << m << std::endl;

namespace PocoLithp {
	namespace Stackless {
		const bool msg_debug = false;
		LithpProcessManager LithpProcessMan;

		bool LithpImplementation::deliver_message(const _cell_type &message) {
			return frame.deliver_message(message, *this);
		}

		// Frame implementation
		bool LithpFrame::execute(const LithpImplementation &impl) {
			if (wait_state == Initialize) {
				wait_state = Run;
				expressions.push_back(exp);
				exp_it = expressions.cbegin();
				setExpression(exp, impl);
				return true;
			}
			if (isResolved())
				return true;
			if (isWaiting())
				return false;
			if (subframe != nullptr) {
				const bool executed = subframe->execute(impl);
				if (subframe->isResolved()) {
					// Copy results
					const LithpCell res(subframe->result);
					const auto mode = subframe_mode;
					const bool was_macro = subframe->isMacro();
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
						nextArgument(impl);
						break;
					case Procedure:
						result = res;
						if (was_macro) {
							DEBUG("  executing macro result");
							setExpression(result, impl);
							return true;
						}
						nextExpression(impl);
						break;
					default:
						throw new RuntimeException("Invalid subframe mode None");
					}
				}
				return executed;
			} else if (arg_it == arguments.cend())
				dispatch(impl);
			else
				nextArgument(impl);
			return true;
		}

		void LithpFrame::nextArgument(const LithpImplementation &impl) {
			if (arg_it == arguments.cend())
				dispatch(impl);
			else if (resolveArgument(*arg_it, arg_id)) {
				++arg_it;
				++arg_id;
				nextArgument(impl);
			}
		}

		void LithpFrame::nextExpression(const LithpImplementation &impl) {
			if (exp_it != expressions.cend()) {
				++exp_it;
				if (exp_it != expressions.cend()) {
					setExpression(*exp_it, impl);
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

		void LithpFrame::setExpression(const LithpCell &value, const LithpImplementation &impl) {
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
			nextArgument(impl);
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
				DEBUG(std::string("  resolveArgument(") + to_string(value) + std::string(") = ") + to_string(value) + std::string(", tag: ") + std::to_string(value.tag));
				resolved_arguments.push_back(value);
				return true;
			}
		}

		bool LithpFrame::resolveExpression(const LithpCell &value) {
			switch (value.tag) {
			case VariableReference:
				result = lookup(value);
				return true;
			case Var:
			case Atom:
			case Thread:
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
						result = LithpCell(*it);
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
					} else if (first == sym_macro) { // (macro (var*) exp)
						result = LithpCell(value);
						result.tag = Macro;
						result.env = env;
						return true;
					} else if (first == sym_begin) {  // (begin exp*)
						ARG("begin: requires an expression");
						resolved_arguments = LithpCells(begin + 1, end);
						return false;
					} else if (first == sym_receive) {
						// (receive (Callback Message) [after Milliseconds Callback])
						// arguments = LithpCells(begin + 1, end);
						ARG("receive: requires a callback");
						// Handler::(Callback Message)
						arguments.push_back(*it); ++it;
						if (it != end) {
							// TimeoutMode::infinity | after
							arguments.push_back(*it); ++it;
						} else {
							arguments.push_back(sym_infinity);
						}
						if (it != end) {
							// Timeout::integer
							arguments.push_back(*it); ++it;
						} else {
							arguments.push_back(LithpCell(Var, 0));
						}
						if (it != end) {
							arguments.push_back(*it); ++it;
						} else {
							arguments.push_back(LithpCell(sym_nil));
						}
						// Final order once evaluation complete:
						//   Handler TimeoutHandler TimeoutMode Timeout
						return false;
					} else if (first == sym_sleep) { // (sleep [infinity | Milliseconds])
						if (it != end) {
							arguments.push_back(*it);
							return false;
						} else {
							resolved_arguments.push_back(sym_infinity);
							return true;
						}
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

		bool LithpFrame::deliver_message(const _cell_type &message, const LithpImplementation &impl) {
			if (subframe != nullptr) {
				// Forward to deepest frame
				return deepestFrame().deliver_message(message, impl);
			}
			if (GetDEBUG())
				std::cerr << "!> deliver_message(" << to_string(message) << ") to " << impl.str() << std::endl;
			// We are deepest frame, check receive state
			if (receive_state.isActive() == true || receive_state.on_message != sym_nil) {
				// Wake thread as we have a valid on_message handler.
				// The Receive instruction will run again, and successfully pull a
				// message.
				LithpProcessMan.thread_wake(LithpThreadReference(impl));
			}
			// Always post message to mailbox
			return false;
		}

		// Dispatcher implementation
		bool LithpDispatcher<instruction::If>::dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl) {
			LithpCells::const_iterator it(args);
			const LithpCell &conseq = *it; ++it;
			const LithpCell &alt = *it; ++it;
			const LithpCell &test = *it; ++it;
			const LithpCell &if_result = (test == sym_true) ? conseq : alt;
			frame.setExpression(LithpCell(if_result), impl);
			// Don't move exp_it
			return false;
		}

		bool LithpDispatcher<instruction::Begin>::dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl) {
			// Update expressions to call list
			frame.expressions.swap(frame.resolved_arguments);
			frame.resolved_arguments.clear();
			frame.exp_it = frame.expressions.cbegin();
			frame.setExpression(*frame.exp_it, impl);
			// Don't move exp_it
			return false;
		}

		bool LithpDispatcher<instruction::Get>::dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl) {
			LithpCells::const_iterator it(args);
			const LithpCell &var = *it; ++it;
			frame.result = frame.lookup(var);
			return true;
		}

		bool LithpDispatcher<instruction::Set>::dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl) {
			LithpCells::const_iterator it(args);
			const LithpCell &var = *it; ++it;
			const LithpCell &val = *it; ++it;
			frame.env->operator[](var.atomid()) = frame.result = val;
			return true;
		}

		bool LithpDispatcher<instruction::Define>::dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl) {
			LithpCells::const_iterator it(args);
			const LithpCell &var = *it; ++it;
			const LithpCell &val = *it; ++it;
			frame.env->operator[](var.atomid()) = frame.result = val;
			return true;
		}

		bool LithpDispatcher<instruction::Defined>::dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl) {
			LithpCells::const_iterator it(args);
			const LithpCell &var = *it; ++it;
			frame.result = frame.env->defined(var.atomid()) ? sym_true : sym_false;
			return true;
		}

		bool LithpDispatcher<instruction::Proc>::dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl) {
			LithpCells::const_iterator it(args);

			// TODO: This is a hack to allow atoms to be returned as is
			if (it == frame.resolved_arguments.cend()) {
				frame.result = LithpCell(frame.exp);
				return true;
			}

			LithpCell proc(*it); ++it;
			LithpCells arguments(it, frame.resolved_arguments.cend());

			DEBUG("Secondary proc evaluation of: " + proc.str());
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

			DEBUG("Primary proc evaluation of: " + proc.str());
			// Primary proc evaluation
			switch (proc.tag) {
			case Proc:               // a builtin procedure in C++ that needs no environment.
			case ProcExtended:       // a builtin procedure in C++ that passes the frame environment.
			case ProcImplementation: // a builtin procedure in C++ that passes the implementation itself.
				// Mark wait state to avoid re-entry into this section
				frame.wait_state = Run_Wait;
				// Copy the rest of the resolved arguments to a new list,
				// that list is passed as the arguments.
				if(proc.tag == Proc) frame.result = proc.proc()(arguments);
				else if(proc.tag == ProcExtended) frame.result = proc.proc_extended()(arguments, frame.env);
				else if(proc.tag == ProcImplementation) frame.result = proc.proc_implementation()(arguments, frame.env, impl);
				// Restore wait_state
				frame.wait_state = Run;
				// Move exp_it
				return true;
			case Lambda:  // A Lisp procedure
			case Macro:   // or macro.
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
				if (frame.subframe_mode == LithpFrame::Procedure && proc.tag == Lambda && frame.isAtEnd()) {
					std::cerr << "@ tail recurse!" << std::endl;
					frame.env = new_env;
					frame.exp = body;
					frame.subframe_mode = LithpFrame::Procedure;
					frame.wait_state = Initialize;
				} else {
					// create subframe
					std::cerr << "@! head recurse" << std::endl;
					frame.subframe_mode = LithpFrame::Procedure;
					frame.subframe = new LithpFrame(body, new_env);
					frame.subframe->setMacro(proc.tag == Macro);
				}
				// don't move exp_it
				return false;
			}
			default:
				throw new RuntimeException("Unknown proc type");
			}
		}

		bool LithpDispatcher<instruction::Receive>::dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl) {
			// Gets called with arguments:
			//   OnMessage OnTimeout TimeoutMode TimeoutValue
			// TODO: This line is a hack:
			LithpCells::const_iterator end = frame.resolved_arguments.cend();
			LithpCells::const_iterator it(args);

			//                        // On_Message::func(Message::any())
			const LithpCell &on_message = *it; ++it;
			LithpCell timeout_mode = *it; ++it;
			LithpCell timeout = *it; ++it;
			LithpCell on_timeout = *it; ++it;
			ThreadTimeUnit timeout_in_units(0);

			if (timeout_mode != sym_infinity) {      // Timeout::integer()
				timeout_in_units = ThreadTimeUnit(timeout.value.convert<UnsignedInteger>());
			}

			const LithpThreadReference thread_ref(impl);
			if (msg_debug || GetDEBUG())
				std::cerr << "@ attempt receive on " << thread_ref.str() << std::endl;

			LithpCell message;
			if (LithpProcessMan.receive(message, thread_ref)) {
				frame.receive_state.clear();
				// Successfully pulled a message from queue.
				// Construct new expression:
				//   (OnMessage (quote Message))
				LithpCells ins;
				ins.push_back(on_message);
				LithpCells ins_quote;
				ins_quote.push_back(sym_quote);
				ins_quote.push_back(message);
				ins.push_back(LithpCell(List, ins_quote));
				if (msg_debug || GetDEBUG())
					std::cerr << "@ receive got message: " << message.str() << std::endl;
				frame.receive_state.clear();
				frame.setExpression(LithpCell(List, ins), impl);
				return false; // don't move exp_it
			}

			// No message available and/or thread has woken again but no message available.
			if (frame.receive_state.isActive()) {
				// We have a current receive state, meaning we've been here
				// before. At this point we've been woken and should do timeout
				// behaviour.
				if (frame.receive_state.hasTimeoutHandler()) {
					// If we have a timeout handler, run it.
					frame.result = getAtom("handling_timeout");
					// Construct new expression:
					//   (OnTimeout)
					LithpCells ins;
					ins.push_back(on_timeout);
					frame.setExpression(LithpCell(List, ins), impl);
					if (msg_debug || GetDEBUG())
						std::cerr << "@ handling timeout with: " << to_string(on_timeout) << std::endl;
					frame.receive_state.clear();
					return false; // don't move exp_it
				} else {
					// Otherwise timeout occurred and no handler
					frame.result = getAtom("timeout");
					if (msg_debug || GetDEBUG())
						std::cerr << "@ timeout occurred, no handler" << std::endl;
					frame.receive_state.clear();
					return true;  // move exp_it
				}
			}

			// No receive state, save it and sleep
			frame.receive_state = ReceiveState(on_message, on_timeout, timeout);

			// Put this frame to sleep for specified duration, or forever
			// if that was specified (or no duration specified.)
			if (timeout_mode == sym_infinity) {
				if (msg_debug || GetDEBUG())
					std::cerr << "@ no message, sleeping forever" << std::endl;
				LithpProcessMan.thread_sleep_forever(thread_ref);
			} else {
				if (msg_debug || GetDEBUG())
					std::cerr << "@ no message, sleeping for " << timeout.str() << std::endl;
				LithpProcessMan.thread_sleep_for(thread_ref, timeout_in_units);
			}
			frame.result = getAtom("sleeping");
			return false; // don't move exp_it
		}

		bool LithpDispatcher<instruction::Sleep>::dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl) {
			// TODO: This line is a hack:
			LithpCells::const_iterator end = frame.resolved_arguments.cend();
			LithpCells::const_iterator it(args);
			LithpCell timeout = sym_infinity;
			if (it != end) {
				timeout = *it; ++it;
			}

			if (impl.isAsleep())
				return false;

			LithpThreadReference thread_ref(impl);
			if (timeout == sym_infinity) {
				LithpProcessMan.thread_sleep_forever(thread_ref);
			} else {
				ThreadTimeUnit duration = (ThreadTimeUnit)timeout.value.convert<UnsignedInteger>();
				LithpProcessMan.thread_sleep_for(thread_ref, duration);
			}
			if (msg_debug || GetDEBUG()) std::cerr << "! sleeping" << std::endl;
			frame.result = LithpCell(Atom, "sleeping");
			// Don't move exp_it - it will be moved by implementation instead
			return false;
		}

		bool LithpFrame::dispatchCall(const LithpImplementation &impl) {
			LithpCells::const_iterator it = resolved_arguments.cbegin();
			instruction::instruction ins = LithpInstructionConverter::convert(exp);
			switch (ins) {
			case instruction::If:
				return LithpDispatcher<instruction::If>::dispatch(*this, it, impl);
			case instruction::Begin:
				return LithpDispatcher<instruction::Begin>::dispatch(*this, it, impl);
			case instruction::Get:
				return LithpDispatcher<instruction::Get>::dispatch(*this, it, impl);
			case instruction::Set:
				return LithpDispatcher<instruction::Set>::dispatch(*this, it, impl);
			case instruction::Define:
				return LithpDispatcher<instruction::Define>::dispatch(*this, it, impl);
			case instruction::Defined:
				return LithpDispatcher<instruction::Defined>::dispatch(*this, it, impl);
			case instruction::Proc:
				return LithpDispatcher<instruction::Proc>::dispatch(*this, it, impl);
			case instruction::Receive:
				return LithpDispatcher<instruction::Receive>::dispatch(*this, it, impl);
			case instruction::Sleep:
				return LithpDispatcher<instruction::Sleep>::dispatch(*this, it, impl);
			default:
				return LithpDispatcher<instruction::Invalid>::dispatch(*this, it, impl);
			}
		}

		// Thread implementation
		LithpCell eval_thread(const LithpCell &ins, Env_p env) {
			// create thread
			LithpThreadReference thread = LithpProcessMan.start(ins, env);
			try {
				// execute multithreading until thread resolved
				LithpProcessMan.runThreadToCompletion(thread);
				// return frame result
				LithpCell result(LithpProcessMan.getResult(thread));
				// remove thread
				LithpProcessMan.remove_thread(thread);
				return result;
			} catch (...) {
				// remove thread
				LithpProcessMan.remove_thread(thread);
				throw;
			}
		}
	}

	#define INDENT()  std::string((depth * 2), ' ')
	// Eval inner-most loop that performs the interpretation.
	LithpCell StacklessInterpreter::eval_inner(LithpCell x, Env_p env) {
		// This could be more efficient
		return Stackless::eval_thread(x, env);
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
