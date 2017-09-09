#pragma once

#include <cstddef>
#if defined( _MSC_VER )
#include <intrin.h>
#endif

static inline size_t query_intel_x86_eflags(const size_t query_bit_mask)
{
#if defined( _MSC_VER )
	return __readeflags() & query_bit_mask;
#elif defined( __GNUC__ )
	// this code will work only on 64-bit GNU-C machines;
	// Tested and does NOT work with Intel C++ 10.1!
	size_t eflags;
	__asm__ __volatile__(
		"pushfq \n\t"
		"pop %%rax\n\t"
		"movq %%rax, %0\n\t"
		:"=r"(eflags)
		:
		: "%rax"
	);
	return eflags & query_bit_mask;
#else
#pragma message("No inline assembly will work with this compiler, overflow check skipped")
	return 0;
#endif
}

static inline bool query_overflow_flag() {
	/*
	  If the operands were multiplied without overflow, you would get a return value of 0 from
	  query_intel_eflags( 0x801 ), i.e. neither the carry nor the overflow flags are set.
	  An overflow occurs and the both flags are set to 1. This check does not imply any further
	  calculations, so it should be quite fast.
	*/
	return query_intel_x86_eflags(0x801) != 0;
}