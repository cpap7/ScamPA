#include "spapch.h"
#include "Random.h"

namespace SPA {
	// Initialize static variables here

	std::mt19937 CRandom::s_random_engine;
	std::uniform_int_distribution<std::mt19937::result_type> CRandom::s_distribution;

}