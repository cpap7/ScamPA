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
		Left = Button0,
		Right = Button1,
		Middle = Button2
	} MouseButton;

	inline std::ostream& operator<<(std::ostream& a_os, EMouseButton_t a_mouse_button) {
		a_os << static_cast<int32_t>(a_mouse_button);
		return a_os;
	}

}