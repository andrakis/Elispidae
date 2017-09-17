#include "stdafx.h"

PocoLithp::AtomMapById_t atomMapById;
PocoLithp::AtomMapByName_t atomMapByName;

namespace PocoLithp {
	const LithpCell sym_false(Atom, "#f");
	const LithpCell sym_true(Atom, "#t");
	const LithpCell sym_nil(Atom, "nil");
	const LithpCell sym_quote(Atom, "quote");
	const LithpCell sym_if(Atom, "if");
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

	LithpCell proc_add(const LithpCells &c, LithpEnvironment *env)
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

	LithpCell proc_sub(const LithpCells &c, LithpEnvironment *env)
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

	LithpCell proc_mul(const LithpCells &c, LithpEnvironment *env)
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

	LithpCell proc_div(const LithpCells &c, LithpEnvironment *env)
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

	LithpCell proc_greater(const LithpCells &c, LithpEnvironment *env)
	{
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i)
			if (n <= *i)
				return sym_false;
		return sym_true;
	}

	LithpCell proc_greater_equal(const LithpCells &c, LithpEnvironment *env)
	{
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i)
			if (n < *i)
				return sym_false;
		return sym_true;
	}

	LithpCell proc_less(const LithpCells &c, LithpEnvironment *env)
	{
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i) {
			if (n >= *i)
				return sym_false;
		}
		return sym_true;
	}

	LithpCell proc_less_equal(const LithpCells &c, LithpEnvironment *env)
	{
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i) {
			if (n > *i)
				return sym_false;
		}
		return sym_true;
	}

	LithpCell proc_equal(const LithpCells &c, LithpEnvironment *env)
	{
		return booleanCell(c[0] == c[1]);
	}

	LithpCell proc_not_equal(const LithpCells &c, LithpEnvironment *env)
	{
		return booleanCell(c[0] != c[1]);
	}

	LithpCell proc_length(const LithpCells &c, LithpEnvironment *env) { return LithpCell(Var, c[0].list().size()); }
	LithpCell proc_nullp(const LithpCells &c, LithpEnvironment *env) { return c[0].is_nullp() ? sym_true : sym_false; }
	LithpCell proc_head(const LithpCells &c, LithpEnvironment *env) { return c[0].list()[0]; }

	LithpCell proc_tail(const LithpCells &c, LithpEnvironment *env)
	{
		if (c[0].list().size() < 2)
			return sym_nil;
		LithpCells result = c[0].list();
		result.erase(result.begin());
		return LithpCell(List, result);
	}

	LithpCell proc_append(const LithpCells &c, LithpEnvironment *env)
	{
		LithpCells result = c[0].list();
		const LithpCells &c1l = c[1].list();
		for (LithpCells::const_iterator i = c1l.begin(); i != c1l.end(); ++i)
			result.push_back(*i);
		return LithpCell(List, result);
	}

	LithpCell proc_cons(const LithpCells &c, LithpEnvironment *env)
	{
		LithpCells result;
		const LithpCells &c1l = c[1].list();
		result.push_back(c[0]);
		for (LithpCells::const_iterator i = c1l.begin(); i != c1l.end(); ++i)
			result.push_back(*i);
		return LithpCell(List, result);
	}

	LithpCell proc_list(const LithpCells &c, LithpEnvironment *env)
	{
		return LithpCell(List, c);
	}

	// Get or set debug state
	LithpCell proc_debug(const LithpCells &c, LithpEnvironment *env)
	{
		bool was = DEBUG;
		if (c.size() != 0)
			DEBUG = booleanVal(c[0]);
		return booleanCell(was);
	}

	// Get or set timing state
	LithpCell proc_timing(const LithpCells &c, LithpEnvironment *env)
	{
		bool was = TIMING;
		if (c.size() != 0)
			TIMING = booleanVal(c[0]);
		return booleanCell(was);
	}

	// Quit interpreter
	LithpCell proc_quit(const LithpCells &c, LithpEnvironment *env)
	{
		QUIT = true;
		return sym_true;
	}

	// Get number of reductions
	LithpCell proc_reds(const LithpCells &c, LithpEnvironment *env) {
		return LithpCell(Var, reductions);
	}

	// Run tests
	LithpCell proc_tests(const LithpCells &c, LithpEnvironment *env) {
		return LithpCell(Var, Test::RunTests());
	}

	// Get environment values
	LithpCell proc_env(const LithpCells &c, LithpEnvironment *env) {
		return LithpCell(List, env->getCompleteEnv());
	}

	// Get current eval depth
	LithpCell proc__depth(const LithpCells &c, LithpEnvironment *env) {
		TRACK_STATS(return LithpCell(Var, depth));
		return LithpCell(Atom, "#stats_not_tracked");
	}

	// Get max eval depth
	LithpCell proc__depth_max(const LithpCells &c, LithpEnvironment *env) {
		TRACK_STATS(return LithpCell(Var, depth_max));
		return LithpCell(Atom, "#stats_not_tracked");
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
	}
}
