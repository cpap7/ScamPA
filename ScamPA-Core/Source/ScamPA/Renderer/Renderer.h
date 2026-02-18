#pragma once
#include <memory>
#include "VulkanContext.h"
#include "VulkanSwapchain.h"

//typedef struct VkCommandBuffer_T* VkCommandBuffer;

namespace SPA {
	// Forward declarations
	class CWindow;

	class CRenderer {
	private:
		CVulkanContext& m_graphics_context;
		CVulkanSwapchain& m_swapchain;

	public:
		CRenderer(CVulkanContext& a_context, CVulkanSwapchain& a_swapchain);

		// Render lifecycle
		void BeginFrame();
		void EndFrame();
		void RenderViewports();

		// Utility methods
		VkCommandBuffer GetCommandBuffer(bool a_begin_flag = true);
		void SubmitResourceFree(std::function<void()>&& a_function);

		inline CVulkanContext& GetGraphicsContext() { return m_graphics_context; }
		inline CVulkanSwapchain& GetSwapchain()		{ return m_swapchain;		 }
	};
}

