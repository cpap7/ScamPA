#pragma once

#include "KeyCodes.h"
#include "MouseCodes.h"

#include <glm/glm.hpp>

namespace SPA {
	class CInput {
	public:
		static bool IsKeyDown(EKeyCode_t a_key_code);
		static bool IsMouseButtonDown(EMouseButton_t a_mouse_button_code);
		static glm::vec2 GetMousePosition();
		static void SetCursorMode(ECursorMode a_cursor_mode);
	};

}
