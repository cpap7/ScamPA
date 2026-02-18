#pragma once

#include "VulkanUtils.h"
#include <backends/imgui_impl_vulkan.h>


namespace SPA {

	// Forward declarations
	class CVulkanContext;

	class CVulkanSwapchain {
	private:
		std::vector<std::vector<VkCommandBuffer>> m_allocated_command_buffers;
		CVulkanContext& m_graphics_context;
		ImGui_ImplVulkanH_Window m_window_data{};
		uint32_t m_current_frame_index = 0;
		uint32_t m_min_image_count = 2;
		bool m_needs_rebuild = false;

	public:
		CVulkanSwapchain(CVulkanContext& a_graphics_context, VkSurfaceKHR a_surface, uint32_t a_width, uint32_t a_height);
		~CVulkanSwapchain();

		void Resize(uint32_t a_width, uint32_t a_height);
		void BeginFrame();
		void EndFrame(ImDrawData* a_draw_data);
		void PresentFrame();


		// Getters
		VkCommandBuffer GetCommandBuffer(bool a_begin_flag = true);
		inline uint32_t GetCurrentFrameIndex() const		{ return m_current_frame_index;		}
		inline uint32_t GetImageCount() const				{ return m_window_data.ImageCount;	}
		inline ImGui_ImplVulkanH_Window& GetWindowData()	{ return m_window_data;				}
		inline bool NeedsRebuild() const					{ return m_needs_rebuild;			}

		// Setters
		void SetNeedsRebuild(bool a_needs_rebuild_flag)		{ m_needs_rebuild = a_needs_rebuild_flag; }
	};
}

