#pragma once

#include "stdafx.h"
#include <Stackless.hpp>

namespace PocoLithp {
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
				Invalid
			};
		}

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
				subframe_mode(None),
				arg_id(0)
			{
			}
		public:
			LithpFrame(const LithpCell &expression, env_p environment)
				: LithpFrame(environment) {
				exp = expression;
				expressions.push_back(exp);
				exp_it = expressions.cbegin();
				setExpression(exp);
			}

			enum SubframeMode {
				None,
				Argument,
				Procedure
			};

			bool isResolved() const { return resolved; }
			// TODO: no longer used
			bool isArgumentsResolved() const { return true; }

			void execute();
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

		struct LithpImplementation : public Implementation<LithpEnvironment, LithpFrame> {
			LithpImplementation(const LithpCell &ins, env_p _env)
				: Implementation(_env), frame(ins, _env) {
			}
			LithpFrame &getCurrentFrame() {
				return frame;
			}
			void executeFrame(LithpFrame &fr) {
				fr.execute();
			}
		private:
			LithpFrame frame;
		};

		using namespace stackless::microthreading;
		struct LithpThreadManager : public MicrothreadManager<LithpImplementation> {
		};
		extern LithpThreadManager LithpThreadMan;
	}
}
