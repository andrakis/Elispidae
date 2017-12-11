#pragma once

#include "stdafx.h"
#include <Stackless.hpp>
#include <queue>

namespace PocoLithp {
	typedef std::queue<LithpCell> LithpMailbox;

	class StacklessInterpreter : public Interpreter {
	public:
		StacklessInterpreter() : Interpreter() { }
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

		static Interpreter *Instantiate() {
			return new StacklessInterpreter();
		}
	};

	namespace Stackless {
		using namespace stackless;
		namespace instruction {
			enum instruction {
				Quote,
				If,
				Set,
				Get,
				Define,
				Defined,
				Begin,
				Proc, ProcExtended, Lambda, Macro,
				Receive,
				Invalid
			};
		}

		extern const LithpCell sym_receive;
		struct LithpInstructionConverter
			: public InstructionConverter<LithpCell, typename instruction::instruction> {
			static _instruction_type convert(_cell_type value) {
				switch (value.tag) {
				case Atom:
					if (value == sym_quote) return instruction::Quote;
					if (value == sym_if) return instruction::If;
					if (value == sym_begin) return instruction::Begin;
					if (value == sym_get) return instruction::Get;
					if (value == sym_set) return instruction::Set;
					if (value == sym_define) return instruction::Define;
					if (value == sym_defined) return instruction::Defined;
					if (value == sym_lambda || value == sym_lambda2) return instruction::Lambda;
					if (value == sym_macro) return instruction::Macro;
					if (value == sym_receive) return instruction::Receive;
					// Fall through
				case List:
				case Lambda:
				case Proc:
				case ProcExtended:
				case Macro:
				case Var:
				case VariableReference:
					return instruction::Proc;
				default:
					return instruction::Invalid;
				}
			}
		};

		enum FrameWaitState {
			Initialize,
			Run,
			Done,
			Receive
		};

		// When in a message receive state
		struct ReceiveState {
			LithpCell on_message;  // (# (Message::any) :: any) callback on message receive
			LithpCell on_timeout;  // (# () :: any) callback on timeout
			LithpCell timeout;     // infinite, or number of milliseconds
		};

		struct LithpImplementation;
		// Frame implementation
		struct LithpFrame : public Frame<LithpCell, typename instruction::instruction, LithpEnvironment> {
		private:
			LithpFrame(env_p environment)
				: Frame(environment),
				wait_state(Initialize),
				exp(sym_nil),
				expressions(),
				arguments(),
				resolved_arguments(),
				exp_it(expressions.cbegin()),
				arg_it(arguments.cbegin()),
				resolved(false),
				subframe(nullptr),
				subframe_mode(None),
				arg_id(0)
			{
			}
		public:
			LithpFrame(const LithpCell &expression, env_p environment)
				: LithpFrame(environment) {
				exp = expression;
			}

			enum SubframeMode {
				None,
				Argument,
				Procedure
			};

			bool isResolved() const { return resolved; }
			bool isWaiting() const { return wait_state == Receive; }

			// TODO: no longer used
			bool isArgumentsResolved() const { return true; }

			void execute(LithpImplementation &);
			void nextArgument();
			void nextExpression();
			void setExpression(const LithpCell &value);
			bool resolveArgument(const LithpCell &value, const unsigned &id);
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

			bool resolveExpression(LithpCell &value);

			FrameWaitState wait_state;
			LithpCell exp;
			LithpCells expressions;
			LithpCells arguments;
			LithpCells resolved_arguments;
			typename LithpCells::const_iterator exp_it;
			typename LithpCells::const_iterator arg_it;
			bool resolved;
			LithpFrame *subframe;
			SubframeMode subframe_mode;
			// TODO: This is a bit of a hack to support return type parsing
			unsigned arg_id;
		};

		template<typename instruction::instruction Instruction>
		struct LithpDispatcher {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args) {
				throw new RuntimeException("Instruction not implemented");
			}
		};

		template<> struct LithpDispatcher<instruction::If> {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args);
		};

		template<> struct LithpDispatcher<instruction::Begin> {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args);
		};

		template<> struct LithpDispatcher<instruction::Get> {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args);
		};

		template<> struct LithpDispatcher<instruction::Set> {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args);
		};

		template<> struct LithpDispatcher<instruction::Define> {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args);
		};

		template<> struct LithpDispatcher<instruction::Defined> {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args);
		};

		template<> struct LithpDispatcher<instruction::Proc> {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args);
		};
		template<> struct LithpDispatcher<instruction::Receive> {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args);
		};

		struct LithpImplementation : public Implementation<LithpEnvironment, LithpFrame> {
			LithpImplementation(MicrothreadBase *thread_ptr, const LithpCell &ins, env_p _env)
				: Implementation(_env), thread(thread_ptr), frame(ins, _env) {
			}
			LithpFrame &getCurrentFrame() {
				return frame;
			}
			void executeFrame(LithpFrame &fr) {
				fr.execute(*this);
			}
			MicrothreadBase *thread;
		private:
			LithpFrame frame;
		};

		using namespace stackless::microthreading;
		struct LithpThreadManager : public MicrothreadManager<LithpImplementation> {
			typedef std::vector<_thread_type> Threads;
			Threads getThreads() {
				Threads list;
				for (auto it = threads.begin(); it != threads.end(); ++it)
					list.push_back(it->second);
				return list;
			}
		};
		extern LithpThreadManager LithpThreadMan;
	}
}
