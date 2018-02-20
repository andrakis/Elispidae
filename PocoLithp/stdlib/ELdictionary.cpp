/**
 * Standard Library: Dictionary functions
 */

#include "stdafx.h"

using namespace PocoLithp;
using namespace PocoLithp::Stackless;

// Get keys from dictionary
LithpCell proc_keys(const LithpCells &c) {
	if(c.empty()) throw InvalidArgumentException("Not enough parameters for keys(::dict)");
	const LithpDict &dict = c[0].dict();
	LithpCells keys;
	for(auto it = dict.begin(); it != dict.end(); ++it)
		keys.push_back(LithpCell(Atom, it->first));
	return LithpCell(List, keys);
}

// Get values from dictionary
LithpCell proc_values(const LithpCells &c) {
	if(c.empty()) throw InvalidArgumentException("Not enough parameters for values(::dict)");
	const LithpDict &dict = c[0].dict();
	LithpCells values;
	for(auto it = dict.begin(); it != dict.end(); ++it)
		values.push_back(LithpCell(Var, it->second));
	return LithpCell(List, values);
}


void Elispidae::Stdlib::init_dictionary() {
	add_environment_runtime([](LithpEnvironment &env) {
		env["dict:keys"] = LithpCell(&proc_keys);
		env["dict:values"] = LithpCell(&proc_values);
	});
}
