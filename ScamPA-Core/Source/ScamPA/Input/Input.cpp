#include "spapch.h"
#include "Input.h"
#include "ScamPA/Core/Application.h"

#include <GLFW/glfw3.h>

namespace SPA {
	bool CInput::IsKeyDown(EKeyCode_t a_keycode) {
		GLFWwindow* window_handle = CApplication::GetApplicationInstance().GetWindowHandle().GetNativeWindow();
		int state = glfwGetKey(window_handle, (int)a_keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool CInput::IsMouseButtonDown(EMouseButton_t a_mouse_button) {
		GLFWwindow* window_handle = CApplication::GetApplicationInstance().GetWindowHandle().GetNativeWindow();
		int state = glfwGetMouseButton(window_handle, (int)a_mouse_button);
		return state == GLFW_PRESS;
	}

	glm::vec2 CInput::GetMousePosition() {
		GLFWwindow* window_handle = CApplication::GetApplicationInstance().GetWindowHandle().GetNativeWindow();

		double x, y;
		glfwGetCursorPos(window_handle, &x, &y);
		return { (float)x, (float)y };
	}

	void CInput::SetCursorMode(ECursorMode mode) {
		GLFWwindow* window_handle = CApplication::GetApplicationInstance().GetWindowHandle().GetNativeWindow();
		glfwSetInputMode(window_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
	}

}