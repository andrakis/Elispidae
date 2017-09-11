#pragma once

#include "stdafx.h"

namespace PocoLithp {
	typedef Poco::Dynamic::Var PocoVar;
	typedef std::vector<PocoVar> PocoList;
	typedef std::map<PocoVar, PocoVar> PocoDict;

	typedef Poco::NumberParser NumberParser;

	typedef Poco::InvalidArgumentException InvalidArgumentException;
	typedef unsigned AtomType;

#if defined(POCO_HAVE_INT64)
	typedef Poco::Int64 SignedInteger;
	typedef Poco::UInt64 UnsignedInteger;
#else
	typedef long SignedInteger;
	typedef unsigned long UnsignedInteger;
#endif

	enum LithpVarType {
		Var,
		Dict,
		Atom,
		Tuple,
		//OpChain, // AKA lambda
		//Closure, // AKA environment

		// scheme types
		Symbol,
		//Number, // Use Var instead
		List,
		Proc,
		Lambda
	};

	struct LithpEnvironment;
	struct LithpVar;

	typedef LithpVar LithpCell;
	typedef std::vector<LithpCell> LithpCells;
	typedef LithpCells::const_iterator LithpCellIt;

	struct LithpVar {
		typedef LithpVar(*proc_type)(const LithpCells &, LithpEnvironment *);
		LithpVarType tag;
		PocoVar value;
		LithpEnvironment *env;

		LithpVar(LithpVarType _tag, PocoVar _value) : tag(_tag), value(_value), env(0) {}
		LithpVar(const PocoVar &_value) : tag(Var), value(_value), env(0) {
		}
		LithpVar(const PocoVar &_value, const LithpVarType &_tag) : tag(_tag), value(_value), env(0) {
		}
		LithpVar(const LithpVar &copy) : value(copy.value), tag(copy.tag), env(copy.env) {
		}
		// Scheme constructors
		LithpVar(LithpVarType _tag = Symbol) : tag(_tag), value(), env(0) {}
		LithpVar(proc_type proc) : tag(Proc), value(proc), env(0) {}

		~LithpVar() {
		}

		std::string str() const {
			switch (tag) {
			case Var:
				// TODO: Floats are getting truncated
			case Symbol:
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
				// TODO
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
			if (a.tag != Var)
				return false;
			const std::type_info &rtti = a.value.type();
			if (rtti == typeid(double) || rtti == typeid(float)) {
				return cb(a.value.convert<double>(), b.value.convert<double>());
			} else if (rtti == typeid(SignedInteger) || rtti == typeid(signed)) {
				return cb(a.value.extract<SignedInteger>(), b.value.convert<SignedInteger>());
			} else if (rtti == typeid(UnsignedInteger) || rtti == typeid(unsigned)) {
				return cb(a.value.extract<UnsignedInteger>(), b.value.convert<UnsignedInteger>());
			} else if (rtti == typeid(bool)) {
				return cb(a.value.extract<bool>(), b.value.convert<bool>());
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

		// Proc related behaviours
		proc_type proc() {
			if (tag != Proc)
				throw InvalidArgumentException("Not a proc");
			return value.extract<proc_type>();
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
				env_[p->str()] = *a++;
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

		// TODO: Map should be using an integer type for key, using atom integer value
		typedef std::map<std::string, LithpCell> map;
		map &find(const std::string &var) {
			if (env_.find(var) != env_.end())
				return env_;
			if (outer_)
				return outer_->find(var);
			throw InvalidArgumentException("Unbound symbol: " + var);
		}

		LithpCell &operator[](const std::string &var) {
			return env_[var];
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
}
