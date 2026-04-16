#pragma once

#include <vector>
#include <mutex>
#include <cstdint>

namespace SPA {
	// Thread-safe resources used by audio I/O classes for capturing samples
	
	struct SAudioInput {
		mutable std::mutex m_mutex;
		std::vector<int16_t> m_buffer;	// Input --> STT
	};

	struct SAudioOutput {
		mutable std::mutex m_mutex;
		std::vector<int16_t> m_buffer;  // TTS --> Output
		size_t m_read_cursor = 0;
	};
}