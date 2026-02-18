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
		std::string m_title = "ScamPA Application";
		
		uint32_t m_width = 1280;
		uint32_t m_height = 720;
	};

	class CWindow {
	public:
		using EventCallbackFunction = std::function<void(IEvent&)>;
		//using ResizeCallbackFunction = std::function<void(uint32_t, uint32_t)>;

	private:
		struct SWindowData {
			EventCallbackFunction m_event_callback_function;
			//ResizeCallbackFunction m_resize_callback_function;

			std::string m_title;
			
			uint32_t m_width;
			uint32_t m_height;

			//bool m_vsync_status;
		};
		SWindowData m_window_data;
		//SWindowSpecification m_specification;

		GLFWwindow* m_window_handle = nullptr;
		GLFWcursor* m_imgui_mouse_cursors[9] = { 0 };

		//std::shared_ptr<CVulkanContext> m_graphics_context;
		//CVulkanSwapchain* m_swapchain;

	public:
		CWindow(const SWindowSpecification& a_specification);
		~CWindow();

		void OnUpdate();

	private:
		void Init();
		void SetupCallbacks(); // Part of Init()

	public:
		void Shutdown();

		// GLFW wrappers
		void Maximize();
		const char** GetRequiredInstanceExtensions(uint32_t& a_extensions_count);
		VkSurfaceKHR CreateSurface(VkInstance a_instance);
		void ToggleVSync(bool a_toggle_flag);
		bool ShouldClose() const;
		void GetFramebufferSize(int* a_width, int* a_height) const;
		void SetTitle(const std::string& a_title);
		std::pair<float, float> GetWindowPosition() const;

		// Getters
		//inline bool IsVSync() const							{ return m_window_data.m_vsync_status;						}
		inline std::pair<uint32_t, uint32_t> GetSize() const	{ return { m_window_data.m_width, m_window_data.m_height }; }
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		//inline uint32_t GetWidth() const						{ return m_window_data.m_width;								}
		//inline uint32_t GetHeight() const						{ return m_window_data.m_height;							}
		inline const std::string& GetTitle() const				{ return m_window_data.m_title;								}
		inline GLFWwindow* GetNativeWindow() const				{ return m_window_handle;									}
		
		// Setters
		inline void SetEventCallback(const EventCallbackFunction a_function)	{ m_window_data.m_event_callback_function = a_function; }
		//inline void SetResizeCallback(const ResizeCallbackFunction a_function)	{ m_window_data.m_resize_callback_function = a_function; }

	};

}
