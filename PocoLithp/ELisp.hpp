#pragma once

#include "stdafx.h"

#ifdef ELISP_STACKLESS
#include "PLint_stackless.hpp"
#endif

namespace Elispidae {
	void init_runtime();

	namespace Threads {
		void init_threads();
	}

	namespace Stdlib {
		void init_dictionary();
		void init_lists();
		void init_strings();
	}
}