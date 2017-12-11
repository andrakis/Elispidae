#include "stdafx.h"

#include "ELisp.hpp"

namespace Elispidae {
	void init_runtime() {
		Threads::init_threads();
	}
}