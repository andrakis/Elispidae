#include "stdafx.h"

#include "ELisp.hpp"

namespace Elispidae {
	void init_runtime() {
		Threads::init_threads();
		Stdlib::init_dictionary();
		Stdlib::init_lists();
		Stdlib::init_strings();
	}
}