#pragma once
#include "Base.h"
#include "LoggerCustomFormatters.h"
#include "../ImGui/ImGuiConsoleSink.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <map>

#define SPA_ASSERT_MESSAGE_BOX (!SPA_DIST && SPA_PLATFORM_WINDOWS)

#ifdef SPA_ASSERT_MESSAGE_BOX
	#ifdef SPA_PLATFORM_WINDOWS
		#include <Windows.h>
	#endif
#endif

namespace SPA {
	class CLogger {
	public:
		enum class EType : uint8_t {
			Core	= 0,
			Client	= 1,
		};
		
		enum class ELevel : uint8_t {
			Trace = 0,
			Info,
			Warn,
			Error,
			Fatal
		};
		
		struct STagDetails {
			ELevel m_level_filter = ELevel::Trace;
			bool m_enabled = true;
		};

	private:
		static std::shared_ptr<spdlog::logger> s_core_logger;
		static std::shared_ptr<spdlog::logger> s_client_logger;
		static std::shared_ptr<CImGuiConsoleSink> s_client_console_sink;
		static std::shared_ptr<CImGuiConsoleSink> s_core_console_sink;

		static inline std::map<std::string, STagDetails> s_enabled_tags;

	public:
		static void Init();

		static inline std::shared_ptr<spdlog::logger>& GetCoreLogger()		{ return s_core_logger; }
		static inline std::shared_ptr<spdlog::logger>& GetClientLogger()	{ return s_client_logger; }
		static inline std::shared_ptr<CImGuiConsoleSink>& GetCoreConsoleSink() { return s_core_console_sink; }
		static inline std::shared_ptr<CImGuiConsoleSink>& GetClientConsoleSink() { return s_client_console_sink; }

		static inline bool HasTag(const std::string& tag) { return s_enabled_tags.find(tag) != s_enabled_tags.end(); }
		static inline std::map<std::string, STagDetails>& EnabledTags() { return s_enabled_tags; }

		template<typename... Args>
		static void PrintMessage(CLogger::EType a_type, CLogger::ELevel a_level, std::string_view a_tag, std::string_view a_fmt, Args&&... a_args);

		template<typename... Args>
		static void PrintAssertMessage(CLogger::EType a_type, std::string_view a_prefix, Args&&... a_args);

	public: // Enum Utils
		static const char* LevelToString(ELevel a_level) {
			switch (a_level) {
				case ELevel::Trace: return "Trace";
				case ELevel::Info:	return "Info";
				case ELevel::Warn:	return "Warn";
				case ELevel::Error:	return "Error";
				case ELevel::Fatal:	return "Fatal";
			}
			return "Trace"; // Default
		}

