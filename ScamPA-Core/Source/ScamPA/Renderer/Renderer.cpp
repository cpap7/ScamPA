#include "spapch.h"
#include "Renderer.h"

namespace SPA {
	CRenderer::CRenderer(CVulkanContext& a_graphics_context, CVulkanSwapchain& a_swapchain) 
		: m_graphics_context(a_graphics_context), m_swapchain(a_swapchain) {

	}

	void CRenderer::BeginFrame() {
		m_swapchain.BeginFrame();
	}

	void CRenderer::EndFrame() {
		if (m_swapchain.NeedsRebuild()) {
			return;
		}
		ImDrawData* draw_data = ImGui::GetDrawData();
		
		const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
		if (!is_minimized) {
			m_swapchain.EndFrame(draw_data);
			m_swapchain.PresentFrame();
		}
	}

	void CRenderer::RenderViewports() {
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	VkCommandBuffer CRenderer::GetCommandBuffer(bool a_begin_flag) {
		return m_swapchain.GetCommandBuffer(a_begin_flag);
	}
	
	void CRenderer::SubmitResourceFree(std::function<void()>&& a_function) {
		m_graphics_context.SubmitResourceFree(m_swapchain.GetCurrentFrameIndex(), std::move(a_function));
	}
}