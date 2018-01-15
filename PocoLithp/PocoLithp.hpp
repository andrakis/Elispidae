#pragma once

#include "stdafx.h"
#include <Stackless.hpp>		// Ugly hack

#define ELISP_VERSION "0.82"

// Undefine to use recursive emulator
#define ELISP_STACKLESS
#ifdef ELISP_STACKLESS
#define STACKLESS_DESC   " (Stackless)"
#else
#define STACKLESS_DESC   ""
#endif

#ifndef NO_STATS
#define ELISP_TRACK_STATS
#endif

#ifdef ELISP_TRACK_STATS
#define TRACK_STATS(code) do { code; } while(0)
#define STATS_DESC        "(stats enabled)"
#else
#define TRACK_STATS(code) while(0)
#define STATS_DESC        "(no stats)"
#endif

#define APP_NAME "Elispidae "
#define ELISP_VERSION_INFO APP_NAME ELISP_VERSION STACKLESS_DESC

namespace PocoLithp {
	typedef Poco::Dynamic::Var PocoVar;
	typedef std::vector<PocoVar> PocoList;
	typedef std::map<PocoVar, PocoVar> PocoDict;

	typedef Poco::NumberParser NumberParser;

	typedef unsigned AtomType;

	class LithpException : public std::exception {
		std::string reason;
	protected:
		LithpException(const std::string &_reason) : reason(_reason) {
		}
	public:
		const char *what() const throw() {
			return reason.c_str();
		}
	};

	class InvalidArgumentException : public LithpException {
	public:
		InvalidArgumentException() : LithpException("Invalid argument: no reason given") {
		}
		InvalidArgumentException(const std::string &_reason)
			: LithpException("Invalid argument: " + _reason) {
		}
	};

	class SyntaxException : public LithpException {
	public:
		SyntaxException() : LithpException("Syntax argument: no reason given") {
		}
		SyntaxException(const std::string &_reason)
			: LithpException("Syntax error: " + _reason) {
		}
	};

	class RuntimeException : public LithpException {
	public:
		RuntimeException() : LithpException("Runtime exception: no reason given") {
		}
		RuntimeException(const std::string &_reason)
			: LithpException("Runtime exception: " + _reason) {
		}
	};

#if defined(POCO_HAVE_INT64)
#define ELISP_INT64
#endif

#ifdef ELISP_INT64
	typedef Poco::Int64 SignedInteger;
	typedef Poco::UInt64 UnsignedInteger;
	const std::string ELISP_ARCH = "(Int64 support)";
#else
	typedef long SignedInteger;
	typedef unsigned long UnsignedInteger;
	const std::string ELISP_ARCH = "(Int32 only)";
#endif

	enum LithpVarType {
		Var,
		Dict,
		Atom,
		VariableReference,
		Tuple,
		//OpChain, // AKA lambda
		//Closure, // AKA environment

		// scheme types
		List,
		Proc,
		ProcExtended,
		ProcImplementation,
		Lambda,
		Macro,

		// Extended types
		Thread
	};

	struct LithpEnvironment;
	struct LithpVar;
	typedef uint32_t atomId;

	typedef LithpVar LithpCell;
	typedef std::vector<LithpCell> LithpCells;
	typedef LithpCells::const_iterator LithpCellIt;
	typedef std::map<atomId, LithpCell> LithpDict;
	typedef LithpDict::const_iterator LithpDictIt;

	typedef std::map<atomId, std::string> AtomMapById_t;
	typedef std::map<std::string, atomId> AtomMapByName_t;
	std::string getAtomById(const atomId id);
	// Declare an atom to exist and get its id
	atomId getAtomId(const std::string &);
	// Declare an atom to exist and get it as a cell
	LithpCell getAtom(const std::string &);

	typedef std::shared_ptr<LithpEnvironment> Env_p;

	// From PLglobals.cpp
	extern const LithpCell sym_false;
	extern const LithpCell sym_true;
	extern const LithpCell sym_nil;
	extern const LithpCell sym_quote;
	extern const LithpCell sym_if;
	extern const LithpCell sym_get;
	extern const LithpCell sym_set;
	extern const LithpCell sym_define;
	extern const LithpCell sym_defined;
	extern const LithpCell sym_lambda;
	extern const LithpCell sym_lambda2;
	extern const LithpCell sym_macro;
	extern const LithpCell sym_begin;
	extern const LithpCell sym_receive;
	extern const LithpCell sym_sleep;
	extern const LithpCell sym_after;
	extern const LithpCell sym_infinity;
	std::string to_string(const LithpCell &exp);
	std::string to_string(const LithpCell &exp, bool advanced, bool repre);
	const LithpCell booleanCell(const bool val);
	bool booleanVal(const LithpCell &val);
	std::string getAtomById(atomId id);
	atomId getAtomId(const std::string &name);
	LithpCell getAtom(const std::string &name);

