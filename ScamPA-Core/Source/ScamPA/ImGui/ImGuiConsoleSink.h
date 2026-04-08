#pragma once
#include <spdlog/sinks/base_sink.h>
#include <spdlog/details/log_msg.h>

#include <mutex>
#include <vector>
#include <string>

namespace SPA {

	struct SConsoleSinkMessage {
		std::string m_message;
		spdlog::level::level_enum m_level = spdlog::level::trace;
	};

	// Custom spdlog sink that buffers formatted log messages for ImGui display
	class CImGuiConsoleSink : public spdlog::sinks::base_sink<std::mutex> {
	private:
		std::vector<SConsoleSinkMessage> m_pending_messages;

	public:
		// Drains all pending messages
		// Called once per frame from the render thread
		std::vector<SConsoleSinkMessage> Drain() {
			std::lock_guard<std::mutex> lock(mutex_);
			std::vector<SConsoleSinkMessage> out;
			std::swap(out, m_pending_messages);
			return out;
		}

	protected:
		void sink_it_(const spdlog::details::log_msg& a_msg) override {
			// Use the raw payload (no timestamp/level prefix since the panel handles presentation)
			m_pending_messages.push_back({ std::string(a_msg.payload.data(), a_msg.payload.size()), a_msg.level });
		}

		void flush_() override {}

	
	};

}
