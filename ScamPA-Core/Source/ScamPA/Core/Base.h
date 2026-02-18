#pragma once

#include <memory>

#include "PlatformDetection.h"

#include "Assert.h"

namespace SPA {
	void InitializeCore();
	void ShutdownCore();
}


// Miscellaneous macros
#define BIT(x) (1 << x)

// Macro for binding event functions instead of using lambdas
#define SPA_BIND_EVENT_FN(fn) [this](auto&&... args)->decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

// Macro for binding callback functions w/ void return types
//#define SPA_BIND_CALLBACK_FN(fn)[this](auto&&... args) { this->fn(std::forward<decltype(args)>(args)...); }

// Size of a static C-style array. Don't use on pointers!
#define SPA_ARRAYSIZE(_ARR) ((int)(sizeof(_ARR) / sizeof(*(_ARR))))     // Used in various parts of the renderer

namespace SPA {
	// Labels for different types of data structures used within ScamPA
	using byte = uint8_t;

 
	// Using smart pointers for reference counting
	// Unique pointers
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	// Shared pointers	
	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
	

}
