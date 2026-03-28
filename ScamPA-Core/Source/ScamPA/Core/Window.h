#pragma once

#include "ScamPA/Events/Event.h"
//#include "ScamPA/Renderer/VulkanSwapchain.h"

#include <string>
#include <functional>

// Forward declarations
struct GLFWwindow;
struct GLFWcursor;

typedef struct VkSurfaceKHR_T* VkSurfaceKHR;
typedef struct VkInstance_T* VkInstance;

namespace SPA {
	struct SWindowSpecification {
		std::filesystem::path m_icon_path;
		std::string m_title = "ScamPA Application";
		
		uint32_t m_width = 1280;
		uint32_t m_height = 720;

		bool m_use_custom_titlebar	= false; // Use custom title bar instead of OS default
		bool m_window_resizeable	= true;
		bool m_center_window		= false; // Generate window in the center of primary window
		// TODO: Flag for undecorated windows possibly
	};

	class CWindow {
	public:
		using EventCallbackFn = std::function<void(IEvent&)>;
		using TitlebarHitTestFn = std::function<bool()>;
		//using ResizeCallbackFn = std::function<void(uint32_t, uint32_t)>;

	private:
		struct SWindowData {
			EventCallbackFn m_event_callback_function;
			TitlebarHitTestFn m_titlebar_hit_test_function;
			//ResizeCallbackFn m_resize_callback_function;
			std::filesystem::path m_icon_path;
			std::string m_title;
			
			uint32_t m_width;
			uint32_t m_height;

			bool m_has_custom_titlebar;
			bool m_window_resizeable;
			bool m_center_window;
		};

		SWindowData m_window_data;

		GLFWwindow* m_window_handle = nullptr;
		GLFWcursor* m_imgui_mouse_cursors[9] = { 0 };

	public:
		CWindow(const SWindowSpecification& a_specification);
		~CWindow();

		void OnUpdate();

	private:
		void Init();
		void SetupCallbacks(); // Part of Init()

	public:
		void Shutdown();
		void ShowWindow();

		// GLFW wrappers
		void Maximize();
		void Minimize();
		void Restore();
		bool IsMaximized() const;

		// Vulkan
		const char** GetRequiredInstanceExtensions(uint32_t& a_extensions_count);
		VkSurfaceKHR CreateSurface(VkInstance a_instance);
		void ToggleVSync(bool a_toggle_flag);
		bool ShouldClose() const;
		void GetFramebufferSize(int* a_width, int* a_height) const;
		
		void SetTitle(const std::string& a_title);
		std::pair<float, float> GetWindowPosition() const;

		// Getters
		inline std::pair<uint32_t, uint32_t> GetSize() const	{ return { m_window_data.m_width, m_window_data.m_height }; }
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		
		inline const std::string& GetTitle() const				{ return m_window_data.m_title;								}
		inline GLFWwindow* GetNativeWindow() const				{ return m_window_handle;									}
		inline bool HasCustomTitlebar() const					{ return m_window_data.m_has_custom_titlebar;				}

		// Setters
		inline void SetEventCallback(const EventCallbackFn a_function)	{ m_window_data.m_event_callback_function = a_function; }
		inline void SetTitlebarHitTestCallback(const TitlebarHitTestFn a_function) { m_window_data.m_titlebar_hit_test_function = a_function; }

		//inline void SetResizeCallback(const ResizeCallbackFunction a_function)	{ m_window_data.m_resize_callback_function = a_function; }

	};

}
