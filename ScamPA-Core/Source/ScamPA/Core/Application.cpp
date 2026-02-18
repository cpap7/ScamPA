#include "spapch.h"
#include "Application.h"
//#include "ScamPA/ImGui/ImGuiLayer.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <imgui.h>

extern bool g_application_running; // See ClientEntryPoint.h

namespace SPA {
	CApplication* CApplication::s_instance = nullptr;

	CApplication::CApplication(const SApplicationSpecification& a_specification)
		: m_specification(a_specification) {

		SPA_CORE_ASSERT(!s_instance, "(Application) App instance already exists!");

		s_instance = this;
		g_application_running = true;

		Init();
	}

	CApplication::~CApplication() {
		Shutdown();

		g_application_running = false;
		s_instance = nullptr;
	}

	void CApplication::Run() {
		m_is_running = true;
		
		// Main loop
		while (!m_window_handle->ShouldClose() && m_is_running) {
			// Update window
			m_window_handle->OnUpdate(); // Poll events (i.e., resize, input, etc.)

			if (m_is_minimized) {
				continue; // Skip frames when minimized
			}

			// Update layer logic (per frame)
			for (auto& layer : m_layer_stack) {
				layer->OnUpdate(m_timestep);
			}

			// Handle resize events (if needed)
			if (m_swapchain->NeedsRebuild()) {
				int width, height;
				m_window_handle->GetFramebufferSize(&width, &height);
				
				if (width > 0 && height > 0) {
					SPA_CORE_INFO("(Application) Rebuilding swapchain ({0} x {1})", width, height);
					m_swapchain->Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
				}
			}
			RenderImGui();
			RenderVulkan();
			
			// Update frame time
			float time = GetTime();
			m_frame_time = time - m_last_frame_time;
			m_timestep = glm::min<float>(m_frame_time, 0.0333f); // Cap at ~30fps for now
			m_last_frame_time = time;
		}
	}

	void CApplication::Close() {
		m_is_running = false;
	}

	CApplication& CApplication::GetApplicationInstance() {
		SPA_CORE_ASSERT(s_instance, "(Application) App instance does not exist!");
		return *s_instance;
	}

	float CApplication::GetTime() {
		return (float)glfwGetTime();
	}

	void CApplication::SetMenubarCallback(const std::function<void()>& a_menubar_callback) {
		m_imgui_layer->SetMenubarCallback(a_menubar_callback);
	}

	void CApplication::RenderImGui() {
		m_imgui_layer->BeginFrame(); 			// ImGui layer must call "BeginFrame" method before any additional ImGui calls are made
		m_imgui_layer->RenderDockspace();

		// Render all layers' UI
		for (auto& layer : m_layer_stack) {
			layer->OnUIRender();
		}

		m_imgui_layer->EndFrame(); 			// End ImGui frame to prepare ImGui draw data

	}

	void CApplication::RenderVulkan() {
		m_renderer->BeginFrame(); 			// Begin Vulkan rendering -- acquires images, begins render pass
		m_renderer->EndFrame(); 			// End frame -- records draw data & presents frame
	}

	void CApplication::Init() {
		SPA_CORE_INFO("Initializing application: {0}", m_specification.m_name);

		// Create the window
		SWindowSpecification window_spec;
		window_spec.m_title = m_specification.m_name;
		window_spec.m_width = m_specification.m_width;
		window_spec.m_height = m_specification.m_height;
		m_window_handle = std::make_unique<CWindow>(window_spec);

		m_window_handle->SetEventCallback(SPA_BIND_EVENT_FN(OnEvent));

		// Setup vulkan context
		CVulkanContext::SConfig vulkan_config;
#ifdef SPA_DEBUG
		vulkan_config.m_enable_validation = true;
#else
		vulkan_config.m_enable_validation = false;
#endif
		vulkan_config.m_min_image_count = 2;

		m_graphics_context = std::make_unique<CVulkanContext>(vulkan_config);

		// Get required Vulkan extensions from GLFW
		uint32_t extensions_count = 0;
		const char** extensions = m_window_handle->GetRequiredInstanceExtensions(extensions_count);
		m_graphics_context->Init(extensions, extensions_count);
		SPA_CORE_INFO("(Application) Vulkan context initialized!");

		// Setup window surface
		VkSurfaceKHR surface = m_window_handle->CreateSurface(m_graphics_context->GetInstance());
		SPA_CORE_INFO("(Application) Vulkan surface created!");

		// Setup swapchain
		int width, height;
		m_window_handle->GetFramebufferSize(&width, &height);
		m_swapchain = std::make_unique<CVulkanSwapchain>(*m_graphics_context, surface, width, height);
		SPA_CORE_INFO("(Application) Vulkan swapchain created! ({0} x {1})", width, height);

		// Setup renderer
		m_renderer = std::make_unique<CRenderer>(*m_graphics_context, *m_swapchain);
		SPA_CORE_INFO("(Application) Renderer initialized!");

		// Push ImGui layer
		m_imgui_layer = std::make_shared<CImGuiLayer>();
		m_layer_stack.PushLayer(m_imgui_layer);
		SPA_CORE_INFO("(Application) ImGui layer initialized!");

		SPA_CORE_INFO("(Application) Initialization complete!");
	}

	void CApplication::Shutdown() {
		SPA_CORE_INFO("(Application) Shutting down...");

		// Wait for device to finish all operations
		if (m_graphics_context && m_graphics_context->GetDevice()) {
			VkResult error_result = vkDeviceWaitIdle(m_graphics_context->GetDevice());
			if (error_result != VK_SUCCESS) {
				SPA_CORE_ERROR("(Application) Failed to wait for device idle during shutdown!");
			}
		}
		// Destroy renderer & swapchain - smart ptrs
		m_renderer.reset();
		m_swapchain.reset();

		// Detach layers
		m_layer_stack.PopAll();
	
		// Additional cleanup
		if (m_graphics_context) {
			m_graphics_context->Shutdown();
		}
		if (m_window_handle) {
			m_window_handle->Shutdown();
		}

		SPA_CORE_INFO("(Application) Shutdown complete!");
	}

	void CApplication::OnEvent(IEvent& a_event) {
		CEventDispatcher dispatcher(a_event);
		
		dispatcher.Dispatch<CWindowResizedEvent>(SPA_BIND_EVENT_FN(OnWindowResize));
		dispatcher.Dispatch<CWindowClosedEvent>(SPA_BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<CWindowDropEvent>(SPA_BIND_EVENT_FN(OnWindowDrop));

		// Pass events to layers in reverse order
		for (auto iterator = m_layer_stack.end(); iterator != m_layer_stack.begin();) {
			(*--iterator)->OnEvent(a_event);
			if (a_event.m_is_handled) {
				break;
			}
		}

	}
	
	bool CApplication::OnWindowResize(CWindowResizedEvent& a_event) {
		int width = a_event.GetWidth();
		int height = a_event.GetHeight();

		if (width == 0 || height == 0) {
			m_is_minimized = true;
			return false;
		}
		m_is_minimized = false;

		if (width > 0 && height > 0) { // Update rebuild flag
			m_swapchain->SetNeedsRebuild(true);
		}

		return false;
	}

	bool CApplication::OnWindowClose(CWindowClosedEvent& a_event) {
		//m_is_running = false;
		Close();
		return true;
	}

	bool CApplication::OnWindowDrop(CWindowDropEvent& a_event) {
		// TODO
		return false;
	}

	
	
}