	typedef void(*add_environment_proc)(LithpEnvironment &);
	void add_environment_runtime(add_environment_proc);
	void add_globals(LithpEnvironment &env);

	// From PLparser.cpp
	LithpCell read(const std::string &s);
	std::string str(long n);
	bool isdig(char c);
	std::list<std::string> tokenize(const std::string & str);
	PocoVar parseNumber(const std::string &token);
	LithpCell symbol(const std::string &token);
	LithpCell read_from(std::list<std::string> &tokens);

	class Interpreter {
		public:
			// Top-level eval function.
			virtual LithpCell eval(LithpCell x, Env_p env) = 0;
			// Timed evaluation. For performance sake, we prefer this to be called once.
			virtual LithpCell evalTimed(const LithpCell &x, Env_p env) = 0;

			bool DEBUG = false;
			bool TIMING = false;
			bool QUIT = false;

			UnsignedInteger evalTime = 0;
			UnsignedInteger reductions = 0, depth = 0, depth_max = 0;
	};

	// From PLinterpreter.cpp
	bool GetDEBUG();
	void SetDEBUG(const bool);
	bool GetTIMING();
	void SetTIMING(const bool);
	bool GetQUIT();
	void SetQUIT(const bool);
	UnsignedInteger GetEvalTime();
	UnsignedInteger GetReductions();
	UnsignedInteger GetDepth();
	UnsignedInteger GetDepthMax();
	extern UnsignedInteger parseTime;
	extern Interpreter *interpreter;
	Interpreter *StandardInterpreter();
	void SetStandardInterpreter();
	LithpCell repl(const std::string &prompt, Env_p env);
	LithpCell eval(LithpCell x, Env_p env);
	LithpCell evalTimed(const LithpCell &x, Env_p env);

	// From PLruntime.cpp
	void init_runtime();

	// From PLtests.cpp
	namespace Test {
		int RunTests();
	}

	typedef stackless::microthreading::ThreadId LithpThreadId;
	typedef LithpThreadId LithpThreadNodeId;
	typedef LithpThreadId LithpCosmosNodeId;
	namespace Stackless {
		struct LithpImplementation; // defined in PLint_stackless.hpp
	}

	struct LithpThreadReference {
		// Thread ID as returned from thread start
		LithpThreadId thread_id;
		// Node ID of the interpreter, currently always 0. In the future if threading
		// is implemented, this number will have differing values.
		LithpThreadNodeId node_id;
		// Global network (Cosmos) node id. 0 until joined to the Cosmos network.
		LithpCosmosNodeId cosmos_id;

		LithpThreadReference(const LithpThreadId id = 0, const LithpThreadNodeId node = 0, const LithpCosmosNodeId cosmos = 0)
			: thread_id(id), node_id(node), cosmos_id(cosmos) {
		}
		LithpThreadReference(const LithpThreadReference &copy)
			: thread_id(copy.thread_id), node_id(copy.node_id), cosmos_id(copy.cosmos_id) {
		}
		LithpThreadReference(const PocoLithp::Stackless::LithpImplementation &impl);

		std::string str() const {
			return std::string("<") +
				std::to_string(cosmos_id) + std::string(".") +
				std::to_string(node_id) + std::string(".") +
				std::to_string(thread_id) +
				std::string(">");
		}
	};

	struct LithpVar {
		typedef LithpVar(*proc_type)(const LithpCells &);
		typedef LithpVar(*proc_extended_type)(const LithpCells &, Env_p);
		typedef LithpVar(*proc_implementation_type)(const LithpCells &, Env_p, const PocoLithp::Stackless::LithpImplementation &);
		LithpVarType tag;
		PocoVar value;
		Env_p env;

		LithpVar(LithpVarType _tag, PocoVar _value) : tag(_tag), value(_value), env(0) {
			if ((_tag == Atom || _tag == VariableReference) && _value.isString()) {
				// Convert from string to atom
				value = getAtomId(_value.toString());
			}
		}
		LithpVar(const PocoVar &_value) : tag(Var), value(_value), env(0) {
		}
		LithpVar(const PocoVar &_value, const LithpVarType &_tag) : tag(_tag), value(_value), env(0) {
		}
		LithpVar(const LithpVar &copy) : value(copy.value), tag(copy.tag), env(copy.env) {
		}
		// Scheme constructors
		LithpVar(LithpVarType _tag = Var) : tag(_tag), value(), env(0) {}
		LithpVar(proc_type proc) : tag(Proc), value(proc), env(0) {}
		LithpVar(proc_extended_type proc) : tag(ProcExtended), value(proc), env(0) {}
		LithpVar(proc_implementation_type proc) : tag(ProcImplementation), value(proc), env(0) {}
		// Extended constructors
		LithpVar(LithpThreadReference &tr) : tag(Thread), value(tr), env(0) {}

