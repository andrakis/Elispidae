#include "stdafx.h"

namespace PocoLithp {
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
	static LithpVar Combine(const PocoVar &a, const PocoVar &b, Callback cb) {
		T ta = a.convert<T>();
		T tb = b.convert<T>();
		return cb(ta, tb);
	}

	///
	/// Combine two Poco vars, which are integers, with a callback.
	/// Specifically handles signed / unsigned values.
	template<typename Callback>
	static LithpVar IntegerCombine(const PocoVar &a, const PocoVar &b, Callback cb) {
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