		static ELevel StringToLevel(std::string_view a_level_string) {
			if (a_level_string == "Trace")	{ return ELevel::Trace; }
			if (a_level_string == "Info")	{ return ELevel::Info;	}
			if (a_level_string == "Warn")	{ return ELevel::Warn;	}
			if (a_level_string == "Error")	{ return ELevel::Error; }
			if (a_level_string == "Fatal")	{ return ELevel::Fatal; }

			return ELevel::Trace;
		}

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tagged logs (prefer these!)                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Core logging
#define SPA_CORE_TRACE_TAG(tag, ...) ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Core, ::SPA::CLogger::ELevel::Trace, tag, __VA_ARGS__)
#define SPA_CORE_INFO_TAG(tag, ...)  ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Core, ::SPA::CLogger::ELevel::Info, tag, __VA_ARGS__)
#define SPA_CORE_WARN_TAG(tag, ...)  ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Core, ::SPA::CLogger::ELevel::Warn, tag, __VA_ARGS__)
#define SPA_CORE_ERROR_TAG(tag, ...) ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Core, ::SPA::CLogger::ELevel::Error, tag, __VA_ARGS__)
#define SPA_CORE_FATAL_TAG(tag, ...) ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Core, ::SPA::CLogger::ELevel::Fatal, tag, __VA_ARGS__)

// Client logging
#define SPA_CLIENT_TRACE_TAG(tag, ...) ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Client, ::SPA::CLogger::ELevel::Trace, tag, __VA_ARGS__)
#define SPA_CLIENT_INFO_TAG(tag, ...)  ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Client, ::SPA::CLogger::ELevel::Info, tag, __VA_ARGS__)
#define SPA_CLIENT_WARN_TAG(tag, ...)  ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Client, ::SPA::CLogger::ELevel::Warn, tag, __VA_ARGS__)
#define SPA_CLIENT_ERROR_TAG(tag, ...) ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Client, ::SPA::CLogger::ELevel::Error, tag, __VA_ARGS__)
#define SPA_CLIENT_FATAL_TAG(tag, ...) ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Client, ::SPA::CLogger::ELevel::Fatal, tag, __VA_ARGS__)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Core Logging
#define SPA_CORE_TRACE(...)  ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Core, ::SPA::CLogger::ELevel::Trace, "", __VA_ARGS__)
#define SPA_CORE_INFO(...)   ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Core, ::SPA::CLogger::ELevel::Info, "", __VA_ARGS__)
#define SPA_CORE_WARN(...)   ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Core, ::SPA::CLogger::ELevel::Warn, "", __VA_ARGS__)
#define SPA_CORE_ERROR(...)  ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Core, ::SPA::CLogger::ELevel::Error, "", __VA_ARGS__)
#define SPA_CORE_FATAL(...)  ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Core, ::SPA::CLogger::ELevel::Fatal, "", __VA_ARGS__)

// Client Logging
#define SPA_CLIENT_TRACE(...)   ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Client, ::SPA::CLogger::ELevel::Trace, "", __VA_ARGS__)
#define SPA_CLIENT_INFO(...)    ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Client, ::SPA::CLogger::ELevel::Info, "", __VA_ARGS__)
#define SPA_CLIENT_WARN(...)    ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Client, ::SPA::CLogger::ELevel::Warn, "", __VA_ARGS__)
#define SPA_CLIENT_ERROR(...)   ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Client, ::SPA::CLogger::ELevel::Error, "", __VA_ARGS__)
#define SPA_CLIENT_FATAL(...)   ::SPA::CLogger::PrintMessage(::SPA::CLogger::EType::Client, ::SPA::CLogger::ELevel::Fatal, "", __VA_ARGS__)


/*
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
*/

namespace SPA {
	template<typename... Args>
	void CLogger::PrintMessage(CLogger::EType a_type, CLogger::ELevel a_level, std::string_view a_tag, std::string_view a_fmt, Args&&... a_args) {
		auto detail = s_enabled_tags[std::string(a_tag)];
		
		if (detail.m_enabled && detail.m_level_filter <= a_level) {
			auto logger = (a_type == EType::Core) ? GetCoreLogger() : GetClientLogger();
			std::string log_string = a_tag.empty() ? "{0}{1}" : "[{0}] {1}";
			std::string formatted_string = fmt::format(fmt::runtime(log_string), a_tag, fmt::vformat(a_fmt, fmt::make_format_args(a_args...)));

			switch (a_level) {
				case ELevel::Trace: logger->trace("{}", formatted_string);		break;
				case ELevel::Info:	logger->info("{}", formatted_string);		break;
				case ELevel::Warn:	logger->warn("{}", formatted_string);		break;
				case ELevel::Error: logger->error("{}", formatted_string);		break;
				case ELevel::Fatal: logger->critical("{}", formatted_string);	break;
			}
		}
	}

	template<typename... Args>
	void CLogger::PrintAssertMessage(CLogger::EType a_type, std::string_view a_prefix, Args&&... a_args) {
		auto logger = (a_type == EType::Core) ? GetCoreLogger() : GetClientLogger();
		logger->error("{0}: {1}", a_prefix, fmt::vformat(fmt::make_format_args(a_args...)));

#ifdef SPA_ASSERT_MESSAGE_BOX
		std::string message = fmt::vformat(fmt::make_format_args(a_args...));
		MessageBoxA(nullptr, message.c_str(), "ScamPA Assert", MB_OK | MB_ICONERROR);
#endif
	}

	template<>
	inline void CLogger::PrintAssertMessage(CLogger::EType a_type, std::string_view a_prefix) {
		auto logger = (a_type == EType::Core) ? GetCoreLogger() : GetClientLogger();
		logger->error("{0}", a_prefix);

#ifdef SPA_ASSERT_MESSAGE_BOX
		MessageBoxA(nullptr, "No Message", "ScamPA Assert", MB_OK | MB_ICONERROR);
#endif
	}
}
