#pragma once

#include "stdafx.h"

#define PLITHP_VERSION "0.28"

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
		Tuple,
		//OpChain, // AKA lambda
		//Closure, // AKA environment

		// scheme types
		List,
		Proc,
		Lambda
	};

	struct LithpEnvironment;
	struct LithpVar;

	typedef LithpVar LithpCell;
	typedef std::vector<LithpCell> LithpCells;
	typedef LithpCells::const_iterator LithpCellIt;

	typedef uint32_t atomId;
	typedef std::map<atomId, std::string> AtomMapById_t;
	typedef std::map<std::string, atomId> AtomMapByName_t;
	std::string getAtomById(const atomId id);
	// Declare an atom to exist and get its id
	atomId getAtomId(const std::string &);
	// Declare an atom to exist and get it as a cell
	LithpCell getAtom(const std::string &);

	struct LithpVar {
		typedef LithpVar(*proc_type)(const LithpCells &, LithpEnvironment *);
		LithpVarType tag;
		PocoVar value;
		LithpEnvironment *env;

		LithpVar(LithpVarType _tag, PocoVar _value) : tag(_tag), value(_value), env(0) {
			if (_tag == Atom && _value.isString()) {
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
			case Dict:
				// TODO
			case Atom:
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

		// Proc related behaviours
		proc_type proc() {
			if (tag != Proc)
				throw InvalidArgumentException("Not a proc");
			return value.extract<proc_type>();
		}

		// Atom behaviours
		const atomId atomid() const {
			if (tag != Atom)
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
		LithpEnvironment(LithpEnvironment *outer = 0) : outer_(outer) {}
		LithpEnvironment(const LithpCells &params, const LithpCells &args, LithpEnvironment *outer)
			: outer_(outer) {
			LithpCellIt a = args.begin();
			for (LithpCellIt p = params.begin(); p != params.end(); ++p)
				env_[p->atomid()] = *a++;
		}
		~LithpEnvironment() {
			// free child environments
			child_env_map::const_iterator child_envs_it = child_envs.begin();
			child_env_delete_depth++;
			std::string spacer((child_env_delete_depth - 1) * 2, ' ');
			for (; child_envs_it != child_envs.end(); ++child_envs_it) {
				LithpEnvironment *child = *child_envs_it;
				//if(DEBUG) std::cerr << spacer << "Deleting child environment: " << child << "\n";
				delete child;
			}
			child_env_delete_depth--;
			child_envs.clear();
		}

		typedef std::map<atomId, LithpCell> map;
		map &find(const atomId var) {
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

		typedef std::vector<LithpEnvironment*> child_env_map;
		void remember_child_env(LithpEnvironment *e) {
			child_envs.push_back(e);
		}
	private:
		map env_;
		LithpEnvironment *outer_;
		child_env_map child_envs;   // Keep track of child environments
		static int child_env_delete_depth;
	};

	// From PLglobals.cpp
	extern const LithpCell sym_false;
	extern const LithpCell sym_true;
	extern const LithpCell sym_nil;
	extern const LithpCell sym_quote;
	extern const LithpCell sym_if;
	extern const LithpCell sym_set;
	extern const LithpCell sym_define;
	extern const LithpCell sym_lambda;
	extern const LithpCell sym_begin;
	std::string to_string(const LithpCell &exp);
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
	extern UnsignedInteger reductions;
	extern bool DEBUG, TIMING, QUIT;
	void repl(const std::string &prompt, LithpEnvironment *env);
	LithpCell eval(LithpCell x, LithpEnvironment *env);
	LithpCell evalTimed(const LithpCell &x, LithpEnvironment *env);

	// From PLtests.cpp
	namespace Test {
		int RunTests();
	}
}
