/**
 * Standard Library: Dictionary functions
 */

#include "stdafx.h"

using namespace PocoLithp;
using namespace PocoLithp::Stackless;

// Create a dict
// (dict() -> dict())
// TODO: take initial values and set them
LithpCell proc_dict(const LithpCells &c) {
	return LithpCell(Dict, LithpDict());
}

// Get keys from dictionary
// (dict:keys dict()) -> list(Keys::atom())
LithpCell proc_keys(const LithpCells &c) {
	if(c.empty()) throw InvalidArgumentException("Not enough parameters for keys(::dict)");
	const LithpDict &dict = c[0].dict();
	LithpCells keys;
	for(auto it = dict.begin(); it != dict.end(); ++it)
		keys.push_back(LithpCell(Atom, it->first));
	return LithpCell(List, keys);
}

// Get values from dictionary
// (dict:values dict()) -> list(Values::atom())
LithpCell proc_values(const LithpCells &c) {
	if(c.empty()) throw InvalidArgumentException("Not enough parameters for values(::dict)");
	const LithpDict &dict = c[0].dict();
	LithpCells values;
	for (auto it = dict.begin(); it != dict.end(); ++it)
		values.push_back(it->second);
	return LithpCell(List, values);
}

// Set a value in a dictionary
// (dict:set A::dict() Key::atom() Value::any()) -> A:dict():modified
LithpCell proc_dictset(const LithpCells &c) {
	auto it = c.cbegin();
	if (it == c.cend()) return LithpCell(Dict, LithpDict());
	if (it->tag != Dict) throw new InvalidArgumentException("Not a dict");
	const LithpCell fromDict = *it; ++it;
	if (it == c.cend()) throw new InvalidArgumentException("Missing key");
	const LithpCell key = *it; ++it;
	if (key.tag != Atom) throw new InvalidArgumentException("Key should be an atom");
	if (it == c.cend()) throw new InvalidArgumentException("Missing value");
	const LithpCell value = *it; ++it;
	LithpDict dict = fromDict.dict();
	dict.operator[](key.atomid()) = value;
	return LithpCell(Dict, dict);
}

// Get a value in a dictionary, or Default
// (dict:get A::dict() Key::atom() Default::any()) -> Default | any()
LithpCell proc_dictget(const LithpCells &c) {
	auto it = c.cbegin();
	if (it == c.cend()) return LithpCell(Dict, LithpDict());
	const LithpCell fromDict = *it; ++it;
	if (fromDict.tag != Dict) throw new InvalidArgumentException("Not a dict");
	if (it == c.cend()) throw new InvalidArgumentException("Missing key");
	const LithpCell key = *it; ++it;
	if (key.tag != Atom) throw new InvalidArgumentException("Key should be an atom");
	if (it == c.cend()) throw new InvalidArgumentException("Missing value");
	LithpCell def = sym_nil;
	if (it != c.cend()) def = *it; ++it;
	LithpDict dict = fromDict.dict();
	auto vit = dict.find(key.atomid());
	if (vit == dict.end())
		return def;
	else
		return *vit;
}

void Elispidae::Stdlib::init_dictionary() {
	add_environment_runtime([](LithpEnvironment &env) {
		env["dict"] = LithpCell(&proc_dict);
		env["dict:keys"] = LithpCell(&proc_keys);
		env["dict:values"] = LithpCell(&proc_values);
		env["dict:set"] = LithpCell(&proc_dictset);
		env["dict:get"] = LithpCell(&proc_dictget);
	});
}
