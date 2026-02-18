#pragma once

#include <random>

#include <glm/glm.hpp>

namespace SPA {

	class CRandom {
	private:
		static std::mt19937 s_random_engine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_distribution;

	public:
		static inline void Init() {
			s_random_engine.seed(std::random_device()());
		}

		static inline uint32_t UInt() {
			return s_distribution(s_random_engine);
		}

		static inline uint32_t UInt(uint32_t min, uint32_t max) {
			return min + (s_distribution(s_random_engine) % (max - min + 1));
		}

		static inline float Float() {
			return (float)s_distribution(s_random_engine) / (float)std::numeric_limits<uint32_t>::max();
		}

		static inline glm::vec3 Vec3() {
			return glm::vec3(Float(), Float(), Float());
		}

		static inline glm::vec3 Vec3(float min, float max) {
			return glm::vec3(Float() * (max - min) + min, Float() * (max - min) + min, Float() * (max - min) + min);
		}

		static inline glm::vec3 InUnitSphere() {
			return glm::normalize(Vec3(-1.0f, 1.0f));
		}
	
	};

}


