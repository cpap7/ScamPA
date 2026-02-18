#include "spapch.h"
#include "ImGuiLayer.h"
#include "ScamPA/Core/Application.h"


#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui_internal.h>

// Embedded font
#include "Roboto-Regular.embed"


namespace SPA {
	CImGuiLayer::CImGuiLayer() 
		: ILayer() {

	}

	CImGuiLayer::~CImGuiLayer() {

	}

	void CImGuiLayer::OnAttach() {
		CApplication& application = CApplication::GetApplicationInstance();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		// Enable keyboard controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Setup style
		ImGui::StyleColorsDark();

		// When viewports are enabled, we tweak window rounding & window background so platform windows can look identical to regular ones
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SetDarkThemeColors();
		
		// Setup platform/renderer backends
		CVulkanContext& graphics_context = application.GetRenderer().GetGraphicsContext();
		CVulkanSwapchain& swapchain = application.GetRenderer().GetSwapchain();

		GLFWwindow* window = application.GetWindowHandle().GetNativeWindow();
		ImGui_ImplGlfw_InitForVulkan(window, true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance			= graphics_context.GetInstance();
		init_info.PhysicalDevice	= graphics_context.GetPhysicalDevice();
		init_info.Device			= graphics_context.GetDevice();
		init_info.QueueFamily		= graphics_context.GetQueueFamily();
		init_info.Queue				= graphics_context.GetQueue();
		init_info.PipelineCache		= VK_NULL_HANDLE;
		init_info.DescriptorPool	= graphics_context.GetDescriptorPool();
		init_info.Subpass			= 0;
		init_info.MinImageCount		= graphics_context.GetConfig().m_min_image_count;
		init_info.ImageCount		= swapchain.GetWindowData().ImageCount;
		init_info.MSAASamples		= VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator			= graphics_context.GetAllocator();
		init_info.CheckVkResultFn	= Utilities::CheckVkResult;
		ImGui_ImplVulkan_Init(&init_info, swapchain.GetWindowData().RenderPass);
		
		// Load default font
		ImFontConfig font_config;
		font_config.FontDataOwnedByAtlas = false;
		ImFont* roboto_font = io.Fonts->AddFontFromMemoryTTF((void*)g_RobotoRegular, sizeof(g_RobotoRegular), 20.0f, &font_config);
		io.FontDefault = roboto_font;

		{ // Upload Fonts
			// Use any command queue
			VkCommandBuffer command_buffer = application.GetRenderer().GetCommandBuffer(true);
			ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
			graphics_context.FlushCommandBuffer(command_buffer);

			VkResult error_result = vkDeviceWaitIdle(graphics_context.GetDevice());
			if (error_result != VK_SUCCESS) {
				SPA_CORE_ERROR("(ImGui Layer) Failed to wait for device idle during font upload!");
			}
			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
	}

	void CImGuiLayer::OnDetach() {
		// LIFO shutdown
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	
	void CImGuiLayer::OnUpdate(float a_timestep) {
		// Per frame updates are not needed here for the ImGui layer
		// Begin/End will handle frame lifecycle
	}
	
	void CImGuiLayer::OnUIRender() {
		// Other derived layer classes would render their UI in this method
		// Not needed here
	}
	
	void CImGuiLayer::OnEvent(IEvent& a_event) {
		if (m_block_events) {
			ImGuiIO& io = ImGui::GetIO();

			// Block mouse events if ImGui wants to capture the mouse
			a_event.m_is_handled |= a_event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;

			// Block keyboard events if ImGui wants to capture the keyboard
			a_event.m_is_handled |= a_event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void CImGuiLayer::BeginFrame() {
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void CImGuiLayer::EndFrame() {
		// Setup display size
		ImGuiIO& io = ImGui::GetIO();
		CApplication& application = CApplication::GetApplicationInstance();
		io.DisplaySize = ImVec2(
			(float)application.GetWindowHandle().GetWidth(),
			(float)application.GetWindowHandle().GetHeight()
		);

		// Rendering
		ImGui::Render();

		// Note: Vulkan rendering is handled by CRenderer::EndFrame()
		// Which calls ImGui_Vulkan_RenderDrawData

		// Update & render additional platform windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void CImGuiLayer::RenderDockspace() {
		// Setup dockspace
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

		if (m_menubar_callback) {
			window_flags |= ImGuiWindowFlags_MenuBar;
		}

		// Make dockspace cover entire viewport
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		// Style the window to be invisible
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Begin dockspace window
		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar(3);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("ScamPADockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		// Render menu bar
		if (m_menubar_callback) {
			if (ImGui::BeginMenuBar()) {
				m_menubar_callback();
				ImGui::EndMenuBar();
			}
		}

		ImGui::End();
	}

	void CImGuiLayer::SetDarkThemeColors() {
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4(&colors)[ImGuiCol_COUNT] = style.Colors;

		// Base dark background
		colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.085f, 0.09f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.075f, 0.98f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.085f, 0.085f, 0.095f, 1.00f);

		// Neutral/dark surface tiers
		const ImVec4 surface_low = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
		const ImVec4 surface_mid = ImVec4(0.16f, 0.16f, 0.19f, 1.00f);
		const ImVec4 surface_high = ImVec4(0.20f, 0.20f, 0.23f, 1.00f);

		// Deep royal purple color family (#4B006E base) for panel/window borders and separators
		const ImVec4 accent = ImVec4(0.294f, 0.000f, 0.431f, 1.00f);		// #4B006E
		const ImVec4 accent_hover = ImVec4(0.365f, 0.000f, 0.537f, 1.00f);	// #5D0089
		const ImVec4 accent_active = ImVec4(0.463f, 0.000f, 0.710f, 1.00f);	// #7600B5

		// Headers
		colors[ImGuiCol_Header] = surface_mid;
		colors[ImGuiCol_HeaderHovered] = surface_high;
		colors[ImGuiCol_HeaderActive] = surface_high;

		// Tabs
		colors[ImGuiCol_Tab] = surface_low;
		colors[ImGuiCol_TabHovered] = surface_high;
		colors[ImGuiCol_TabActive] = surface_mid;
		colors[ImGuiCol_TabUnfocused] = surface_low;
		colors[ImGuiCol_TabUnfocusedActive] = surface_mid;

		// Title bars 
		colors[ImGuiCol_TitleBg] = surface_low;
		colors[ImGuiCol_TitleBgActive] = surface_low;
		colors[ImGuiCol_TitleBgCollapsed] = surface_low;

		// Frames (inputs, sliders)
		colors[ImGuiCol_FrameBg] = surface_mid;
		colors[ImGuiCol_FrameBgHovered] = surface_high;
		colors[ImGuiCol_FrameBgActive] = surface_high;

		// Buttons
		colors[ImGuiCol_Button] = surface_mid;
		colors[ImGuiCol_ButtonHovered] = surface_high;
		colors[ImGuiCol_ButtonActive] = surface_high;

		// Interactive accents
		colors[ImGuiCol_CheckMark] = ImVec4(0.85f, 0.85f, 0.90f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.65f, 0.65f, 0.70f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.75f, 0.75f, 0.80f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.45f, 0.45f, 0.50f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.60f, 0.60f, 0.65f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.70f, 0.70f, 0.75f, 1.00f);

		// Scrollbar
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.06f, 0.065f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = surface_low;
		colors[ImGuiCol_ScrollbarGrabHovered] = surface_mid;
		colors[ImGuiCol_ScrollbarGrabActive] = surface_high;

		// Separators
		colors[ImGuiCol_Separator] = ImVec4(0.24f, 0.24f, 0.27f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = accent_hover; // = ImVec4(0.32f, 0.32f, 0.36f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = accent_active; //= ImVec4(0.36f, 0.36f, 0.40f, 1.00f);

		// Borders
		colors[ImGuiCol_Border] = accent; // Purple
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

		// Text
		colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.95f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.55f, 1.00f);

		// Drag & Drop 
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.80f, 0.80f, 0.90f, 1.00f);

		// Plot
		colors[ImGuiCol_PlotLines] = ImVec4(0.60f, 0.60f, 0.65f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.75f, 0.75f, 0.80f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.60f, 0.60f, 0.65f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.75f, 0.75f, 0.80f, 1.00f);

		// Rounding and spacing
		style.WindowRounding = 6.0f;
		style.FrameRounding = 5.0f;
		style.GrabRounding = 4.0f;
		style.PopupRounding = 6.0f;
		style.TabRounding = 6.0f;

		// Borders on panels/windows 
		style.WindowBorderSize = 1.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupBorderSize = 1.0f;
		style.FrameBorderSize = 0.0f;
		style.TabBorderSize = 0.0f;

		style.ItemSpacing = ImVec2(8.0f, 6.0f);
		style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
		style.WindowPadding = ImVec2(10.0f, 10.0f);
		style.FramePadding = ImVec2(8.0f, 6.0f);
		style.ScrollbarSize = 14.0f;
	}

	uint32_t CImGuiLayer::GetActiveWidgetID() const {
		return GImGui->ActiveId;
	}
}