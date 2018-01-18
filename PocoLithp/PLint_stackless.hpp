#pragma once

#include "stdafx.h"
#include <Stackless.hpp>

// For avoiding 100% cpu usage
#include <chrono>
#include <mutex>
#include <thread>

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
		using namespace stackless::microthreading;
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
				Sleep,
				Invalid
			};
		}

		typedef std::vector<LithpThreadReference> LithpThreadReferenceList;

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
					if (value == sym_sleep) return instruction::Sleep;
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

			ReceiveState() :
				on_message(sym_nil), on_timeout(sym_nil), timeout(sym_nil)
			{
			}

			ReceiveState(const LithpCell &_onmessage,
				         const LithpCell &_ontimeout,
				         const LithpCell &_timeout = sym_infinity) :
				on_message(_onmessage), on_timeout(_ontimeout),
				timeout(_timeout) {
			}

			bool isActive() const {
				return on_message == sym_nil &&
					on_timeout == sym_nil &&
					timeout == sym_nil;
			}
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
				arg_id(0),
				receive_state(),
				is_macro(false),
				is_sleeping(false)
			{
			}
		public:
			LithpFrame(const LithpCell &expression, env_p environment)
				: LithpFrame(environment) {
				exp = expression;
			}

			void setMacro(bool macro) {
				is_macro = macro;
			}

			bool isMacro() const {
				return is_macro;
			}

			enum SubframeMode {
				None,
				Argument,
				Procedure,
			};

			LithpFrame &deepestFrame() {
				if (subframe != nullptr)
					return subframe->deepestFrame();
				return *this;
			}

			bool deliver_message(const _cell_type &message, const LithpImplementation &impl);

			bool isResolved() const { return resolved; }
			bool isWaiting(const LithpImplementation &impl);// { return wait_state == REPL || wait_state == Run_Wait; }

			// TODO: no longer used
			bool isArgumentsResolved() const { return true; }

			void execute(const LithpImplementation &impl);
			void nextArgument(const LithpImplementation &impl);
			void nextExpression(const LithpImplementation &impl);
			void setExpression(const LithpCell &value, const LithpImplementation &impl);
			bool resolveArgument(const LithpCell &value, const unsigned &id);
			LithpCell &lookup(const LithpCell &symbol) {
				return env->find(symbol.atomid())[symbol.atomid()];
			}

			bool dispatchCall(const LithpImplementation &impl);
			void dispatch(const LithpImplementation &impl) {
				if (dispatchCall(impl))
					nextExpression(impl);
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
			ReceiveState receive_state;
			bool is_macro;
			bool is_sleeping;
		};

		template<typename instruction::instruction Instruction>
		struct LithpDispatcher {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl) {
				throw new RuntimeException("Instruction not implemented");
			}
		};

		template<> struct LithpDispatcher<instruction::If> {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl);
		};

		template<> struct LithpDispatcher<instruction::Begin> {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl);
		};

		template<> struct LithpDispatcher<instruction::Get> {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl);
		};

		template<> struct LithpDispatcher<instruction::Set> {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl);
		};

		template<> struct LithpDispatcher<instruction::Define> {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl);
		};

		template<> struct LithpDispatcher<instruction::Defined> {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl);
		};

		template<> struct LithpDispatcher<instruction::Proc> {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl);
		};
		template<> struct LithpDispatcher<instruction::Receive> {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl);
		};
		template<> struct LithpDispatcher<instruction::Sleep> {
			static bool dispatch(LithpFrame &frame, const LithpCells::const_iterator &args, const LithpImplementation &impl);
		};

		struct LithpImplementationInfo {
			const LithpCell ins;
			const LithpThreadId thread_id;
			const LithpThreadNodeId node_id;
			const LithpCosmosNodeId cosmos_id;

			LithpImplementationInfo(const LithpCell &_ins, const LithpThreadId _thread_id,
				const LithpThreadNodeId _node_id, const LithpCosmosNodeId _cosmos_id)
				: ins(_ins), thread_id(_thread_id), node_id(_node_id), cosmos_id(_cosmos_id)
			{
			}

			LithpImplementationInfo(const LithpCell &_ins, const LithpThreadReference &ref)
				: ins(_ins), thread_id(ref.thread_id), node_id(ref.node_id), cosmos_id(ref.cosmos_id)
			{
			}
		};
		struct LithpImplementation : public Implementation<LithpEnvironment, LithpFrame> {
			const LithpThreadId thread_id;
			const LithpThreadNodeId node_id;
			const LithpCosmosNodeId cosmos_id;

			LithpImplementation(const LithpImplementationInfo &info, env_p _env)
				: Implementation(_env), thread_id(info.thread_id), node_id(info.node_id),
				cosmos_id(info.cosmos_id), frame(info.ins, _env)
			{
			}
			LithpFrame &getCurrentFrame() {
				return frame;
			}
			void executeFrame(LithpFrame &fr) {
				fr.execute(*this);
			}

			bool deliver_message(const _cell_type &message) {
				return frame.deliver_message(message, *this);
			}
		private:
			LithpFrame frame;
		};

		using namespace stackless::microthreading;

		struct LithpThreadManager : protected MicrothreadManager<LithpImplementation> {
			const LithpThreadNodeId node_id;
			const LithpCosmosNodeId cosmos_id;

			LithpThreadManager(const LithpThreadNodeId _node_id, const LithpCosmosNodeId _cosmos_id)
				: MicrothreadManager(), node_id(_node_id), cosmos_id(_cosmos_id) {
			}

			_threads_type::iterator getThreadById(const LithpThreadId thread_id, const LithpThreadNodeId node_id = 0, const LithpCosmosNodeId cosmos_id = 0) {
				// TODO: Only thread_id is used so far
				return threads.find(thread_id);
			}
			_threads_type::iterator getThreadById(const LithpThreadReference &ref) {
				return getThreadById(ref.thread_id, ref.node_id, ref.cosmos_id);
			}

			// Attempt to receive a message for a thread.
			// Returns: true on message received and copied into &message, false if nothing available.
			bool receive(LithpCell &message, const ThreadId thread_id) {
				typename _threads_type::iterator thread = getThread(thread_id);
				if (thread == threads.end())
					return false; // Not found
				if(thread->second.mailbox.empty())
					return false; // No messages waiting
				// Update message object
				message.update(thread->second.mailbox.front());
				thread->second.mailbox.pop();
				return true;
			}
			// Sleep for duration from current time
			void thread_sleep(const LithpThreadReference &thread_ref, const ThreadTimeUnit &duration) {
				const ThreadId id = thread_ref.thread_id;
				thread_sleep_for(id, duration);
			}

			LithpThreadReference start(const LithpCell &ins, Env_p env) {
				ThreadId thread_id = thread_counter;
				LithpThreadReference thread_ref(thread_id, node_id, cosmos_id);
				LithpImplementationInfo info(ins, thread_ref);
				MicrothreadManager::start([&info, env]() {
					LithpThreadManager::impl_p impl(new LithpImplementation(info, env));
					return impl;
				});
				return thread_ref;
			}

			LithpCells getThreadReferences() const {
				LithpCells refs;
				for (auto it = threads.cbegin(); it != threads.cend(); ++it)
					refs.push_back(LithpCell(Thread, LithpThreadReference(it->first, node_id, cosmos_id)));
				return refs;
			}

			void runThreadToCompletion(const LithpThreadReference &ref) {
				MicrothreadManager::runThreadToCompletion(ref.thread_id, Multi);
			}
			using MicrothreadManager::_thread_type;
			typename _threads_type::iterator getThread(const LithpThreadReference &ref) {
				return MicrothreadManager::getThread(ref.thread_id);
			}
			void remove_thread(const LithpThreadReference &ref) {
				MicrothreadManager::remove_thread(ref.thread_id);
			}

			using MicrothreadManager::send;

			// TODO: This needs to be removed
			using MicrothreadManager::getCurrentThread;
			// TODO: This needs to be removed
			using MicrothreadManager::thread_sleep_forever;
			using MicrothreadManager::thread_sleep_for;
			using MicrothreadManager::threadCount;
			using MicrothreadManager::hasThreads;
			using MicrothreadManager::executeThreads;

			bool isThreadSleeping(const LithpThreadReference &thread_ref) {
				auto thread_it = getThread(thread_ref);
				if (thread_it == threads.end())
					return false;
				return false == isThreadScheduled(thread_it);
			}

		protected:
			void yield_process(bool unwatched_resolved, int threads_run) {
				if (threads_run == 0) {
					// Default to 1ms sleep time
					std::chrono::milliseconds time(1);
					// Unless there is scheduling information
					if (scheduling.empty() == false) {
						// Set time to the next thread timeout
						const SchedulingInformation &next = *scheduling.cbegin();
						ThreadTimePoint now = ThreadClock::now();
						ThreadTimePoint target = next.time_point;
						time = std::chrono::duration_cast<ThreadTimeUnit>(target - now);
						if (time <= std::chrono::milliseconds(0))
							return;
					}
						// Acquire lock for sleeping
						//if (!yield_mutex.try_lock()) {
							// Expensive sleep
							std::this_thread::yield();
							if (GetDEBUG())
								std::cerr << "! Sleeping for " << time.count() << "ms" << std::endl;
							std::this_thread::sleep_for(time);
						//}
				}
			}
			void deliver_message(_threads_iterator thread, const _cell_type &message) {
				if (thread->second.impl->deliver_message(message))
					return; // Message delivered
				// Message queued
				thread->second.mailbox.push(message);
				if (GetDEBUG())
					std::cerr << "! thread mailbox: " << thread->second.mailbox.size() << " in size" << std::endl;
			}
		private:
			//Mailboxes mailboxes;
			std::mutex yield_mutex;
		};

		class LithpCosmosNode;
		typedef std::shared_ptr<LithpThreadManager> LithpThreadManagerPtr;
		typedef std::shared_ptr<LithpCosmosNode> LithpCosmosNodePtr;

		class LithpCosmosNode {
		public:
			struct LithpThreadNode {
				// Thread node 
				LithpThreadManagerPtr ptr;
				// CPU mask
				unsigned mask;
				LithpThreadNode(LithpCosmosNodeId _cosmos_id, LithpThreadNodeId _id)
					: ptr(new LithpThreadManager(_id, _cosmos_id)), node_id(_id), cosmos_id(_cosmos_id)
				{
				}
				const LithpThreadNodeId node_id;
				const LithpCosmosNodeId cosmos_id;
			};
			typedef std::map<LithpThreadNodeId,LithpThreadNode> LithpThreadNodeMap;
			LithpThreadNodeMap thread_nodes;
			LithpThreadNodeId thread_nodes_counter;
			static const LithpThreadNodeId LocalNodeId = 0;

			LithpCosmosNode(LithpCosmosNodeId _id) : thread_nodes(), thread_nodes_counter(0), cosmos_id(_id) {
				allocateThreadNode();
			}

			LithpThreadNodeId allocateThreadNode() {
				LithpThreadNodeId node_id = thread_nodes_counter++;
				LithpThreadNode node(cosmos_id, node_id);
				thread_nodes.emplace(node_id, node);
				return node_id;
			}

			// TODO: This doesn't support multithreading yet
			LithpThreadManagerPtr GetAnyThreadManager() {
				// TODO: only gets the first element
				return thread_nodes.begin()->second.ptr;
			}

			LithpThreadManagerPtr GetThreadManagerForThread(const LithpThreadReference &thread_ref) {
				return thread_nodes.find(thread_ref.node_id)->second.ptr;
			}

			size_t threadCount() {
				size_t count = 0;
				for (auto it = thread_nodes.begin(); it != thread_nodes.end(); ++it) {
					count += it->second.ptr->threadCount();
				}
				return count;
			}

			bool hasThreads() {
				for (auto it = thread_nodes.begin(); it != thread_nodes.end(); ++it) {
					if (it->second.ptr->hasThreads())
						return true;
				}
				return false;
			}

			int executeThreads() {
				int threads_executed = 0;
				for (auto it = thread_nodes.begin(); it != thread_nodes.end(); ++it) {
					threads_executed += it->second.ptr->executeThreads();
				}
				return threads_executed;
			}

			LithpCells getThreadReferences() {
				LithpCells refs;
				for (auto it = thread_nodes.begin(); it != thread_nodes.end(); ++it) {
					auto thread_refs = it->second.ptr->getThreadReferences();
					refs.insert(refs.end(), thread_refs.cbegin(), thread_refs.cend());
				}
				return refs;
			}

			bool send(const LithpCell &message, const LithpThreadReference &thread_ref) {
				return GetThreadManagerForThread(thread_ref)->send(message, thread_ref.thread_id);
			}

			bool isThreadSleeping(const LithpThreadReference &thread_ref) {
				return GetThreadManagerForThread(thread_ref)->isThreadSleeping(thread_ref);
			}

			const LithpCosmosNodeId cosmos_id;
		};

		class LithpProcessManager {
			typedef std::map<LithpCosmosNodeId, LithpCosmosNodePtr> LithpCosmosNodeMap;
			LithpCosmosNodeMap cosmos_nodes;

			LithpCosmosNodePtr getCosmosNodeFor(const LithpThreadReference &thread_ref) {
				return cosmos_nodes.find(thread_ref.cosmos_id)->second;
			}
			LithpThreadManagerPtr getThreadManagerFor(const LithpThreadReference &thread_ref) {
				return getCosmosNodeFor(thread_ref)->GetThreadManagerForThread(thread_ref);
			}
		public:
			static const LithpCosmosNodeId LocalNodeId = 0;

			LithpProcessManager() : cosmos_nodes() {
				allocateCosmosNode(LocalNodeId);
			}

			void allocateCosmosNode(LithpCosmosNodeId id) {
				LithpCosmosNode *node = new LithpCosmosNode(id);
				cosmos_nodes.emplace(id, LithpCosmosNodePtr(node));
			}

			LithpCosmosNodePtr GetLocalCosmosNode() {
				return cosmos_nodes.find(LocalNodeId)->second;
			}

			void thread_sleep_for(const LithpThreadReference &thread_ref, const ThreadTimeUnit &duration) {
				getThreadManagerFor(thread_ref)->thread_sleep_for(thread_ref.thread_id, duration);
			}
			void thread_sleep_forever(const LithpThreadReference &thread_ref) {
				getThreadManagerFor(thread_ref)->thread_sleep_forever(thread_ref.thread_id);
			}

			// Start a thread on the local cosmos node, in the first available thread manager
			LithpThreadReference start(const LithpCell &ins, Env_p env) {
				LithpCosmosNodePtr cosmos = getCosmosNodeFor(LocalNodeId);
				LithpThreadManagerPtr threadman = cosmos->GetAnyThreadManager();
				return threadman->start(ins, env);
			}

			// Start a thread on the same thread manager as an existing thread
			LithpThreadReference start(const LithpCell &ins, Env_p env, const LithpThreadReference &parent) {
				return getThreadManagerFor(parent)->start(ins, env);
			}

			void runThreadToCompletion(const LithpThreadReference &ref) {
				getThreadManagerFor(ref)->runThreadToCompletion(ref);
			}

			void remove_thread(const LithpThreadReference &ref) {
				getThreadManagerFor(ref)->remove_thread(ref);
			}

			LithpCell getResult(const LithpThreadReference &ref) {
				return getThreadManagerFor(ref)->getThread(ref)->second.getResult();
			}

			size_t threadCount() {
				return GetLocalCosmosNode()->threadCount();
			}

			bool hasThreads() {
				return GetLocalCosmosNode()->hasThreads();
			}

			int executeThreads() {
				return GetLocalCosmosNode()->executeThreads();
			}

			LithpCell getThreadReferences() {
				LithpCells refs;
				for (auto it = cosmos_nodes.begin(); it != cosmos_nodes.end(); ++it) {
					auto thread_refs = it->second->getThreadReferences();
					refs.insert(refs.end(), thread_refs.cbegin(), thread_refs.cend());
				}
				return LithpCell(List, refs);
			}

			bool send(const LithpCell &message, const LithpThreadReference &thread_ref) {
				return getCosmosNodeFor(thread_ref)->send(message, thread_ref);
			}

			bool receive(LithpCell &message, const LithpThreadReference &thread_ref) {
				return getThreadManagerFor(thread_ref)->receive(message, thread_ref.thread_id);
			}

			bool isThreadSleeping(const LithpThreadReference &thread_ref) {
				return getThreadManagerFor(thread_ref)->isThreadSleeping(thread_ref);
			}
		};

		extern LithpProcessManager LithpProcessMan;
	}
}
