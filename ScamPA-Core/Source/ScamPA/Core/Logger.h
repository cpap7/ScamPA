#pragma once
#include "Base.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/ostream.h>

#include <glm/glm.hpp>

namespace SPA {
	class CLogger {
	private:
		static std::shared_ptr<spdlog::logger> s_core_logger;
		static std::shared_ptr<spdlog::logger> s_client_logger;

	public:
		static void Init();

		static inline std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_core_logger; }
		static inline std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_client_logger; }


	};
}

template<typename OStream>
OStream& operator <<(OStream& a_ostream, const glm::vec2& a_vector) {
	return a_ostream << '(' << a_vector.x << ", " << a_vector.y << ")";
}

template<typename OStream>
OStream& operator <<(OStream& a_ostream, const glm::vec3& a_vector) {
	return a_ostream << '(' << a_vector.x << ", " << a_vector.y << ", " << a_vector.z << ")";
}

template<typename OStream>
OStream& operator <<(OStream& a_ostream, const glm::vec4& a_vector) {
	return a_ostream << '(' << a_vector.x << ", " << a_vector.y << ", " << a_vector.z << ", " << a_vector.w << ")";
}

// Core logging macros
#define SPA_CORE_TRACE(...)			SPA::CLogger::GetCoreLogger()->trace(__VA_ARGS__)
#define SPA_CORE_INFO(...)			SPA::CLogger::GetCoreLogger()->info(__VA_ARGS__)
#define SPA_CORE_WARN(...)			SPA::CLogger::GetCoreLogger()->warn(__VA_ARGS__)
#define SPA_CORE_ERROR(...)			SPA::CLogger::GetCoreLogger()->error(__VA_ARGS__)
#define SPA_CORE_CRITICAL(...)		SPA::CLogger::GetCoreLogger()->critical(__VA_ARGS__)

// Client logging macros
#define SPA_CLIENT_TRACE(...)		SPA::CLogger::GetClientLogger()->trace(__VA_ARGS__)
#define SPA_CLIENT_INFO(...)		SPA::CLogger::GetClientLogger()->info(__VA_ARGS__)
#define SPA_CLIENT_WARN(...)		SPA::CLogger::GetClientLogger()->warn(__VA_ARGS__)
#define SPA_CLIENT_ERROR(...)		SPA::CLogger::GetClientLogger()->error(__VA_ARGS__)
#define SPA_CLIENT_CRITICAL(...)	SPA::CLogger::GetClientLogger()->critical(__VA_ARGS__)
