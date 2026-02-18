#pragma once

// Platform detection via macros
#ifdef _WIN32 // Windows x86
	#ifdef _WIN64 // Windows x86-x64
		#define SPA_PLATFORM_WINDOWS
		#include <Windows.h>
		#include <commdlg.h>
	#else
		/* Windows x86 */
		#error "ScamPA is not supported on Windows x86 (32-bit) builds!"
	#endif

#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	/* TARGET_OS_MAC exists on all platforms,
	* so all different Apple platforms must be
	* checked to ensure it's running on MACOS, not
	* IOS or some other Apple platform
	*/
	#if TARGET_IPHONE_SIMULATOR == 1
		#define SPA_PLATFORM_IOS_SIMULATOR
		#error "ScamPA is not supported on iOS simulators!"
	#elif TARGET_OS_IPHONE == 1
		#define SPA_PLATFORM_IOS
		#error "ScamPA is not supported on iOS!"
	#elif TARGET_OS_MAC == 1
		#define SPA_PLATFORM_MACOS
		#error "ScamPA is not supported on MacOS!"
	#else
		#error "Unknown Apple platform!"
#endif

// Also have to check __ANDROID__ before __linux__ since
// android is based on the linux kernel and has __linux__ defined
#elif defined(__ANDROID__)
	#define SPA_PLATFORM_ANDROID
	#error "ScamPA is not supported on Android!"
#elif defined(__linux__)
	#define SPA_PLATFORM_LINUX
	#error "ScamPA is not supported on Linux!"
#endif 
