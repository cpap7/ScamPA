#include "spapch.h"
#include "ImGuiLayer.h"
#include "ImGuiCustomUI.h"
#include "ImGuiTheme.h"

#include "ScamPA/Core/Application.h"

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui_internal.h>

// Embedded images
#include "ScamPALogo.embed"
#include "WindowImages.embed"

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
		m_use_custom_titlebar = application.GetWindowHandle().HasCustomTitlebar();

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

		SPA::UI::SetDarkTheme();

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

		// Load images
		{
			uint32_t w, h;
			void* data = CVulkanImage::Decode(g_ScamPALogo, sizeof(g_ScamPALogo), w, h);
			m_app_header_icon = std::make_shared<SPA::CVulkanImage>(w, h, EImageFormat::RGBA, data);
			free(data);
		}
		{
			uint32_t w, h;
			void* data = CVulkanImage::Decode(g_WindowMinimizeIcon, sizeof(g_WindowMinimizeIcon), w, h);
			m_icon_minimize = std::make_shared<SPA::CVulkanImage>(w, h, EImageFormat::RGBA, data);
			free(data);
		}
		{
			uint32_t w, h;
			void* data = CVulkanImage::Decode(g_WindowMaximizeIcon, sizeof(g_WindowMaximizeIcon), w, h);
			m_icon_maximize = std::make_shared<SPA::CVulkanImage>(w, h, EImageFormat::RGBA, data);
			free(data);
		}
		{
			uint32_t w, h;
			void* data = CVulkanImage::Decode(g_WindowRestoreIcon, sizeof(g_WindowRestoreIcon), w, h);
			m_icon_restore = std::make_shared<SPA::CVulkanImage>(w, h, EImageFormat::RGBA, data);
			free(data);
		}
		{
			uint32_t w, h;
			void* data = CVulkanImage::Decode(g_WindowCloseIcon, sizeof(g_WindowCloseIcon), w, h);
			m_icon_close = std::make_shared<SPA::CVulkanImage>(w, h, EImageFormat::RGBA, data);
			free(data);
		}
	}

	void CImGuiLayer::OnDetach() {
		// Release icon resources
		m_app_header_icon.reset();
		m_icon_close.reset();
		m_icon_minimize.reset();
		m_icon_maximize.reset();
		m_icon_restore.reset();

		// LIFO backend shutdown
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
		if (m_is_blocking_events) {
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

		if (!m_use_custom_titlebar && m_menubar_callback) {
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

		// Draw custom titlebar if enabled
		if (m_use_custom_titlebar) {
			float titlebar_height = 0.0f; // populated via RenderCustomTitlebar()
			RenderCustomTitlebar(titlebar_height);
			ImGui::SetCursorPosY(titlebar_height);
		}

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("ScamPADockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		// Render menu bar if we're not using a custom titlebar, since it draws its own
		if (!m_use_custom_titlebar && m_menubar_callback) {
			if (ImGui::BeginMenuBar()) {
				m_menubar_callback();
				ImGui::EndMenuBar();
			}
		}

		ImGui::End();
	}

	void CImGuiLayer::RenderCustomMenubar() {
		if (!m_menubar_callback)
			return;

		if (m_use_custom_titlebar) {
			const ImRect menu_bar_rect = { ImGui::GetCursorPos(), { ImGui::GetContentRegionAvail().x + ImGui::GetCursorScreenPos().x, ImGui::GetFrameHeightWithSpacing() } };

			ImGui::BeginGroup();
			if (UI::BeginMenubar(menu_bar_rect)) {
				m_menubar_callback();
			}

			UI::EndMenubar();
			ImGui::EndGroup();

		}
		else {
			if (ImGui::BeginMenuBar()) {
				m_menubar_callback();
				ImGui::EndMenuBar();
			}
		}
	}

	void CImGuiLayer::RenderCustomTitlebar(float& a_out_titlebar_height) {
		CApplication& app = CApplication::GetApplicationInstance();
		SApplicationSpecification app_spec = app.GetSpecification();
		app_spec.m_name = app.GetApplicationVersion();

		CWindow& window = app.GetApplicationInstance().GetWindowHandle();

		const float titlebar_height = 58.0f;
		const bool is_maximized = window.IsMaximized();
		float titlebar_vertical_offset = is_maximized ? -6.0f : 0.0f;
		const ImVec2 window_padding = ImGui::GetCurrentWindow()->WindowPadding;

		ImGui::SetCursorPos(ImVec2(window_padding.x, window_padding.y + titlebar_vertical_offset));
		const ImVec2 titlebar_min = ImGui::GetCursorScreenPos();
		const ImVec2 titlebar_max = { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - window_padding.y * 2.0f,
									 ImGui::GetCursorScreenPos().y + titlebar_height };
		auto* bg_draw_list = ImGui::GetBackgroundDrawList();
		auto* fg_draw_list = ImGui::GetForegroundDrawList();
		bg_draw_list->AddRectFilled(titlebar_min, titlebar_max, UI::Colors::Theme::titlebar);
		// DEBUG TITLEBAR BOUNDS
		// fg_draw_list->AddRect(titlebar_min, titlebar_max, UI::Colors::Theme::invalidPrefab);

		// Logo
		{
			const int logoWidth = 50;// m_LogoTex->GetWidth();
			const int logoHeight = 50;// m_LogoTex->GetHeight();
			const ImVec2 logoOffset(16.0f + window_padding.x, 5.0f + window_padding.y + titlebar_vertical_offset);
			const ImVec2 logoRectStart = { ImGui::GetItemRectMin().x + logoOffset.x, ImGui::GetItemRectMin().y + logoOffset.y };
			const ImVec2 logoRectMax = { logoRectStart.x + logoWidth, logoRectStart.y + logoHeight };
			fg_draw_list->AddImage(m_app_header_icon->GetDescriptorSet(), logoRectStart, logoRectMax);
		}

		ImGui::BeginHorizontal("Titlebar", { ImGui::GetWindowWidth() - window_padding.y * 2.0f, ImGui::GetFrameHeightWithSpacing() });

		static float move_offset_x;
		static float move_offset_y;
		const float w = ImGui::GetContentRegionAvail().x;
		const float buttons_area_width = 94;

		// Title bar drag area
		// On Windows we hook into the GLFW win32 window internals
		ImGui::SetCursorPos(ImVec2(window_padding.x, window_padding.y + titlebar_vertical_offset)); // Reset cursor pos
		// DEBUG DRAG BOUNDS
		// fg_draw_list->AddRect(ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetCursorScreenPos().x + w - buttons_area_width, ImGui::GetCursorScreenPos().y + titlebar_height), UI::Colors::Theme::invalidPrefab);
		ImGui::InvisibleButton("##titleBarDragZone", ImVec2(w - buttons_area_width, titlebar_height));

		m_is_titlebar_hovered = ImGui::IsItemHovered();

		if (is_maximized) {
			float window_mouse_pos_y = ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y;
			if (window_mouse_pos_y >= 0.0f && window_mouse_pos_y <= 5.0f)
				m_is_titlebar_hovered = true; // Account for the top-most pixels which don't register
		}

		// Draw Menubar
		if (m_menubar_callback) {
			ImGui::SuspendLayout();
			{
				ImGui::SetItemAllowOverlap();
				const float logoHorizontalOffset = 16.0f * 2.0f + 48.0f + window_padding.x;
				ImGui::SetCursorPos(ImVec2(logoHorizontalOffset, 6.0f + titlebar_vertical_offset));
				RenderCustomMenubar();

				if (ImGui::IsItemHovered())
					m_is_titlebar_hovered = false;
			}

			ImGui::ResumeLayout();
		}

		{
			// Centered Window title
			ImVec2 current_cursor_pos = ImGui::GetCursorPos();
			ImVec2 text_size = ImGui::CalcTextSize(app_spec.m_name.c_str());
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() * 0.5f - text_size.x * 0.5f, 2.0f + window_padding.y + 6.0f));
			ImGui::Text("%s", app_spec.m_name.c_str()); // Draw title
			ImGui::SetCursorPos(current_cursor_pos);
		}

		// Window buttons
		const ImU32 button_col_n = UI::Colors::ColorWithMultipliedValue(UI::Colors::Theme::text, 0.9f);
		const ImU32 button_col_h = UI::Colors::ColorWithMultipliedValue(UI::Colors::Theme::text, 1.2f);
		const ImU32 button_col_p = UI::Colors::Theme::text_darker;
		const float button_width = 14.0f;
		const float button_height = 14.0f;

		// Minimize Button

		ImGui::Spring();
		UI::ShiftCursorY(8.0f);
		{
			const int icon_width = m_icon_minimize->GetWidth();
			const int icon_height = m_icon_minimize->GetHeight();
			const float pad_y = (button_height - (float)icon_height) / 2.0f;
			if (ImGui::InvisibleButton("Minimize", ImVec2(button_width, button_height))) {
				window.Minimize();
			}

			UI::DrawButtonImage(m_icon_minimize, button_col_n, button_col_h, button_col_p, UI::RectExpanded(UI::GetItemRect(), 0.0f, -pad_y));
		}


		// Maximize Button
		ImGui::Spring(-1.0f, 17.0f);
		UI::ShiftCursorY(8.0f);
		{
			const int icon_width = m_icon_maximize->GetWidth();
			const int icon_height = m_icon_maximize->GetHeight();

			//const bool is_maximized = window.IsMaximized();

			if (ImGui::InvisibleButton("Maximize", ImVec2(button_width, button_height))) {
				if (is_maximized) {
					window.Restore();
				}
				else {
					window.Maximize();
				}
			}

			UI::DrawButtonImage(is_maximized ? m_icon_restore : m_icon_maximize, button_col_n, button_col_h, button_col_p);
		}

		// Close Button
		ImGui::Spring(-1.0f, 15.0f);
		UI::ShiftCursorY(8.0f);
		{
			const int icon_width = m_icon_close->GetWidth();
			const int icon_height = m_icon_close->GetHeight();
			if (ImGui::InvisibleButton("Close", ImVec2(button_width, button_height))) {
				app.Close();
			}

			UI::DrawButtonImage(m_icon_close, UI::Colors::Theme::text, UI::Colors::ColorWithMultipliedValue(UI::Colors::Theme::text, 1.4f), button_col_p);
		}

		ImGui::Spring(-1.0f, 18.0f);
		ImGui::EndHorizontal();

		a_out_titlebar_height = titlebar_height;
	}
	
	uint32_t CImGuiLayer::GetActiveWidgetID() const {
		return GImGui->ActiveId;
	}

}