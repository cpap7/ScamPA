#pragma once

#include <cstdint>
#include <cstddef>

namespace SPA {
	class CUUID64 {
	private:
		uint64_t m_uuid;

	public:
		CUUID64();
		CUUID64(uint64_t a_uuid);
		CUUID64(const CUUID64&) = default;

		inline operator uint64_t() const { return m_uuid; }
	};

	class CUUID32 {
	private:
		uint32_t m_uuid;

	public:
		CUUID32();
		CUUID32(uint32_t a_uuid);
		CUUID32(const CUUID32&) = default;

		inline operator uint32_t() const { return m_uuid; }
	};
}

namespace std { 
	template<typename T> struct hash;

	// Template specialization
	// to allow these classes to be used in hashmap data structures (i.e., unordered_map)
	template<>
	struct hash<SPA::CUUID64> {
		std::size_t operator()(const SPA::CUUID64& a_uuid) const {
			return (uint64_t)a_uuid;
		}
	};

	template<>
	struct hash<SPA::CUUID32> {
		std::size_t operator()(const SPA::CUUID32& a_uuid) const {
			return (uint32_t)a_uuid;
		}
	};
}