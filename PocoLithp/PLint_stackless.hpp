#pragma once

#include "stdafx.h"
#include <Stackless.hpp>
#include <queue>

// For avoiding 100% cpu usage
#include <chrono>
#include <mutex>
#include <thread>

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
			// Frame startup
			Initialize,
			// Standard run
			Run,
			// Running but awaiting on a proc to finish
			Run_Wait,
			// Finished
			Done,
			// Awaiting message
			Receive,
			// Special state
			REPL
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

			LithpFrame &deepestFrame() {
				if (subframe != nullptr)
					return subframe->deepestFrame();
				return *this;
			}

			bool isResolved() const { return resolved; }
			bool isWaiting() const { return wait_state == Receive || wait_state == REPL || wait_state == Run_Wait; }

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
			typedef std::vector<_thread_type> Threads;
			// TODO: This should be part of the thread, but we don't have access to it
			typedef std::map<ThreadId, LithpMailbox> Mailboxes;

			LithpThreadManager() : MicrothreadManager() {
			}

			Threads getThreads() {
				Threads list;
				for (auto it = threads.begin(); it != threads.end(); ++it)
					list.push_back(it->second);
				return list;
			}

			_threads_type::iterator getThreadById(const LithpThreadId thread_id, const LithpThreadNode node_id = 0, const LithpCosmosNode cosmos_id = 0) {
				// TODO: Only thread_id is used so far
				return threads.find(thread_id);
			}
			_threads_type::iterator getThreadById(const LithpThreadReference &ref) {
				return getThreadById(ref.thread_id, ref.node_id, ref.cosmos_id);
			}

			Mailboxes::iterator getMailboxById(const LithpThreadId thread_id, const LithpThreadNode node_id = 0, const LithpCosmosNode cosmos_id = 0) {
				Mailboxes::iterator mailbox_it = mailboxes.find(thread_id);
				if (mailbox_it == mailboxes.end()) {
					// Mailbox not found, create it
					mailboxes.emplace(thread_id, LithpMailbox());
					mailbox_it = mailboxes.find(thread_id);
				}
				return mailbox_it;
			}
			Mailboxes::iterator getMailboxById(const LithpThreadReference &ref) {
				return getMailboxById(ref.thread_id, ref.node_id, ref.cosmos_id);
			}

			// Send a message to a thread.
			// Returns: true on success, false on thread not existing.
			bool send(const LithpCell &message, const LithpThreadReference &ref) {
				auto thread_it = getThreadById(ref);
				if (thread_it == threads.end())
					return false; // Thread not found
				auto mailbox_it = getMailboxById(ref);
				mailbox_it->second.push(LithpCell(message));
				return true;
			}
			
			// Attempt to receive a message for a thread.
			// Returns: true on message received and copied into &message, false if nothing available.
			bool receive(LithpCell &message, const LithpThreadReference &ref) {
				auto thread_it = getThreadById(ref);
				if (thread_it == threads.end())
					return false; // No thread with this id
				auto mailbox_it = getMailboxById(ref);
				if (mailbox_it->second.empty())
					return false; // No messages waiting
				// Update message object
				message.update(mailbox_it->second.front());
				mailbox_it->second.pop();
				return true;
			}
		protected:
			void yield_process() {
				for(;;) {
					if (!yield_mutex.try_lock()) {
						std::this_thread::yield();
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
						continue;
					}
					break;
				}
			}
		private:
			Mailboxes mailboxes;
			std::mutex yield_mutex;
		};
		extern LithpThreadManager LithpThreadMan;
	}
}
