#pragma once

#include <iostream>
#include <string>
#include <chrono>

namespace SPA {

	class CTimer {
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;

	public:
		inline CTimer() {
			Reset();
		}

		inline void Reset() {
			m_start_time = std::chrono::high_resolution_clock::now();
		}

		inline float GetTimeElapsed() {
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_start_time).count() * 0.001f * 0.001f * 0.001f;
		}

		inline float GetTimeElapsedMillis() {
			return GetTimeElapsed() * 1000.0f;
		}
	};

	class CScopedTimer {
	private:
		std::string m_name;
		CTimer m_timer;

	public:
		inline CScopedTimer(const std::string& a_name)
			: m_name(a_name) {}

		inline ~CScopedTimer() {
			float time = m_timer.GetTimeElapsedMillis();
			std::cout << "(TIMER) " << m_name << " - " << time << "ms\n";
		}
	};
}
