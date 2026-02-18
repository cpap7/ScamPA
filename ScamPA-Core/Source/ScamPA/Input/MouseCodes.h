#pragma once

#include <stdint.h>
#include <iostream>


namespace SPA {
	enum class ECursorMode {
		Normal = 0,
		Hidden = 1,
		Locked = 2
	};

	typedef enum class EMouseButton_t : uint16_t {
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Left	= Button0,
		Right	= Button1,
		Middle	= Button2
	} MouseButton;

	inline std::ostream& operator<<(std::ostream& a_os, EMouseButton_t a_mouse_button) {
		a_os << static_cast<int32_t>(a_mouse_button);
		return a_os;
	}
}

#define SPA_MOUSE_BUTTON_0      ::SPA::EMouseButton_t::Button0
#define SPA_MOUSE_BUTTON_1      ::SPA::EMouseButton_t::Button1
#define SPA_MOUSE_BUTTON_2      ::SPA::EMouseButton_t::Button2
#define SPA_MOUSE_BUTTON_3      ::SPA::EMouseButton_t::Button3
#define SPA_MOUSE_BUTTON_4      ::SPA::EMouseButton_t::Button4
#define SPA_MOUSE_BUTTON_5      ::SPA::EMouseButton_t::Button5
#define SPA_MOUSE_BUTTON_6      ::SPA::EMouseButton_t::Button6
#define SPA_MOUSE_BUTTON_LEFT   ::SPA::EMouseButton_t::Left
#define SPA_MOUSE_BUTTON_RIGHT  ::SPA::EMouseButton_t::Right
#define SPA_MOUSE_BUTTON_MIDDLE ::SPA::EMouseButton_t::Middle

namespace SPA {
	// Platform conversion dictionaries go here
	inline std::unordered_map<EMouseButton_t, int> windows_mouse_button_map = {
		{ SPA_MOUSE_BUTTON_LEFT,	0x01 },
		{ SPA_MOUSE_BUTTON_RIGHT,	0x02 },
		{ SPA_MOUSE_BUTTON_MIDDLE,	0x04 },
		{ SPA_MOUSE_BUTTON_5,		0x05 },	// X1 mouse button
		{ SPA_MOUSE_BUTTON_6,		0x06 } 	// X2 mouse button
	};
}