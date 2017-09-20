#pragma once

#include "stdafx.h"

#define PLITHP_VERSION "0.46"

#ifndef NO_STATS
#define PLITHP_TRACK_STATS
#endif

#ifdef PLITHP_TRACK_STATS
#define TRACK_STATS(code) do { code; } while(0)
#define STATS_DESC        "(stats enabled)"
#else
#define TRACK_STATS(code) while(0)
#define STATS_DESC        "(no stats)"
#endif

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
#define PLITHP_INT64
#endif

#ifdef PLITHP_INT64
	typedef Poco::Int64 SignedInteger;
	typedef Poco::UInt64 UnsignedInteger;
	const std::string PLITHP_ARCH = "(Int64 support)";
#else
	typedef long SignedInteger;
	typedef unsigned long UnsignedInteger;
	const std::string PLITHP_ARCH = "(Int32 only)";
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
		Lambda
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
	extern const LithpCell sym_lambda;
	extern const LithpCell sym_begin;
	std::string to_string(const LithpCell &exp);
	std::string to_string(const LithpCell &exp, bool repre);
	const LithpCell booleanCell(const bool val);
	bool booleanVal(const LithpCell &val);
	std::string getAtomById(atomId id);
	atomId getAtomId(const std::string &name);
	LithpCell getAtom(const std::string &name);
	void add_globals(LithpEnvironment &env);

	// From PLparser.cpp
	LithpCell read(const std::string &s);
	std::string str(long n);
	bool isdig(char c);
	std::list<std::string> tokenize(const std::string & str);
	PocoVar parseNumber(const std::string &token);
	LithpCell atom(const std::string &token);
	LithpCell read_from(std::list<std::string> &tokens);

	// From PLinterpreter.cpp
	extern UnsignedInteger parseTime, evalTime;
	extern UnsignedInteger reductions, depth, depth_max;
	extern bool DEBUG, TIMING, QUIT;
	LithpCell repl(const std::string &prompt, Env_p env);
	LithpCell eval(LithpCell x, Env_p env);
	LithpCell evalTimed(const LithpCell &x, Env_p env);

	// From PLtests.cpp
	namespace Test {
		int RunTests();
	}

	struct LithpVar {
		typedef LithpVar(*proc_type)(const LithpCells &);
		typedef LithpVar(*proc_extended_type)(const LithpCells &, Env_p);
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

		~LithpVar() {
		}

		std::string str() const {
			switch (tag) {
			case Var:
				// TODO: Floats are getting truncated
				return value.toString();
			case Lambda:
				return "<Lambda>";
			case List:
				return "<List>";
			case Proc:
				return "<Proc>";
			case ProcExtended:
				return "<ProcExtended>";
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
			if (a.tag != b.tag)
				return false;
			if (a.tag != Var && a.tag != Atom)
				return false;
			const std::type_info &rtti = a.value.type();
			if (rtti == typeid(atomId)) {
				return cb(a.atomid(), b.atomid());
			} else if (rtti == typeid(UnsignedInteger) || rtti == typeid(unsigned)) {
				return cb(a.value.extract<UnsignedInteger>(), b.value.convert<UnsignedInteger>());
			} else if (rtti == typeid(SignedInteger) || rtti == typeid(signed)) {
				return cb(a.value.extract<SignedInteger>(), b.value.convert<SignedInteger>());
			} else if (rtti == typeid(bool)) {
				return cb(a.value.extract<bool>(), b.value.convert<bool>());
			} else if (rtti == typeid(double) || rtti == typeid(float)) {
				return cb(a.value.convert<double>(), b.value.convert<double>());
			}
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

		// List related behaviours
		// Read-only!
		const LithpCells &list() const {
			if (tag != List && tag != Lambda)
				throw InvalidArgumentException("Not a list");
			return value.extract<LithpCells>();
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

		// Atom behaviours
		const atomId atomid() const {
			if (tag != Atom && tag != VariableReference)
				throw InvalidArgumentException("Not an atom");
			return value.extract<atomId>();
		}

		bool is_nullp() const {
			switch (tag) {
			case List:
			case Lambda:
				return list().empty();
			}
			return true;
		}
	};

	struct LithpEnvironment {
		LithpEnvironment(Env_p outer = 0) : outer_(outer) {}
		LithpEnvironment(const LithpCells &params, const LithpCells &args, Env_p outer)
			: outer_(outer) {
			update(params, args);
		}

		void update(const LithpCells &params, const LithpCells &args) {
			LithpCellIt a = args.begin();
			for (LithpCellIt p = params.begin(); p != params.end(); ++p)
				env_[p->atomid()] = *a++;
		}

		~LithpEnvironment() {
			if (DEBUG) std::cerr << "Environment " << this << " has been deleted\n";
		}

		LithpDict &find(const atomId var) {
			if (env_.find(var) != env_.end())
				return env_;
			if (outer_)
				return outer_->find(var);
			throw InvalidArgumentException("Unbound symbol '" + getAtomById(var) + "'");
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
	private:
		LithpDict env_;
		Env_p outer_;
		static int child_env_delete_depth;
	};
}
