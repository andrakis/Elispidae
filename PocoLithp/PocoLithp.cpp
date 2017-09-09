// PocoLithp.cpp : Defines the entry point for the console application.
//

// TODO: Bignum support

#include "stdafx.h"

typedef Poco::Dynamic::Var PocoVar;
typedef std::vector<PocoVar> PocoList;
typedef std::map<PocoVar, PocoVar> PocoDict;

typedef Poco::NumberParser NumberParser;

namespace PocoLithp {
	typedef Poco::InvalidArgumentException InvalidArgumentException;

	const bool DEBUG = false;

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
		typedef LithpVar(*proc_type)(const LithpCells &);
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
				//if(DEBUG) std::cerr << spacer << "Deleting child environment: " << child << std::endl;
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
	int LithpEnvironment::child_env_delete_depth = 0;

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

	// TODO: These should be atoms?
	const LithpCell false_sym(Symbol, "#f");
	const LithpCell true_sym(Symbol, "#t");
	const LithpCell nil(Symbol, "nil");
	std::string to_string(const LithpCell & exp);

	LithpCell eval(LithpCell x, LithpEnvironment *env) {
		// TODO: Only do this lookup with atoms
		if (x.tag == Symbol) {
			LithpCell &r = env->find(x.str())[x.str()];
			if (DEBUG) std::cerr << to_string(x) << " => " << to_string(r) << std::endl;
			return r;
		}
		// TODO: Variable lookup (must currently use Symbol)

		// Vars can be returned as is
		if (x.tag == Var)
			return x;
		// TODO: Differentiate between list and opchain? Current implementation assumes list and
		// determines type
		if (!x.value.isArray() || x.value.size() == 0)
			return nil;
		const LithpCells &xl = x.list();
		const LithpCell &xl0 = xl[0];
		if (xl0.tag == Symbol) {
			if (xl0.value == "quote")      // (quote exp)
				return xl[1];
			if (xl0.value == "if")         // (if test conseq [alt])
				return eval(eval(xl[1], env).value == "#f" ? (xl.size() < 4 ? nil : xl[3]) : xl[2], env);
			if (xl0.value == "set!")       // (set! var exp)
				return env->find(xl[1].str())[xl[1].str()] = eval(xl[2], env);
			if (xl0.value == "define")     // (define var exp)
				return (*env)[xl[1].str()] = eval(xl[2], env);
			if (xl0.value == "lambda") {   // (lambda (var*) exp)
				x.tag = Lambda;
				// Keep a reference to the environment that exists now (when the
				// lambda is being defined) because that's the outer environment
				// we'll need to use when the lambda is executed
				x.env = env;
				return x;
			}
			if (xl0.value == "begin") {     // (begin exp*)
				// TODO: This is the OpChain run section
				for (size_t i = 1; i < xl.size() - 1; ++i)
					eval(xl[i], env);
				// WAS: return eval(x.list[x.list.size() - 1], env)
				return eval(xl.back(), env);
			}
		} 
		// (proc exp*)
		LithpCell proc(eval(xl[0], env));
		LithpCells exps;
		for (auto exp = xl.begin() + 1; exp != xl.end(); ++exp)
			exps.push_back(eval(*exp, env));
		if (proc.tag == Lambda) {
			// Create an environment for the execution of this lambda function
			// where the outer environment is the one that existed* at the time
			// the lambda was defined and the new inner associations are the
			// parameter names with the given arguments.
			// *Although the environmet existed at the time the lambda was defined
			// it wasn't necessarily complete - it may have subsequently had
			// more symbols defined in that environment.
			LithpCells proclist = proc.list();
			LithpEnvironment *child_env = new LithpEnvironment(/*parms*/proclist[1].list(), /*args*/exps, proc.env);
			env->remember_child_env(child_env);
			return eval(/*body*/proclist[2], child_env);
		}
		else if (proc.tag == Proc) {
			LithpCell &r = proc.proc()(exps);
			if (DEBUG) std::cerr << "<Proc>" << to_string(LithpVar(List, exps)) << " => " << to_string(r) << std::endl;
			return r;
		}
		throw InvalidArgumentException("Unhandled type");
	}

