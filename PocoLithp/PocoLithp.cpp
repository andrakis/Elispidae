// PocoLithp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

namespace PocoLithp {
	typedef Poco::InvalidArgumentException InvalidArgumentException;

	struct LithpVar {
		enum LithpVarType {
			Var,
			Dict,
			Atom,
			Tuple,
			OpChain,
			Closure
		} tag;
		Poco::Dynamic::Var value;

		LithpVar(const Poco::Dynamic::Var &_value) : value(_value), tag(Var) {
		}
		LithpVar(const Poco::Dynamic::Var &_value, const LithpVarType &_tag) : value(_value), tag(_tag) {
		}
		LithpVar(const LithpVar &copy) : value(copy.value), tag(copy.tag) {
		}
		~LithpVar() {
		}

		std::string str() const {
			switch (tag) {
			case Var:
				return value.toString();
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

		bool operator == (const LithpVar &other) const {
			if (tag != other.tag)
				return false;
			return value == other.value;
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
			return cb(a.value, b.value);
		}
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
	};

	const LithpVar LithpVar::operator + (const LithpVar &other) const {
		switch (tag) {
		case Var:
			return LithpVar(value + other.value);
		default:
			throw InvalidArgumentException("Invalid operation");
		}
	}
	const LithpVar LithpVar::operator - (const LithpVar &other) const {
		switch (tag) {
		case Var:
			return LithpVar(value - other.value);
		default:
			throw InvalidArgumentException("Invalid operation");
		}
	}
	const LithpVar LithpVar::operator / (const LithpVar &other) const {
		switch (tag) {
		case Var:
			return LithpVar(value / other.value);
		default:
			throw InvalidArgumentException("Invalid operation");
		}
	}
	const LithpVar LithpVar::operator * (const LithpVar &other) const {
		switch (tag) {
		case Var:
			return LithpVar(value * other.value);
		default:
			throw InvalidArgumentException("Invalid operation");
		}
	}
	///
	/// Combine two Poco Vars after converting to type T, with a callback.
	/// cb is passed two arguments, the values of a and b as type T.
	/// cb should return a LithpVar.
	template<typename T, typename Callback>
	static LithpVar Combine(const Poco::Dynamic::Var &a, const Poco::Dynamic::Var &b, Callback cb) {
		T ta = a.convert<T>();
		T tb = b.convert<T>();
		return cb(ta, tb);
	}

	///
	/// Combine two Poco vars, which are integers, with a callback.
	/// Specifically handles signed / unsigned values.
	template<typename Callback>
	static LithpVar IntegerCombine(const Poco::Dynamic::Var &a, const Poco::Dynamic::Var &b, Callback cb) {
		if (!a.isNumeric() || !a.isInteger())
			throw InvalidArgumentException("LHS must be an integer");
		if (!b.isNumeric() || !b.isInteger())
			throw InvalidArgumentException("RHS must be an integer");
		if (a.isSigned()) {
			return Combine<Poco::Int64>(a, b, cb);
		} else {
			return Combine<Poco::UInt64>(a, b, cb);
		}
	}

	const LithpVar LithpVar::operator << (const LithpVar &other) const {
		switch (tag) {
		case Var:
			return IntegerCombine(value, other.value, [](auto a, auto b) { return LithpVar(a << b); });
		default:
			throw InvalidArgumentException("Invalid operation");
		}
	}
	const LithpVar LithpVar::operator >> (const LithpVar &other) const {
		switch (tag) {
		case Var:
			return IntegerCombine(value, other.value, [](auto a, auto b) { return LithpVar(a >> b); });
		default:
			throw InvalidArgumentException("Invalid operation");
		}
	}
	const LithpVar LithpVar::operator ^ (const LithpVar &other) const {
		switch (tag) {
		case Var:
			return IntegerCombine(value, other.value, [](auto a, auto b) { return LithpVar(a ^ b); });
		default:
			throw InvalidArgumentException("Invalid operation");
		}
	}
	const LithpVar LithpVar::operator | (const LithpVar &other) const {
		switch (tag) {
		case Var:
			return IntegerCombine(value, other.value, [](auto a, auto b) { return LithpVar(a | b); });
		default:
			throw InvalidArgumentException("Invalid operation");
		}
	}
	const LithpVar LithpVar::operator & (const LithpVar &other) const {
		switch (tag) {
		case Var:
			return IntegerCombine(value, other.value, [](auto a, auto b) { return LithpVar(a & b); });
		default:
			throw InvalidArgumentException("Invalid operation");
		}
	}
	const LithpVar LithpVar::operator % (const LithpVar &other) const {
		switch (tag) {
		case Var:
			return IntegerCombine(value, other.value, [](auto a, auto b) { return LithpVar(a % b); });
		default:
			throw InvalidArgumentException("Invalid operation");
		}
	}
	LithpVar& LithpVar::operator += (const LithpVar &other) {
		switch (tag) {
		case Var:
			return *this = LithpVar(value + other.value);
		default:
			throw InvalidArgumentException("Invalid operation");
		}
	}
	LithpVar& LithpVar::operator -= (const LithpVar &other) {
		switch (tag) {
		case Var:
			return *this = LithpVar(value - other.value);
		default:
			throw InvalidArgumentException("Invalid operation");
		}
	}
	LithpVar& LithpVar::operator /= (const LithpVar &other) {
		switch (tag) {
		case Var:
			return *this = LithpVar(value / other.value);
		default:
			throw InvalidArgumentException("Invalid operation");
		}
	}
	LithpVar& LithpVar::operator *= (const LithpVar &other) {
		switch (tag) {
		case Var:
			return *this = LithpVar(value * other.value);
		default:
			throw InvalidArgumentException("Invalid operation");
		}
	}
}

using namespace PocoLithp;
int scheme_main();
void scheme_test();

int main()
{
	Poco::Dynamic::Struct<int> atomTable;

	atomTable[0] = 1;
	atomTable[5] = "??";

	LithpVar a = 4, b = 2;
	LithpVar c = a * b;
	std::cout << "A: " << a.str() << ", B: " << b.str() << ", C: " << c.str() << std::endl;
	//scheme_main();
	scheme_test();
    return 0;
}

