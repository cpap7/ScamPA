#include "spapch.h"
#include "Window.h"

#include "ScamPA/Events/ApplicationEvent.h"
#include "ScamPA/Events/KeyEvent.h"
#include "ScamPA/Events/MouseEvent.h"


#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <imgui.h>

namespace SPA {
	static void GLFWErrorCallback(int a_error, const char* a_description) {
		SPA_CORE_ERROR("(GLFW) Error {1}: {0}", a_error, a_description);
	}

	static bool s_glfw_initialized = false;

	CWindow::CWindow(const SWindowSpecification& a_specification) {
		m_window_data.m_title = a_specification.m_title;
		m_window_data.m_width = a_specification.m_width;
		m_window_data.m_height = a_specification.m_height;

		Init();
	}

	CWindow::~CWindow() {
		Shutdown();
		//glfwTerminate();
	}

	void CWindow::OnUpdate() {
		glfwPollEvents();

		// OpenGL-specific
		//glfwSwapBuffers(m_window_handle);

		// Update ImGui cursors
		ImGuiMouseCursor current_imgui_cursor = ImGui::GetMouseCursor();
		glfwSetCursor(
			m_window_handle,
			m_imgui_mouse_cursors[current_imgui_cursor] ? m_imgui_mouse_cursors[current_imgui_cursor] : m_imgui_mouse_cursors[ImGuiMouseCursor_Arrow]
		);
	}

	void CWindow::Init() {
		

		if (!s_glfw_initialized) {
			glfwSetErrorCallback(GLFWErrorCallback);
			
			if (!glfwInit()) {
				SPA_CORE_ERROR("(GLFW) Error: Could not initialize GLFW!");
				return;
			}

			s_glfw_initialized = true;
		}
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // This would differ between graphics APIs (i.e., OpenGL), but for Vulkan, this is sufficient

#ifdef SPA_PLATFORM_WINDOWS
		// TODO: Different platforms might need different window hints (i.e., Mac OSX & Linux)
		// I.e., to remove window decoration
#endif
		m_window_handle = glfwCreateWindow(
			static_cast<int>(m_window_data.m_width),
			static_cast<int>(m_window_data.m_height),
			m_window_data.m_title.c_str(),
			nullptr,
			nullptr
		);

		if (!m_window_handle) {
			SPA_CORE_ERROR("(GLFW) Failed to create GLFW window!");
			return;
		}

		if (!glfwVulkanSupported()) {
			SPA_CORE_ERROR("(GLFW) Vulkan not supported!");
			return;
		}

		// Store window data pointer for callbacks, then set up callbacks
		glfwSetWindowUserPointer(m_window_handle, &m_window_data);
		SetupCallbacks();

		m_imgui_mouse_cursors[ImGuiMouseCursor_Arrow]		= glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		m_imgui_mouse_cursors[ImGuiMouseCursor_TextInput]	= glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
		m_imgui_mouse_cursors[ImGuiMouseCursor_ResizeAll]	= glfwCreateStandardCursor(GLFW_ARROW_CURSOR);	// FIXME: GLFW doesn't have this
		m_imgui_mouse_cursors[ImGuiMouseCursor_ResizeNS]	= glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
		m_imgui_mouse_cursors[ImGuiMouseCursor_ResizeEW]	= glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
		m_imgui_mouse_cursors[ImGuiMouseCursor_ResizeNESW]	= glfwCreateStandardCursor(GLFW_ARROW_CURSOR);	// FIXME: GLFW doesn't have this 
		m_imgui_mouse_cursors[ImGuiMouseCursor_ResizeNWSE]	= glfwCreateStandardCursor(GLFW_ARROW_CURSOR);	// FIXME: GLFW doesn't have this
		m_imgui_mouse_cursors[ImGuiMouseCursor_Hand]		= glfwCreateStandardCursor(GLFW_HAND_CURSOR);
	}


