#include "spapch.h"
#include "UUID.h"

#include <random>

namespace SPA {
	// Random number generator
	static std::random_device s_random_device;
	
	static std::mt19937_64 s_rng_engine64(s_random_device());
	static std::mt19937 s_rng_engine32(s_random_device());

	static std::uniform_int_distribution<uint64_t> s_uniform_distribution64;
	static std::uniform_int_distribution<uint32_t> s_uniform_distribution32;

	CUUID64::CUUID64() 
		: m_uuid(s_uniform_distribution64(s_rng_engine64)) {
	}

	CUUID64::CUUID64(uint64_t a_uuid)
		: m_uuid(a_uuid) {
	}

	CUUID32::CUUID32() 
		: m_uuid(s_uniform_distribution32(s_rng_engine32)) {
	}

	CUUID32::CUUID32(uint32_t a_uuid) 
		: m_uuid(a_uuid) {
	}
}