	// Define the bare minimum set of primitives necessary to pass the unit tests
	LithpCell proc_add(const LithpCells &c)
	{
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i) {
			// Signedness promotion check
			if (i->value.isSigned() == true && n.value.isSigned() == false) {
#if defined(POCO_HAVE_INT64)
				n.value = n.value.convert<Poco::Int64>();
#else
				n.value = n.value.convert<long>();
#endif
			}
			n += *i;
		}
		return n;
	}

	LithpCell proc_sub(const LithpCells & c)
	{
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i) {
			// Signedness promotion check
			if (i->value.isSigned() == true && n.value.isSigned() == false) {
#if defined(POCO_HAVE_INT64)
				n.value = n.value.convert<Poco::Int64>();
#else
				n.value = n.value.convert<long>();
#endif
			}
			n -= *i;
		}
		return n;
	}

	LithpCell proc_mul(const LithpCells & c)
	{
		LithpCell n(1);
		for (LithpCells::const_iterator i = c.begin(); i != c.end(); ++i) {
			// Signedness promotion check
			if (i->value.isSigned() == true && n.value.isSigned() == false) {
#if defined(POCO_HAVE_INT64)
				n.value = n.value.convert<Poco::Int64>();
#else
				n.value = n.value.convert<long>();
#endif
			}
			n *= *i;
		}
		return n;
	}

	LithpCell proc_div(const LithpCells & c)
	{
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i) {
			// Signedness promotion check
			if (i->value.isSigned() == true && n.value.isSigned() == false) {
#if defined(POCO_HAVE_INT64)
				n.value = n.value.convert<Poco::Int64>();
#else
				n.value = n.value.convert<long>();
#endif
			}
			n /= *i;
		}
		return n;
	}

	LithpCell proc_greater(const LithpCells & c)
	{
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i)
			if (n <= *i)
				return false_sym;
		return true_sym;
	}

	LithpCell proc_less(const LithpCells & c)
	{
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i)
			if (n >= *i)
				return false_sym;
		return true_sym;
	}

	LithpCell proc_less_equal(const LithpCells & c)
	{
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i)
			if (n > *i)
				return false_sym;
		return true_sym;
	}

	LithpCell proc_length(const LithpCells & c) { return LithpCell(Var, c[0].list().size()); }
	LithpCell proc_nullp(const LithpCells & c) { return c[0].is_nullp() ? true_sym : false_sym; }
	LithpCell proc_head(const LithpCells & c) { return c[0].list()[0]; }

	LithpCell proc_tail(const LithpCells & c)
	{
		if (c[0].list().size() < 2)
			return nil;
		LithpCells result = c[0].list();
		result.erase(result.begin());
		return LithpCell(List, result);
	}

	LithpCell proc_append(const LithpCells & c)
	{
		LithpCells result = c[0].list();
		const LithpCells &c1l = c[1].list();
		for (LithpCells::const_iterator i = c1l.begin(); i != c1l.end(); ++i)
			result.push_back(*i);
		return LithpCell(List, result);
	}

	LithpCell proc_cons(const LithpCells & c)
	{
		LithpCells result;
		const LithpCells &c1l = c[1].list();
		result.push_back(c[0]);
		for (LithpCells::const_iterator i = c1l.begin(); i != c1l.end(); ++i)
			result.push_back(*i);
		return LithpCell(List, result);
	}

	LithpCell proc_list(const LithpCells & c)
	{
//		LithpCell result(List); result.list = c;
//		return result;
		return LithpCell(List, c);
	}

	// define the bare minimum set of primitives necessary to pass the unit tests
	void add_globals(LithpEnvironment &env)
	{
		env["nil"] = nil;   env["#f"] = false_sym;  env["#t"] = true_sym;
		env["append"] = LithpCell(&proc_append);   env["head"] = LithpCell(&proc_head);
		env["tail"] = LithpCell(&proc_tail);      env["cons"] = LithpCell(&proc_cons);
		env["length"] = LithpCell(&proc_length);   env["list"] = LithpCell(&proc_list);
		env["null?"] = LithpCell(&proc_nullp);    env["+"] = LithpCell(&proc_add);
		env["-"] = LithpCell(&proc_sub);      env["*"] = LithpCell(&proc_mul);
		env["/"] = LithpCell(&proc_div);      env[">"] = LithpCell(&proc_greater);
		env["<"] = LithpCell(&proc_less);     env["<="] = LithpCell(&proc_less_equal);
	}

	////////////////////// parse, read and user interaction
	// return given mumber as a string
	std::string str(long n) { std::ostringstream os; os << n; return os.str(); }

	// return true iff given character is '0'..'9'
	bool isdig(char c) { return isdigit(static_cast<unsigned char>(c)) != 0; }

	// convert given string to list of tokens
	std::list<std::string> tokenize(const std::string & str)
	{
		std::list<std::string> tokens;
		const char * s = str.c_str();
		while (*s) {
			while (*s == ' ')
				++s;
			if (*s == '(' || *s == ')')
				tokens.push_back(*s++ == '(' ? "(" : ")");
			else {
				const char * t = s;
				while (*t && *t != ' ' && *t != '(' && *t != ')')
					++t;
				tokens.push_back(std::string(s, t));
				s = t;
			}
		}
		return tokens;
	}

	PocoVar parseNumber(const std::string &token) {
		// TODO: This is hacky

		if (token.find('.', 0) != std::string::npos) {
			// Float
			return NumberParser::parseFloat(token);
		} else {
			// Integer or unsigned hex / octal

			// Force signed for standard numbers (hex and octal will fail)
			if (token[0] == '-')
			{
#if defined(POCO_HAVE_INT64)
				Poco::Int64 i64;
				if (NumberParser::tryParse64(token, i64))
					return i64;
#else
				long i;
				if (NumberParser::tryParse(token, i))
					return i;
#endif
			}
			// Unsigned (including hex and octal)
#if defined(POCO_HAVE_INT64)
			Poco::UInt64 ui64;
			if (NumberParser::tryParseUnsigned64(token, ui64))
				return ui64;
			if (NumberParser::tryParseHex64(token, ui64))
				return ui64;
			if (NumberParser::tryParseOct64(token, ui64))
				return ui64;
#else
			unsigned long ui;
			if (NumberParser::tryParseUnsigned(token, ui))
				return ui;
			if (NumberParser::tryParseHex(token, ui))
				return ui;
			if (NumberParser::tryParseOct(token, ui))
				return ui;
#endif
		}
		throw InvalidArgumentException("Not a number: " + token);
	}

	// numbers become Numbers; every other token is a Symbol
	LithpCell atom(const std::string & token)
	{
		if (isdig(token[0]) || (token[0] == '-' && isdig(token[1]))) {
			PocoVar V = parseNumber(token);
			if (DEBUG) {
				std::cerr << "atom(" << token << ") = " << to_string(V) << std::endl;
				std::cerr << "    .isNumeric = " << V.isNumeric() << std::endl;
				std::cerr << "    .isString = " << V.isString() << std::endl;
			}
			return LithpCell(Var, V);
		}
		return LithpCell(Symbol, token);
	}

	// return the Lisp expression in the given tokens
	LithpCell read_from(std::list<std::string> & tokens)
	{
		const std::string token(tokens.front());
		tokens.pop_front();
		if (token == "(") {
//			LithpCell c(List);
//			while (tokens.front() != ")")
//				c.list.push_back(read_from(tokens));
			LithpCells c;
			while (tokens.front() != ")")
				c.push_back(read_from(tokens));
			tokens.pop_front();
			return LithpCell(List, c);
		}
		else
			return atom(token);
	}

	// return the Lisp expression represented by the given string
	LithpCell read(const std::string & s)
	{
		std::list<std::string> tokens(tokenize(s));
		LithpCell result = read_from(tokens);
		//std::cerr << "read_from() => " << to_string(result) << std::endl;
		return result;
	}

	// convert given LithpCell to a Lisp-readable string
	std::string to_string(const LithpCell & exp)
	{
		if (exp.tag == List) {
			std::string s("(");
			// TODO: Expensive copy here
			LithpCells expl = exp.list();
			for (LithpCells::const_iterator e = expl.begin(); e != expl.end(); ++e)
				s += to_string(*e) + ' ';
			if (s[s.size() - 1] == ' ')
				s.erase(s.size() - 1);
			return s + ')';
		}
		else if (exp.tag == Lambda)
			return "<Lambda>";
		else if (exp.tag == Proc)
			return "<Proc>";
		return exp.str();
	}

	// the default read-eval-print-loop
	void repl(const std::string & prompt, LithpEnvironment *env)
	{
		for (;;) {
			std::cout << prompt;
			std::string line; std::getline(std::cin, line);
			std::cout << to_string(eval(read(line), env)) << '\n';
		}
	}

	int plithp_main()
	{
		LithpEnvironment global_env; add_globals(global_env);
		repl("plithp> ", &global_env);
		return 0;
	}

	void plithp_test() {
		std::string line;
		LithpEnvironment _env, *env = &_env; add_globals(_env);
		eval(read("(define multiply-by (lambda (n) (lambda (y) (* y n))))"), env);
		eval(read("(define doubler (multiply-by 2))"), env);
		LithpCell result = eval(read("(doubler 4)"), env);
		std::cout << to_string(result) << std::endl;
	}

	////////////////////// unit tests
	unsigned g_test_count;      // count of number of unit tests executed
	unsigned g_fault_count;     // count of number of unit tests that fail
	template <typename T1, typename T2>
	void test_equal_(const T1 & value, const T2 & expected_value, const char * file, int line, const std::string &code)
	{
		++g_test_count;
		std::cerr
			<< code << " : "
			<< " expected " << expected_value
			<< ", got " << value;
		if (value != expected_value) {
			++g_fault_count;
			std::cerr << " - FAIL\n";
		} else {
			std::cerr << " - success\n";
		}
	}
	// write a message to std::cout if value != expected_value
	#define TEST_EQUAL(value, expected_value, code) test_equal_(value, expected_value, __FILE__, __LINE__, code)
	// evaluate the given Lisp expression and compare the result against the given expected_result
	#define TEST(expr, expected_result) TEST_EQUAL(to_string(eval(read(expr), &global_env)), expected_result, expr)

	unsigned plithp_abs_test() {
		LithpEnvironment global_env; add_globals(global_env);
		TEST("(define abs (lambda (n) ((if (> n 0) + -) 0 n)))", "<Lambda>");
		TEST("(list (abs -3) (abs 0) (abs 3))", "(3 0 3)");
		return 0;
	}

	unsigned plithp_complete_test() {
		LithpEnvironment global_env; add_globals(global_env);
		// the 29 unit tests for lis.py
		TEST("(quote (testing 1 (2.0) -3.14e159))", "(testing 1 (2) -3.14e+159)");
		TEST("(+ 2 2)", "4");
		TEST("(+ (* 2 100) (* 1 10))", "210");
		TEST("(if (> 6 5) (+ 1 1) (+ 2 2))", "2");
		TEST("(if (< 6 5) (+ 1 1) (+ 2 2))", "4");
		TEST("(define x 3)", "3");
		TEST("x", "3");
		TEST("(+ x x)", "6");
		TEST("(begin (define x 1) (set! x (+ x 1)) (+ x 1))", "3");
		TEST("((lambda (x) (+ x x)) 5)", "10");
		TEST("(define twice (lambda (x) (* 2 x)))", "<Lambda>");
		TEST("(twice 5)", "10");
		TEST("(define compose (lambda (f g) (lambda (x) (f (g x)))))", "<Lambda>");
		TEST("((compose list twice) 5)", "(10)");
		TEST("(define repeat (lambda (f) (compose f f)))", "<Lambda>");
		TEST("((repeat twice) 5)", "20");
		TEST("((repeat (repeat twice)) 5)", "80");
		TEST("(define fact (lambda (n) (if (<= n 1) 1 (* n (fact (- n 1))))))", "<Lambda>");
		TEST("(fact 3)", "6");
		// TODO: Bignum support
		//TEST("(fact 50)", "30414093201713378043612608166064768844377641568960512000000000000");
		TEST("(fact 12)", "479001600");
		TEST("(define abs (lambda (n) ((if (> n 0) + -) 0 n)))", "<Lambda>");
		TEST("(list (abs -3) (abs 0) (abs 3))", "(3 0 3)");
		TEST("(define combine (lambda (f)"
			"(lambda (x y)"
			"(if (null? x) (quote ())"
			"(f (list (head x) (head y))"
			"((combine f) (tail x) (tail y)))))))", "<Lambda>");
		TEST("(define zip (combine cons))", "<Lambda>");
		TEST("(zip (list 1 2 3 4) (list 5 6 7 8))", "((1 5) (2 6) (3 7) (4 8))");
		TEST("(define riff-shuffle (lambda (deck) (begin"
			"(define take (lambda (n seq) (if (<= n 0) (quote ()) (cons (head seq) (take (- n 1) (tail seq))))))"
			"(define drop (lambda (n seq) (if (<= n 0) seq (drop (- n 1) (tail seq)))))"
			"(define mid (lambda (seq) (/ (length seq) 2)))"
			"((combine append) (take (mid deck) deck) (drop (mid deck) deck)))))", "<Lambda>");
		TEST("(riff-shuffle (list 1 2 3 4 5 6 7 8))", "(1 5 2 6 3 7 4 8)");
		TEST("((repeat riff-shuffle) (list 1 2 3 4 5 6 7 8))", "(1 3 5 7 2 4 6 8)");
		TEST("(riff-shuffle (riff-shuffle (riff-shuffle (list 1 2 3 4 5 6 7 8))))", "(1 2 3 4 5 6 7 8)");
		std::cout
			<< "total tests " << g_test_count
			<< ", total failures " << g_fault_count
			<< "\n";
		return g_fault_count ? EXIT_FAILURE : EXIT_SUCCESS;
	}
}

using namespace PocoLithp;

int main()
{
	Poco::Dynamic::Struct<int> atomTable;

	atomTable[0] = 1;
	atomTable[5] = "??";

	LithpVar a = 4, b = 2;
	LithpVar c = a * b;
	std::cout << "A: " << a.str() << ", B: " << b.str() << ", C: " << c.str() << std::endl;
	//scheme_main();
	//scheme_test();
	//scheme_complete_test();
	//plithp_test();
	//plithp_abs_test();
	plithp_complete_test();
	//plithp_main();
    return 0;
}