		~LithpVar() {
		}

		// Set the value to that of another LithpVar
		void update(const LithpVar &copy) {
			tag = copy.tag;
			value = copy.value;
			env = copy.env;
		}

		std::string str() const {
			switch (tag) {
			case Var:
			{
				// Floating point check
				const std::type_info &rtti = value.type();
				if (rtti == typeid(double) || rtti == typeid(float)) {
					std::stringstream ss;
					ss << std::scientific << value.convert<double>();
					return ss.str();
				}
				return value.toString();
			}
			case Lambda:
				return "<Lambda>";
			case Macro:
				return "<Macro>";
			case List:
				return "<List>";
			case Proc:
				return "<Proc>";
			case ProcExtended:
				return "<ProcExtended>";
			case ProcImplementation:
				return "<ProcImplementation>";
			case Thread:
				return thread_ref().str();
			case Dict:
				throw InvalidArgumentException("Should be handled higher up");
			case Atom:
			case VariableReference:
				return getAtomById(atomid());
			case Tuple:
				// TODO
			default:
				throw InvalidArgumentException("Unknown type");
			}
		}
		const char *c_str() const {
			return str().c_str();
		}

		bool operator != (const LithpVar &other) const {
			return !(*this == other);
		}
		template<typename Callback>
		static bool CompareWith(const LithpVar &a, const LithpVar &b, Callback cb) {
			// Mismatched tags are always a false comparison
			if (a.tag != b.tag)
				return false;

			// If we have one of the basic types, extract it and perform comparison.
			// Only a specific few types are supported here.
			if (a.tag == Atom && b.tag == Atom) {
				return cb(a.atomid(), b.atomid());
			}
			if (a.tag == Var || a.tag == VariableReference) {
				const std::type_info &rtti = a.value.type();
				if (rtti == typeid(UnsignedInteger) || rtti == typeid(unsigned)) {
					return cb(a.value.convert<UnsignedInteger>(), b.value.convert<UnsignedInteger>());
				} else if (rtti == typeid(SignedInteger) || rtti == typeid(signed)) {
					return cb(a.value.convert<SignedInteger>(), b.value.convert<SignedInteger>());
				} else if (rtti == typeid(bool)) {
					return cb(a.value.convert<bool>(), b.value.convert<bool>());
				} else if (rtti == typeid(double) || rtti == typeid(float)) {
					return cb(a.value.convert<double>(), b.value.convert<double>());
				} else {
					throw InvalidArgumentException("CompareWith: no matching variable types");
				}
			} else if (a.tag == List) {
				// List comparison
				if (b.tag != List)
					return false;
				const LithpCells &alist = a.list();
				const LithpCells &blist = b.list();
				auto it_a = alist.begin(), it_b = blist.begin();
				for ( ;
					it_a != alist.end() && it_b != blist.end();
					++it_a, ++it_b) {
					// Call callback with the list elements
					if (!CompareWith(*it_a, *it_b, cb))
						// Element mismatch, lists are not equal
						return false;
				}
				// Comparison is true if both lists are at their endpoints (length is same)
				return (it_a == alist.end() && it_b == blist.end());
			}

			// Fall back to string comparison (slow!)
			return cb(a.value, b.value);
		}
		bool operator == (const LithpVar &other) const { return CompareWith(*this, other, [](auto a, auto b) { return a == b; }); }
		bool operator < (const LithpVar &other) const { return CompareWith(*this, other, [](auto a, auto b) { return a < b; }); }
		bool operator > (const LithpVar &other) const { return CompareWith(*this, other, [](auto a, auto b) { return a > b; }); }
		bool operator <= (const LithpVar &other) const { return CompareWith(*this, other, [](auto a, auto b) { return a <= b; }); }
		bool operator >= (const LithpVar &other) const { return CompareWith(*this, other, [](auto a, auto b) { return a >= b; }); }
		bool operator && (const LithpVar &other) const { return CompareWith(*this, other, [](auto a, auto b) { return a && b; }); }
		bool operator || (const LithpVar &other) const { return CompareWith(*this, other, [](auto a, auto b) { return a || b; }); }
		const LithpVar operator + (const LithpVar &other) const;
		const LithpVar operator - (const LithpVar &other) const;
		const LithpVar operator / (const LithpVar &other) const;
		const LithpVar operator * (const LithpVar &other) const;
		const LithpVar operator << (const LithpVar &other) const;
		const LithpVar operator >> (const LithpVar &other) const;
		const LithpVar operator ^ (const LithpVar &other) const;
		const LithpVar operator | (const LithpVar &other) const;
		const LithpVar operator & (const LithpVar &other) const;
		const LithpVar operator % (const LithpVar &other) const;
		LithpVar& operator += (const LithpVar &other);
		LithpVar& operator -= (const LithpVar &other);
		LithpVar& operator /= (const LithpVar &other);
		LithpVar& operator *= (const LithpVar &other);

