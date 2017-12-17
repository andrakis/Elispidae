/**
 * Standard Library: String functions
 */

#include "stdafx.h"

using namespace PocoLithp;
using namespace PocoLithp::Stackless;

// (asc String::str()) => AsciiCode::int()
// -desc Returns ASCII code of first character, or nil if no string provided
LithpCell proc_asc(const LithpCells &c) {
	if (c.empty())
		return sym_nil;
	const LithpCell &cell = *c.begin();
	const std::string str = cell.value.toString();
	// TODO: won't work with unicide.
	return LithpCell(Var, (UnsignedInteger)str[0]);
}

LithpCell proc_chr(const LithpCells &c) {
	if (c.empty())
		return sym_nil;
	const LithpCell &cell = *c.begin();
	// TODO: wont work with unicode.
	char x = 1;
	return LithpCell(Var, std::string(1, cell.value.convert<char>()));
}

// Explode a string into a list
LithpCell proc_expl(const LithpCells &c) {
	if(c.size() == 0) return LithpCell(List, LithpCells());
	std::string str = c[0].str();
	LithpCells list;
	for (auto it = str.begin(); it != str.end(); ++it)
		list.push_back(LithpCell(Var, std::string(1, *it)));
	return LithpCell(List, list);
}

// Convert argument to string
LithpCell proc_tostring(const LithpCells &c) {
	bool repre = false;
	if (c.size() == 0)
		return LithpCell(Var, std::string(""));
	if (c.size() == 2)
		repre = c[1] == sym_true ? true : false;
	return LithpCell(Var, to_string(c[0], true, repre));
}

void Elispidae::Stdlib::init_strings() {
	add_environment_runtime([](LithpEnvironment &env) {
		env["asc"] = LithpCell(&proc_asc);
		env["chr"] = LithpCell(&proc_chr);
		env["expl"] = LithpCell(&proc_expl);
		env["str"] = env["tostring"] = LithpCell(&proc_tostring);
	});
}