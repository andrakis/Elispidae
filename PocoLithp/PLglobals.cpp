#include "stdafx.h"

PocoLithp::AtomMapById_t atomMapById;
PocoLithp::AtomMapByName_t atomMapByName;

namespace PocoLithp {
	const LithpCell sym_false(Atom, "#f");
	const LithpCell sym_true(Atom, "#t");
	const LithpCell sym_nil(Atom, "nil");
	const LithpCell sym_quote(Atom, "quote");
	const LithpCell sym_if(Atom, "if");
	const LithpCell sym_get(Atom, "get!");
	const LithpCell sym_set(Atom, "set!");
	const LithpCell sym_define(Atom, "define");
	const LithpCell sym_lambda(Atom, "lambda");
	const LithpCell sym_begin(Atom, "begin");

	std::string to_string(const LithpCell & exp);

	const LithpCell booleanCell(const bool val) {
		return val ? sym_true : sym_false;
	}
	bool booleanVal(const LithpCell &val) {
		return val == sym_true;
	}

	std::string getAtomById(atomId id) {
		return atomMapById[id];
	}

	atomId getAtomId(const std::string &name) {
		AtomMapByName_t::const_iterator it = atomMapByName.find(name);
		if (it != atomMapByName.end())
			return it->second;
		atomId id = (atomId)atomMapByName.size();
		atomMapByName.emplace(name, id);
		atomMapById.emplace(id, name);
		return id;
	}
	LithpCell getAtom(const std::string &name) {
		return LithpCell(Atom, getAtomId(name));
	}

