#include "stdafx.h"

PocoLithp::AtomMapById_t atomMapById;
PocoLithp::AtomMapByName_t atomMapByName;

typedef std::list<PocoLithp::add_environment_proc> environment_procs_t;
environment_procs_t environment_procs;

// For when you really need to dig deep into the behaviour of the interpreter.
#if 1
#define TRACE_DEBUG(x)
#else
#define TRACE_DEBUG(x) do { x; } while(0);
#endif

namespace PocoLithp {
	const LithpCell sym_false(Atom, "false");
	const LithpCell sym_true(Atom, "true");
	const LithpCell sym_nil(Atom, "nil");
	const LithpCell sym_quote(Atom, "quote");
	const LithpCell sym_if(Atom, "if");
	const LithpCell sym_get(Atom, "get!");
	const LithpCell sym_set(Atom, "set!");
	const LithpCell sym_define(Atom, "define");
	const LithpCell sym_defined(Atom, "defined");
	const LithpCell sym_lambda(Atom, "lambda");
	const LithpCell sym_lambda2(Atom, "#");   // Alternative to lambda
	const LithpCell sym_macro(Atom, "macro");
	const LithpCell sym_begin(Atom, "begin");
	const LithpCell sym_receive(Atom, "receive");
	const LithpCell sym_sleep(Atom, "sleep");
	const LithpCell sym_after(Atom, "after");
	const LithpCell sym_infinity(Atom, "infinity");

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
		if(c.size() == 0) return LithpCell(Var, 0);
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
		if(c.size() == 0) return LithpCell(Var, 0);
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
		if(c.size() == 0) return LithpCell(Var, 0);
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i) {
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
		if(c.size() == 0) return LithpCell(Var, 0);
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
		TRACE_DEBUG(std::cerr << "Greater ..." << std::endl);
		if(c.size() < 2) return sym_false;
		LithpCell n(c[0]);
		TRACE_DEBUG(std::cerr << "Greater: n=" << n.str() << ", tag: " << n.tag << std::endl);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i) {
			TRACE_DEBUG(std::cerr << "Greater: *i=" << i->str() << ", tag: " << i->tag << std::endl);
			if (n <= *i)
				return sym_false;
		}
		TRACE_DEBUG(std::cerr << "Greater: not greater" << std::endl);
		return sym_true;
	}

	LithpCell proc_greater_equal(const LithpCells &c)
	{
		if(c.size() < 2) return sym_false;
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i)
			if (n < *i)
				return sym_false;
		return sym_true;
	}

	LithpCell proc_less(const LithpCells &c)
	{
		if(c.size() < 2) return sym_false;
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i) {
			if (n >= *i)
				return sym_false;
		}
		return sym_true;
	}

	LithpCell proc_less_equal(const LithpCells &c)
	{
		if(c.size() < 2) return sym_false;
		LithpCell n(c[0]);
		for (LithpCells::const_iterator i = c.begin() + 1; i != c.end(); ++i) {
			if (n > *i)
				return sym_false;
		}
		return sym_true;
	}

	LithpCell proc_equal(const LithpCells &c)
	{
		if(c.size() < 2) return sym_false;
		return booleanCell(c[0] == c[1]);
	}

	LithpCell proc_not_equal(const LithpCells &c)
	{
		if(c.size() < 2) return sym_false;
		return booleanCell(c[0] != c[1]);
	}

	LithpCell proc_not(const LithpCells &c)
	{
		if(c.size() == 0) return sym_true;
		return booleanCell(!(c[0] == sym_true));
	}

	// Get or set debug state
	LithpCell proc_debug(const LithpCells &c)
	{
		bool was = GetDEBUG();
		if (c.size() != 0)
			SetDEBUG(booleanVal(c[0]));
		return booleanCell(was);
	}

	// Get or set timing state
	LithpCell proc_timing(const LithpCells &c)
	{
		bool was = GetTIMING();
		if (c.size() != 0)
			SetTIMING(booleanVal(c[0]));
		return booleanCell(was);
	}

	// Quit interpreter
	LithpCell proc_quit(const LithpCells &c)
	{
		SetQUIT(true);
		return sym_true;
	}

	// Get number of reductions
	LithpCell proc_reds(const LithpCells &c) {
		return LithpCell(Var, 0); // reductions);
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
		TRACK_STATS(return LithpCell(Var, GetDepth()));
		return LithpCell(Atom, "#stats_not_tracked");
	}

	// Get max eval depth
	LithpCell proc__depth_max(const LithpCells &c) {
		TRACK_STATS(return LithpCell(Var, GetDepthMax()));
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
		std::string prompt = "> ";
		if(c.size() > 0)
			prompt = c[0].str();
		std::string line = GETLINE(prompt);
		return LithpCell(Var, line);
	}

	// Invoke REPL
	LithpCell proc_repl(const LithpCells &c, Env_p env) {
		std::string prompt = "Elisp> ";
		if (c.size() == 1)
			prompt = c[0].str();
		return repl(prompt, env);
	}

	// Tokenize the given string
	LithpCell proc__tokenize(const LithpCells &c) {
		if(c.size() == 0) return sym_nil;
		std::list<std::string> tokens = tokenize(c[0].str());
		return read_from(tokens);
	}

	/** Variable procedures */

	const LithpCell tag_atom(Atom, "atom");
	const LithpCell tag_number(Atom, "number");
	const LithpCell tag_string(Atom, "string");
	const LithpCell tag_other(Atom, "other");
	const LithpCell tag_lambda(Atom, "lambda");
	const LithpCell tag_macro(Atom, "macro");
	const LithpCell tag_proc(Atom, "proc");
	const LithpCell tag_list(Atom, "list");
	const LithpCell tag_dict(Atom, "dict");
	const LithpCell tag_thread(Atom, "thread");

	// Get the tag of the given variable
	LithpCell proc_tag(const LithpCells &c) {
		if (c.size() != 1)
			return getAtom("too_many_args");
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
		case Macro:
			return tag_macro;
		case Proc:
		case ProcExtended:
		case ProcImplementation:
			return tag_proc;
		case List:
			return tag_list;
		case Dict:
			return tag_dict;
		case Thread:
			return tag_thread;
		default:
			return getAtom("unknown");
		}
	}

	// Export given environment symbols to the topmost environment.
	// @param (list atom())
	LithpCell proc_export(const LithpCells &args, Env_p env) {
		Env_p topmost = env->getTopmost(env);
		unsigned count = 0;
		for (auto it = args.cbegin(); it != args.cend(); ++it, ++count) {
			// Expects atoms
			if (GetDEBUG())
				std::cerr << "!  exporting symbol: " << it->atomid() << std::endl;
			topmost->operator[](it->atomid()) = env->operator[](it->atomid());
		}
		return LithpCell(Var, count);
	}

	void add_environment_runtime(add_environment_proc p) {
		environment_procs.push_back(p);
	}

	// define the bare minimum set of primitives necessary to pass the unit tests
	void add_globals(LithpEnvironment &env)
	{
		env["+"] = LithpCell(&proc_add);                env["-"] = LithpCell(&proc_sub);
		env["*"] = LithpCell(&proc_mul);                env["/"] = LithpCell(&proc_div);
		env[">"] = LithpCell(&proc_greater);            env["<"] = LithpCell(&proc_less);
		env[">="] = LithpCell(&proc_greater_equal);     env["<="] = LithpCell(&proc_less_equal);
		env["="] = env["=="] = LithpCell(&proc_equal);  env["!="] = LithpCell(&proc_not_equal);
		env["!"] = LithpCell(&proc_not);
		env["debug"] = LithpCell(&proc_debug);          env["timing"] = LithpCell(&proc_timing);
		env["q"] = env["quit"] = LithpCell(&proc_quit);
		env["reds"] = LithpCell(&proc_reds);            env["tests"] = LithpCell(&proc_tests);
		env["env"] = LithpCell(&proc_env);
		env["_depth"] = LithpCell(&proc__depth);        env["_depth_max"] = LithpCell(&proc__depth_max);

		// IO
		env["print"] = LithpCell(&proc_print);
		env["getline"] = LithpCell(&proc_getline);
		// Stackless interpreter defines this in ELthreads.cpp
		env["repl"] = LithpCell(&proc_repl);
		env["_tokenize"] = LithpCell(&proc__tokenize);

		// File IO

		// Variable information
		env["tag"] = LithpCell(&proc_tag);

		// Export functions
		env["export"] = LithpCell(&proc_export);
		
		// Add any other procs
		for (auto it = environment_procs.begin(); it != environment_procs.end(); ++it)
			(*it)(env);
	}
}