	void CWindow::SetupCallbacks() {
		// Window resize callback
		glfwSetWindowSizeCallback(m_window_handle, [](GLFWwindow* a_window, int a_width, int a_height) {
			SWindowData& data = *(SWindowData*)glfwGetWindowUserPointer(a_window);
			data.m_width = a_width;
			data.m_height = a_height;

			// Initialize & dispatch event for layers, ImGui, etc.
			CWindowResizedEvent event(a_width, a_height);
			data.m_event_callback_function(event);
		});

		// Window close callback
		glfwSetWindowCloseCallback(m_window_handle, [](GLFWwindow* a_window) {
			SWindowData& data = *(SWindowData*)glfwGetWindowUserPointer(a_window);

			// Initialize & dispatch event
			CWindowClosedEvent event;
			data.m_event_callback_function(event);
		});

		// Window focus callback
		glfwSetWindowFocusCallback(m_window_handle, [](GLFWwindow* a_window, int a_focused) {
			SWindowData& data = *(SWindowData*)glfwGetWindowUserPointer(a_window);

			// TODO: Look into GLFW_FOCUSED
			if (a_focused) {
				CWindowFocusEvent event; 
				data.m_event_callback_function(event);
			}
			else {
				CWindowLostFocusEvent event; 
				data.m_event_callback_function(event);
			}
		
		});

		// Key callback
		glfwSetKeyCallback(m_window_handle, [](GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods) {
			SWindowData& data = *(SWindowData*)glfwGetWindowUserPointer(a_window);

			SPA::EKeyCode_t key = static_cast<EKeyCode_t>(a_key);

			// Handle different key actions
			switch (a_action) {
				case GLFW_PRESS:	{ CKeyPressedEvent event(key, false);	data.m_event_callback_function(event); break; }
				case GLFW_RELEASE:	{ CKeyReleasedEvent event(key);			data.m_event_callback_function(event); break; }
				case GLFW_REPEAT:	{ CKeyPressedEvent event(key, true);	data.m_event_callback_function(event); break; }
			}
		});

		// Character callback (for text input)
		glfwSetCharCallback(m_window_handle, [](GLFWwindow* a_window, unsigned int a_character) {
			SWindowData& data = *(SWindowData*)glfwGetWindowUserPointer(a_window);
		
			SPA::EKeyCode_t key = static_cast<EKeyCode_t>(a_character);

			// Handle event
			CKeyTypedEvent event(key);
			data.m_event_callback_function(event);
		});

		// Mouse button callback
		glfwSetMouseButtonCallback(m_window_handle, [](GLFWwindow* a_window, int a_button, int a_action, int a_mods) {
			SWindowData& data = *(SWindowData*)glfwGetWindowUserPointer(a_window);
			SPA::EMouseButton_t mouse_button = static_cast<EMouseButton_t>(a_button);

			// Handle different mouse actions
			switch (a_action) {
				case GLFW_PRESS:	{ CMouseButtonPressedEvent event(mouse_button); data.m_event_callback_function(event); break; }
				case GLFW_RELEASE:	{ CMouseButtonReleasedEvent event(mouse_button); data.m_event_callback_function(event); break; }
			}
		});

		// Mouse scroll callback
		glfwSetScrollCallback(m_window_handle, [](GLFWwindow* a_window, double a_x_offset, double a_y_offset) {
			SWindowData& data = *(SWindowData*)glfwGetWindowUserPointer(a_window);

			CMouseScrolledEvent event((float)a_x_offset, (float)a_y_offset);
			data.m_event_callback_function(event);
		});

		// Mouse position callback
		glfwSetCursorPosCallback(m_window_handle, [](GLFWwindow* a_window, double a_x_position, double a_y_position) {
			SWindowData& data = *(SWindowData*)glfwGetWindowUserPointer(a_window);


			CMouseMovedEvent event((float)a_x_position, (float)a_y_position);
			data.m_event_callback_function(event);
		});

		// File drop callback
		glfwSetDropCallback(m_window_handle, [](GLFWwindow* a_window, int a_path_count, const char* a_paths[]) {
			SWindowData& data = *(SWindowData*)glfwGetWindowUserPointer(a_window);

			std::vector<std::filesystem::path> file_paths(a_path_count);
			for (size_t i{}; i < a_path_count; i++) {
				file_paths[i] = a_paths[i];
			}

			CWindowDropEvent event(std::move(file_paths));
			data.m_event_callback_function(event);
		});
	}

	void CWindow::Shutdown() {
		if (m_window_handle) {
			glfwDestroyWindow(m_window_handle);
			m_window_handle = nullptr;
		}
		// Note: we don't call glfwTerminate here b/c there might be other windows active
	}

	void CWindow::Maximize() {
		glfwMaximizeWindow(m_window_handle);
	}

	const char** CWindow::GetRequiredInstanceExtensions(uint32_t& a_extensions_count) {
		return glfwGetRequiredInstanceExtensions(&a_extensions_count);
	}

	VkSurfaceKHR CWindow::CreateSurface(VkInstance a_instance) {
		VkSurfaceKHR surface;
		VkResult error_result = glfwCreateWindowSurface(a_instance, m_window_handle, nullptr, &surface);
		
		if (error_result != VK_SUCCESS) {
			SPA_CORE_ERROR("(GLFW) Failed to create Vulkan surface!");
			return VK_NULL_HANDLE;
		}

		return surface;
	}

	void CWindow::ToggleVSync(bool a_toggle_flag) {
		// TODO: Swapchain recreation via Vulkan
	}

	bool CWindow::ShouldClose() const {
		return glfwWindowShouldClose(m_window_handle);
	}

	void CWindow::GetFramebufferSize(int* a_width, int* a_height) const {
		glfwGetFramebufferSize(m_window_handle, a_width, a_height);
	}

	void CWindow::SetTitle(const std::string& a_title) {
		m_window_data.m_title = a_title;
		glfwSetWindowTitle(m_window_handle, m_window_data.m_title.c_str());
	}

	std::pair<float, float> CWindow::GetWindowPosition() const {
		int width; 
		int height;

		glfwGetWindowPos(m_window_handle, &width, &height);
		return { static_cast<float>(width), static_cast<float>(height) };
	}
	

	uint32_t CWindow::GetWidth() const {
		int width, height;
		GetFramebufferSize(&width, &height);
	
		return static_cast<uint32_t>(width);
	}
	
	uint32_t CWindow::GetHeight() const {
		int width, height;
		GetFramebufferSize(&width, &height);

		return static_cast<uint32_t>(height);
	}
}