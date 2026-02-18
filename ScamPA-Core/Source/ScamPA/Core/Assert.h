#pragma once
#include <filesystem>

// Debug macros
#ifdef SPA_DEBUG
	#if defined(SPA_PLATFORM_WINDOWS)
		#define SPA_DEBUGBREAK() __debugbreak()
	#elif defined(SPA_PLATFORM_LINUX)
		#include <signal.h>
		#define SPA_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "SPA doesn't support debugbreak on this platform yet!"
#endif

#define SPA_ENABLE_ASSERTS

#else
	#define SPA_DEBUGBREAK()
#endif

#ifndef SPA_DIST
	#define SPA_ENABLE_VERIFY
#endif

// Assertion macros (might change later)
#define SPA_EXPAND_MACRO(x) x
#define SPA_STRINGIFY_MACRO(x) #x


#ifdef SPA_ENABLE_ASSERTS
	// TODO: Might need to change where the formatting is structured
	#define SPA_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)){ SPA##type##ERROR(msg, __VA_ARGS__); SPA_DEBUGBREAK(); } }
	#define SPA_INTERNAL_ASSERT_WITH_MSG(type, check, ...) SPA_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define SPA_INTERNAL_ASSERT_NO_MSG(type, check) SPA_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", SPA_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define SPA_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define SPA_INTERNAL_ASSERT_GET_MACRO(...) SPA_EXPAND_MACRO( SPA_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, SPA_INTERNAL_ASSERT_WITH_MSG, SPA_INTERNAL_ASSERT_NO_MSG) )

	// Accepts the condition w/ 1 additional optional parameter (the message) 
	#define SPA_CLIENT_ASSERT(...) SPA_EXPAND_MACRO( SPA_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define SPA_CORE_ASSERT(...) SPA_EXPAND_MACRO( SPA_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define SPA_CLIENT_ASSERT(...)
	#define SPA_CORE_ASSERT(...)
#endif

#ifdef SPA_ENABLE_VERIFY
	// Alteratively, could use the same "default" message for both 'WITH_MSG' and 'NO_MSG' and
	// provide support for custom formatting by concatenating the formatting string instead of having 
	// the format inside the default message

	#define SPA_INTERNAL_VERIFY_IMPL(type, check, msg, ...) { if(!check) { SPA##type##ERROR(msg, __VA_ARGS__); SPA_DEBUGBREAK(); } }
	#define SPA_INTERNAL_VERIFY_WITH_MSG(type, check, ...) SPA_INTERNAL_VERIFY_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define SPA_INTERNAL_VERIFY_NO_MSG(type, check) SPA_INTERNAL_VERIFY_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", SPA_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define SPA_INTERNAL_VERIFY_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define SPA_INTERNAL_VERIFY_GET_MACRO(...) SPA_EXPAND_MACRO( SPA_INTERNAL_VERIFY_GET_MACRO_NAME(__VA_ARGS__, SPA_INTERNAL_VERIFY_WITH_MSG, SPA_INTERNAL_VERIFY_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define SPA_CLIENT_VERIFY(...) SPA_EXPAND_MACRO( SPA_INTERNAL_VERIFY_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define SPA_CORE_VERIFY(...) SPA_EXPAND_MACRO( SPA_INTERNAL_VERIFY_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define SPA_CLIENT_VERIFY(...)
	#define SPA_CORE_VERIFY(...)
#endif