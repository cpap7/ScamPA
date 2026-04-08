#include "spapch.h"
#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace SPA {
	std::shared_ptr<spdlog::logger> CLogger::s_core_logger;
	std::shared_ptr<spdlog::logger> CLogger::s_client_logger;

	std::shared_ptr<CImGuiConsoleSink> CLogger::s_core_console_sink;
	std::shared_ptr<CImGuiConsoleSink> CLogger::s_client_console_sink;

	void CLogger::Init() {
		// Create stdout color sinks
		auto core_stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		auto client_stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

		// Create ImGui console sinks
		s_core_console_sink = std::make_shared<CImGuiConsoleSink>();
		s_client_console_sink = std::make_shared<CImGuiConsoleSink>();

		// Core logger (stdout + console panel)
		std::vector<spdlog::sink_ptr> core_sinks = { core_stdout_sink, s_core_console_sink };
		s_core_logger = std::make_shared<spdlog::logger>("[CORE]", core_sinks.begin(), core_sinks.end());
		s_core_logger->set_level(spdlog::level::trace);
		s_core_logger->set_pattern("%^[%T] %n: %v%$");

		// Client logger (stdout + console panel)
		std::vector<spdlog::sink_ptr> client_sinks = { client_stdout_sink, s_client_console_sink };
		s_client_logger = std::make_shared<spdlog::logger>("[CLIENT]", client_sinks.begin(), client_sinks.end());
		s_client_logger->set_level(spdlog::level::trace);
		s_client_logger->set_pattern("%^[%T] %n: %v%$");

		// Register with spdlog 
		spdlog::register_logger(s_core_logger);
		spdlog::register_logger(s_client_logger);
	}
}