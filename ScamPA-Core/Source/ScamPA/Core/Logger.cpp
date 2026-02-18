#include "spapch.h"
#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace SPA {
	std::shared_ptr<spdlog::logger> CLogger::s_core_logger;
	std::shared_ptr<spdlog::logger> CLogger::s_client_logger;

	void CLogger::Init() {
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_core_logger = spdlog::stdout_color_mt("[CORE]");
		s_client_logger = spdlog::stdout_color_mt("[CLIENT]");

		s_core_logger->set_level(spdlog::level::trace);
		s_client_logger->set_level(spdlog::level::trace);
	}
}