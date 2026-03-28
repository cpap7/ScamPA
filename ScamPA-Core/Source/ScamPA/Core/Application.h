#pragma once

#include "Base.h"
#include "LayerStack.h"
#include "Window.h"
#include "FileDialogs.h"

#include "ScamPA/ImGui/ImGuiLayer.h"
#include "ScamPA/Events/ApplicationEvent.h"
//#include "ScamPA/Renderer/VulkanImage.h"
#include "ScamPA/Renderer/Renderer.h"

#include <string>
#include <memory>
#include <functional>
#include <filesystem>

namespace SPA {
	/* Note: For application construction + lifecycle, see ClientEntryPoint.h
		A function like:
		CApplication* CreateApplication();
		would be defined by the client ONLY!
	*/

	struct SApplicationCommandLineArgs {
		int m_count = 0;
		char** m_args = nullptr;

		const char* operator[](int a_index) const {
			SPA_CORE_ASSERT(a_index < m_count);
			return m_args[a_index];
		}
	};

	struct SApplicationSpecification {
		//SApplicationCommandLineArgs m_args; // TODO: Add this later once files can be used in a meaningful way

		std::string m_name = "ScamPA App";
		std::filesystem::path m_icon_path;

		uint32_t m_width = 1600;
		uint32_t m_height = 900;

		bool m_use_custom_titlebar = false; // Use custom title bar instead of OS default
		bool m_window_resizeable = true;
		bool m_center_window = false; // Generate window in the center of primary window
	};

	class CApplication {
	private:
		// Resolution, title, args
		SApplicationSpecification m_specification;

		CLayerStack m_layer_stack; // Layer management
		
		// Shared resources
		std::shared_ptr<CImGuiLayer> m_imgui_layer;

		// Owned resources
		std::unique_ptr<CWindow> m_window_handle;
		std::unique_ptr<IFileDialogs> m_file_dialogs;

		std::unique_ptr<CRenderer> m_renderer;
		// TODO: abstract these out, leave only renderer
		std::unique_ptr<CVulkanContext> m_graphics_context;
		std::unique_ptr<CVulkanSwapchain> m_swapchain;
		
		// Singleton
		static CApplication* s_instance;

		// Frame time
		float m_timestep = 0.0f;
		float m_frame_time = 0.0f;
		float m_last_frame_time = 0.0f;

		// Flags
		bool m_is_running = false;
		bool m_is_minimized = false;

	public:
		CApplication(const SApplicationSpecification& a_application_specification = SApplicationSpecification());
		~CApplication();

		void Run();			// Main loop
		void Close();		// Sets global bool, which controls application lifetime, to false 
		
		std::string OpenFile(const char* a_filter = "All.\0*.*\0\0");
		std::string SaveFile(const char* a_filter = "All.\0*.*\0\0", const char* a_default_extension = "");

		// Getters
		float GetTime();
		inline CWindow& GetWindowHandle() const							 { return *m_window_handle;						}
		inline CRenderer& GetRenderer() const							 { return *m_renderer;							}
		inline const SApplicationSpecification& GetSpecification() const { return m_specification;						}
		//inline bool IsTitlebarHovered() const							 { return m_imgui_layer->IsTitlebarHovered();	}
		//inline std::shared_ptr<CVulkanImage> GetApplicationIcon() const { return m_app_header_icon; }

		static CApplication& GetApplicationInstance(); // For singleton
		static const char* GetConfigurationName();
		static const char* GetPlatformName();
		static const char* GetApplicationVersion();				

		// Setters
		void SetMenubarCallback(const std::function<void()>& a_menubar_callback);
		
	private:
		// Helpers
		void RenderImGui();
		void RenderVulkan();
		void Init();
		void Shutdown();
		

		// Event handling helpers
		void OnEvent(IEvent& a_event);
		bool OnWindowResize(CWindowResizedEvent& a_event);
		bool OnWindowClose(CWindowClosedEvent& a_event);
		bool OnWindowDrop(CWindowDropEvent& a_event);
	
	public:
		template<typename T>
		inline void PushLayer() {
			m_layer_stack.PushLayer<T>();
		}
	};
}