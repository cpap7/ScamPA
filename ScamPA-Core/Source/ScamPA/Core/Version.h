#pragma once

#define SPA_BUILD_VERSION "v1.0.0a"

// Build platform
#if defined (SPA_PLATFORM_WINDOWS)
	#define SPA_BUILD_PLATFORM_NAME "Windows x64"
#elif defined (SPA_PLATFORM_LINUX)
	#define SPA_BUILD_PLATFORM_NAME "Linux"
#elif defined (SPA_PLATFORM_MACOS)
	#define SPA_BUILD_PLATFORM_NAME "macOS"
#else
	#define SPA_BUILD_PLATFORM_NAME "Unknown"
#endif

// Build configuration
#if defined(SPA_DEBUG)
	#define SPA_BUILD_CONFIG_NAME "Debug Build"
#elif defined(SPA_RELEASE)
	#define SPA_BUILD_CONFIG_NAME "Release Build"
#elif defined(SPA_DIST)
	#define SPA_BUILD_CONFIG_NAME "Dist Build"
#endif

#define SPA_VERSION_LONG "ScamPA " SPA_BUILD_VERSION " (" SPA_BUILD_PLATFORM_NAME " - " SPA_BUILD_CONFIG_NAME ")" 