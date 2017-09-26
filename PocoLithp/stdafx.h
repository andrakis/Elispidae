// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <cctype>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <Poco/Dynamic/Var.h>
#include <Poco/Dynamic/Pair.h>
#include <Poco/Dynamic/Struct.h>
#include <Poco/NumberParser.h>

std::string linenoise_getline(const std::string &);
std::string stdin_getline(const std::string &);

#ifdef READLINE_NG
#include "linenoise.h"
#define GETLINE(PROMPT)       linenoise_getline(PROMPT)
#define GETLINE_INIT()        linenoise_init()
#else
#define GETLINE(PROMPT)       stdin_getline(PROMPT)
#define GETLINE_INIT()
#endif

#include "PocoLithp.hpp"