	LithpCell proc_add(const LithpCells &c)
	{
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i) {
			// Signedness promotion check
			if (i->value.isSigned() == true && n.value.isSigned() == false) {
				n.value = n.value.convert<SignedInteger>();
			}
			n += *i;
		}
		return n;
	}

	LithpCell proc_sub(const LithpCells &c)
	{
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i) {
			// Signedness promotion check
			if (i->value.isSigned() == true && n.value.isSigned() == false) {
				n.value = n.value.convert<SignedInteger>();
			}
			n -= *i;
		}
		return n;
	}

	LithpCell proc_mul(const LithpCells &c)
	{
		LithpCell n(1);
		for (LithpCells::const_iterator i = c.begin(); i != c.end(); ++i) {
			// Signedness promotion check
			if (i->value.isSigned() == true && n.value.isSigned() == false) {
				n.value = n.value.convert<SignedInteger>();
			}
			n *= *i;
		}
		return n;
	}

	LithpCell proc_div(const LithpCells &c)
	{
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i) {
			// Signedness promotion check
			if (i->value.isSigned() == true && n.value.isSigned() == false) {
				n.value = n.value.convert<SignedInteger>();
			}
			n /= *i;
		}
		return n;
	}

	LithpCell proc_greater(const LithpCells &c)
	{
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i)
			if (n <= *i)
				return sym_false;
		return sym_true;
	}

	LithpCell proc_greater_equal(const LithpCells &c)
	{
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i)
			if (n < *i)
				return sym_false;
		return sym_true;
	}

	LithpCell proc_less(const LithpCells &c)
	{
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i) {
			if (n >= *i)
				return sym_false;
		}
		return sym_true;
	}

	LithpCell proc_less_equal(const LithpCells &c)
	{
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i) {
			if (n > *i)
				return sym_false;
		}
		return sym_true;
	}

	LithpCell proc_equal(const LithpCells &c)
	{
		return booleanCell(c[0] == c[1]);
	}

	LithpCell proc_not_equal(const LithpCells &c)
	{
		return booleanCell(c[0] != c[1]);
	}

	LithpCell proc_length(const LithpCells &c) { return LithpCell(Var, c[0].list().size()); }
	LithpCell proc_nullp(const LithpCells &c) { return c[0].is_nullp() ? sym_true : sym_false; }
	LithpCell proc_head(const LithpCells &c) { return c[0].list()[0]; }

	LithpCell proc_tail(const LithpCells &c)
	{
		if (c[0].list().size() < 2)
			return sym_nil;
		LithpCells result = c[0].list();
		result.erase(result.begin());
		return LithpCell(List, result);
	}

	LithpCell proc_append(const LithpCells &c)
	{
		LithpCells result = c[0].list();
		const LithpCells &c1l = c[1].list();
		for (LithpCells::const_iterator i = c1l.begin(); i != c1l.end(); ++i)
			result.push_back(*i);
		return LithpCell(List, result);
	}

	LithpCell proc_cons(const LithpCells &c)
	{
		LithpCells result;
		const LithpCells &c1l = c[1].list();
		result.push_back(c[0]);
		for (LithpCells::const_iterator i = c1l.begin(); i != c1l.end(); ++i)
			result.push_back(*i);
		return LithpCell(List, result);
	}

	LithpCell proc_list(const LithpCells &c)
	{
		return LithpCell(List, c);
	}

	// Get or set debug state
	LithpCell proc_debug(const LithpCells &c)
	{
		bool was = DEBUG;
		if (c.size() != 0)
			DEBUG = booleanVal(c[0]);
		return booleanCell(was);
	}

	// Get or set timing state
	LithpCell proc_timing(const LithpCells &c)
	{
		bool was = TIMING;
		if (c.size() != 0)
			TIMING = booleanVal(c[0]);
		return booleanCell(was);
	}

	// Quit interpreter
	LithpCell proc_quit(const LithpCells &c)
	{
		QUIT = true;
		return sym_true;
	}

	// Get number of reductions
	LithpCell proc_reds(const LithpCells &c) {
		return LithpCell(Var, reductions);
	}

	// Run tests
	LithpCell proc_tests(const LithpCells &c) {
		return LithpCell(Var, Test::RunTests());
	}

	// Get environment values
	LithpCell proc_env(const LithpCells &c, Env_p env) {
		return LithpCell(List, env->getCompleteEnv());
	}

	// Get current eval depth
	LithpCell proc__depth(const LithpCells &c) {
		TRACK_STATS(return LithpCell(Var, depth));
		return LithpCell(Atom, "#stats_not_tracked");
	}

	// Get max eval depth
	LithpCell proc__depth_max(const LithpCells &c) {
		TRACK_STATS(return LithpCell(Var, depth_max));
		return LithpCell(Atom, "#stats_not_tracked");
	}

	/** IO Procedures */

	// Print the given arguments
	LithpCell proc_print(const LithpCells &c) {
		std::stringstream ss;
		bool first = true;
		for (auto it = c.begin(); it != c.end(); ++it) {
			if (first) first = false;
			else ss << " ";
			ss << to_string(*it);
		}
		std::cout << ss.str() << "\n";
		return sym_nil;
	}

	// Read a line
	LithpCell proc_getline(const LithpCells &c) {
		std::string line; std::getline(std::cin, line);
		return LithpCell(Var, line);
	}

	// Invoke REPL
	LithpCell proc_repl(const LithpCells &c, Env_p env) {
		std::string prompt = "REPL> ";
		if (c.size() == 1)
			prompt = c[0].str();
		return repl(prompt, env);
	}

	// Call the parent eval function. It is named _eval since implementing a native
	// eval function is a goal of this interpreter. It would allow extending the
	// interpreter in classic Lisp ways.
	LithpCell proc__eval(const LithpCells &c, Env_p env) {
		return eval(c[0], env);
	}

	/** String procedures */

	// Convert argument to string
	LithpCell proc_tostring(const LithpCells &c) {
		bool repre = false;
		if (c.size() == 2)
			repre = c[1] == sym_true ? true : false;
		return LithpCell(Var, to_string(c[0], repre));
	}

	/** Variable procedures */

	const LithpCell tag_atom(Atom, "#atom");
	const LithpCell tag_number(Atom, "#number");
	const LithpCell tag_string(Atom, "#string");
	const LithpCell tag_other(Atom, "#other");
	const LithpCell tag_lambda(Atom, "#lambda");
	const LithpCell tag_proc(Atom, "#proc");
	const LithpCell tag_list(Atom, "#list");
	const LithpCell tag_dict(Atom, "#dict");

	// Get the tag of the given variable
	LithpCell proc_tag(const LithpCells &c) {
		if (c.size() != 1)
			return getAtom("#invalid_arg");
		const LithpCell &c0 = c[0];
		switch (c0.tag) {
		case Atom:
			return tag_atom;
		case Var:
			if (c0.value.isString())
				return tag_string;
			if (c0.value.isNumeric())
				return tag_number;
			return tag_other;
		case Lambda:
			return tag_lambda;
		case Proc:
			return tag_proc;
		case List:
			return tag_list;
		case Dict:
			return tag_dict;
		default:
			return getAtom("#unknown");
		}
	}

	// define the bare minimum set of primitives necessary to pass the unit tests
	void add_globals(LithpEnvironment &env)
	{
		env["nil"] = sym_nil;   env["#f"] = sym_false;  env["#t"] = sym_true;
		env["append"] = LithpCell(&proc_append);        env["head"] = LithpCell(&proc_head);
		env["tail"] = LithpCell(&proc_tail);            env["cons"] = LithpCell(&proc_cons);
		env["length"] = LithpCell(&proc_length);        env["list"] = LithpCell(&proc_list);
		env["null?"] = LithpCell(&proc_nullp);
		env["+"] = LithpCell(&proc_add);                env["-"] = LithpCell(&proc_sub);
		env["*"] = LithpCell(&proc_mul);                env["/"] = LithpCell(&proc_div);
		env[">"] = LithpCell(&proc_greater);            env["<"] = LithpCell(&proc_less);
		env[">="] = LithpCell(&proc_greater_equal);     env["<="] = LithpCell(&proc_less_equal);
		env["="] = env["=="] = LithpCell(&proc_equal);  env["!="] = LithpCell(&proc_not_equal);
		env["debug"] = LithpCell(&proc_debug);          env["timing"] = LithpCell(&proc_timing);
		env["q"] = env["quit"] = LithpCell(&proc_quit);
		env["reds"] = LithpCell(&proc_reds);            env["tests"] = LithpCell(&proc_tests);
		env["env"] = LithpCell(&proc_env);
		env["_depth"] = LithpCell(&proc__depth);        env["_depth_max"] = LithpCell(&proc__depth_max);

		// IO
		env["print"] = LithpCell(&proc_print);
		env["getline"] = LithpCell(&proc_getline);
		env["repl"] = LithpCell(&proc_repl);
		env["_eval"] = LithpCell(&proc__eval);
		
		// String
		env["str"] = env["tostring"] = LithpCell(&proc_tostring);

		// Variable information
		env["tag"] = LithpCell(&proc_tag);
	}
}
