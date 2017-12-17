/**
 * Standard Library: String functions
 */

#include "stdafx.h"

using namespace PocoLithp;
using namespace PocoLithp::Stackless;


LithpCell proc_empty(const LithpCells &c) {
	if(c.size() == 0) return LithpCell(Var, 0);
	return c[0].empty() ? sym_true : sym_false;
}

LithpCell proc_length(const LithpCells &c) {
	if(c.size() == 0) return LithpCell(Var, 0);
	return LithpCell(Var, c[0].size());
}
LithpCell proc_nullp(const LithpCells &c) {
	if(c.size() == 0) return sym_true;
	return c[0].is_nullp() ? sym_true : sym_false;
}
LithpCell proc_head(const LithpCells &c) {
	if(c.size() == 0) return sym_nil;
	if(c[0].size() == 0) return sym_nil;
	return c[0][0];
}

LithpCell proc_tail(const LithpCells &c)
{
	if (c.size() == 0)
		return LithpCell(List, LithpCells());
	LithpCells result = c[0].list();
	if(result.size() == 0)
		return LithpCell(List, LithpCells());
	result.erase(result.begin());
	return LithpCell(List, result);
}

LithpCell proc_append(const LithpCells &c)
{
	if (c.size() == 0)
		return LithpCell(List, LithpCells());
	else if(c.size() == 1)
		return LithpCell(List, c[0].list());
	LithpCells result = c[0].list();
	const LithpCells &c1l = c[1].list();
	for (LithpCells::const_iterator i = c1l.begin(); i != c1l.end(); ++i)
		result.push_back(*i);
	return LithpCell(List, result);
}

LithpCell proc_cons(const LithpCells &c)
{
	if (c.size() == 0)
		return LithpCell(List, LithpCells());
	else if(c.size() == 1)
		return LithpCell(List, c[0].list());
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

void Elispidae::Stdlib::init_lists() {
	add_environment_runtime([](LithpEnvironment &env) {
		env["append"] = LithpCell(&proc_append);        env["head"] = LithpCell(&proc_head);
		env["tail"] = LithpCell(&proc_tail);            env["cons"] = LithpCell(&proc_cons);
		env["length"] = LithpCell(&proc_length);        env["list"] = LithpCell(&proc_list);
		env["null?"] = LithpCell(&proc_nullp);
		env["empty?"] = LithpCell(&proc_empty);
	});
}