		const bool isString() const {
			return (tag == Var && value.isString());
		}
		bool empty() const {
			if (isList())
				return list().empty();
			else if (isString())
				return value.size() == 0;
			else if (tag == Dict)
				return dict().empty();
			else if (tag == Lambda || tag == Macro)
				return list().empty();
			// TODO: Too lenient?
			return false;
		}
		size_t size() const {
			if (isList())
				return list().size();
			else if (isString())
				return value.size();
			throw InvalidArgumentException("Not an enumerable type");
		}
		Poco::Dynamic::Var::ConstIterator begin() { return value.begin(); }
		Poco::Dynamic::Var::ConstIterator end() { return value.end(); }

		// List related behaviours
		// Read-only!
		bool isList() const {
			return (tag == List || tag == Lambda || tag == Macro);
		}
		const LithpCells &list() const {
			if(isList())
				return value.extract<LithpCells>();
			throw InvalidArgumentException("Not a list");
		}
		const LithpCell &operator[](int index) const {
			return list()[index];
		}

		// Dict related behaviours
		// Read-only!
		const LithpDict &dict() const {
			if (tag != Dict)
				throw InvalidArgumentException("Not a dict");
			return value.extract<LithpDict>();
		}

		// Proc related behaviours
		proc_type proc() const {
			if (tag != Proc)
				throw InvalidArgumentException("Not a proc");
			return value.extract<proc_type>();
		}
		proc_extended_type proc_extended() const {
			if (tag != ProcExtended)
				throw InvalidArgumentException("Not a proc_extended");
			return value.extract<proc_extended_type>();
		}
		proc_implementation_type proc_implementation() const {
			if (tag != ProcImplementation)
				throw InvalidArgumentException("Not a proc_implementation");
			return value.extract<proc_implementation_type>();
		}

		// Atom behaviours
		const atomId atomid() const {
			if (tag != Atom && tag != VariableReference)
				throw InvalidArgumentException("Not an atom, tag: " + std::to_string(tag) + ", value: " + str());
			return value.extract<atomId>();
		}
		
		// Extended behaviours
		LithpThreadReference thread_ref() const {
			if (tag != Thread)
				throw InvalidArgumentException("Not a thread ref");
			return value.extract<LithpThreadReference>();
		}

		bool is_nullp() const {
			switch (tag) {
			case List:
			case Lambda:
			case Macro:
				return list().empty();
			}
			return true;
		}
	};

	struct LithpEnvironment {
		LithpEnvironment(Env_p outer = nullptr) : outer_(outer) {}
		LithpEnvironment(const LithpCells &params, const LithpCells &args, Env_p outer)
			: outer_(outer) {
			update(params, args);
		}

		typedef std::shared_ptr<LithpEnvironment> _env_p;

		void update(const LithpCells &params, const LithpCells &args) {
			LithpCellIt a = args.begin();
			for (LithpCellIt p = params.begin(); p != params.end() && a != args.end(); ++p)
				env_[p->atomid()] = *a++;
		}

		~LithpEnvironment() {
			if (GetDEBUG()) std::cerr << "Environment " << this << " has been deleted\n";
		}

		bool defined(const atomId var) const {
			if (env_.find(var) != env_.end())
				return true;
			if (outer_)
				return outer_->defined(var);
			return false;
		}
		LithpDict &find(const atomId var) {
			if (env_.find(var) != env_.end())
				return env_;
			if (outer_)
				return outer_->find(var);
			throw InvalidArgumentException("Unbound symbol " + getAtomById(var));
		}

		LithpCell &operator[](const atomId var) {
			return env_[var];
		}
		LithpCell &operator[](const std::string &name) {
			return env_[getAtomId(name)];
		}

		LithpCells getCompleteEnv() const {
			LithpCells resultList;
			if (outer_) {
				const LithpCells &parentEnv = outer_->getCompleteEnv();
				resultList.push_back(LithpCell(List, parentEnv));
			}
			resultList.push_back(LithpCell(Dict, env_));
			return resultList;
		}
		size_t size() const {
			return env_.size();
		}

		LithpEnvironment *getOuter() const {
			return outer_.get();
		}
		bool hasOuter() const { return outer_ != nullptr; }

		Env_p getTopmost(Env_p topmost) {
			while(topmost->hasOuter())
				topmost = topmost->outer_;
			return topmost;
		}
	private:
		LithpDict env_;
		Env_p outer_;
	};
}
