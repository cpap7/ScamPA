#include "spapch.h"
#include "VulkanSwapchain.h"
#include "VulkanContext.h"

namespace SPA {
	
	CVulkanSwapchain::CVulkanSwapchain(CVulkanContext& a_graphics_context, VkSurfaceKHR a_surface, uint32_t a_width, uint32_t a_height) 
		: m_graphics_context(a_graphics_context) {
		m_window_data.Surface = a_surface;

		// Check for WSI support
		VkBool32 result;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_graphics_context.GetPhysicalDevice(), m_graphics_context.GetQueueFamily(), m_window_data.Surface, &result);
		if (result != VK_TRUE) {
			fprintf(stderr, "Error no WSI support on physical device 0\n");
			exit(-1);
		}

		// Select Surface Format
		const VkFormat request_surface_image_format[] = { 
			VK_FORMAT_B8G8R8A8_UNORM, 
			VK_FORMAT_R8G8B8A8_UNORM, 
			VK_FORMAT_B8G8R8_UNORM, 
			VK_FORMAT_R8G8B8_UNORM 
		};
		const VkColorSpaceKHR request_surface_color_space = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		m_window_data.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(
			m_graphics_context.GetPhysicalDevice(),
			m_window_data.Surface, 
			request_surface_image_format, 
			(size_t)SPA_ARRAYSIZE(request_surface_image_format), 
			request_surface_color_space
		);

		// Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
		VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
		VkPresentModeKHR present_modes[] = { 
			VK_PRESENT_MODE_FIFO_KHR 
		};
#endif
		m_window_data.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(
			m_graphics_context.GetPhysicalDevice(),
			m_window_data.Surface, 
			&present_modes[0], 
			SPA_ARRAYSIZE(present_modes)
		);
		
		
		//printf("[vulkan] Selected PresentMode = %d\n", m_window_data.PresentMode);

		// Create SwapChain, RenderPass, Framebuffer, etc.
		SPA_CORE_ASSERT(m_graphics_context.GetConfig().m_min_image_count >= 2, "(Vulkan) Invalid minimum image count!");
		m_min_image_count = m_graphics_context.GetConfig().m_min_image_count;

		ImGui_ImplVulkanH_CreateOrResizeWindow(
			m_graphics_context.GetInstance(), 
			m_graphics_context.GetPhysicalDevice(),
			m_graphics_context.GetDevice(),
			&m_window_data, 
			m_graphics_context.GetQueueFamily(),
			m_graphics_context.GetAllocator(),
			a_width, 
			a_height, 
			m_min_image_count
		);

		m_allocated_command_buffers.resize(m_window_data.ImageCount);
	}

	CVulkanSwapchain::~CVulkanSwapchain() {
		ImGui_ImplVulkanH_DestroyWindow(
			m_graphics_context.GetInstance(),
			m_graphics_context.GetDevice(),
			&m_window_data,
			m_graphics_context.GetAllocator()
		);
	}

	void CVulkanSwapchain::Resize(uint32_t a_width, uint32_t a_height) {
		if (a_width > 0 && a_height > 0) {
			ImGui_ImplVulkan_SetMinImageCount(m_min_image_count);
			ImGui_ImplVulkanH_CreateOrResizeWindow(
				m_graphics_context.GetInstance(),
				m_graphics_context.GetPhysicalDevice(),
				m_graphics_context.GetDevice(),
				&m_window_data,
				m_graphics_context.GetQueueFamily(),
				m_graphics_context.GetAllocator(),
				a_width,
				a_height,
				m_min_image_count
			);
			m_window_data.FrameIndex = 0;

			// Clear allocated command buffers
			m_allocated_command_buffers.clear();
			m_allocated_command_buffers.resize(m_window_data.ImageCount);

			m_needs_rebuild = false;
		}
	}

	void CVulkanSwapchain::BeginFrame() {
		VkResult error;

		// Wait for all fences to avoid semaphore reuse issues in multi-viewport mode
		// Note: ImGui::RenderPlatformWindowsDefault() can use semaphores that need to complete first
		for (uint32_t i = 0; i < m_window_data.ImageCount; i++) {
			ImGui_ImplVulkanH_Frame* frame = &m_window_data.Frames[i];
			if (frame->Fence != VK_NULL_HANDLE) {
				vkWaitForFences(m_graphics_context.GetDevice(), 1, &frame->Fence, VK_TRUE, UINT64_MAX);
			}
		}
		
		// Per Vulkan 1.3.280.0 specification, we advance to the next semaphore pair before using it
		//m_window_data.SemaphoreIndex = (m_window_data.SemaphoreIndex + 1) % m_window_data.ImageCount;

		VkSemaphore image_acquired_semaphore = m_window_data.FrameSemaphores[m_window_data.SemaphoreIndex].ImageAcquiredSemaphore;
		VkSemaphore render_complete_semaphore = m_window_data.FrameSemaphores[m_window_data.SemaphoreIndex].RenderCompleteSemaphore;

		error = vkAcquireNextImageKHR(
			m_graphics_context.GetDevice(),
			m_window_data.Swapchain, 
			UINT64_MAX, 
			image_acquired_semaphore, 
			VK_NULL_HANDLE, 
			&m_window_data.FrameIndex
		);
		if (error == VK_ERROR_OUT_OF_DATE_KHR || error == VK_SUBOPTIMAL_KHR) {
			m_needs_rebuild = true;
			return;
		}
		Utilities::CheckVkResult(error);


		ImGui_ImplVulkanH_Frame* frame_data = &m_window_data.Frames[m_window_data.FrameIndex];
		
		// Wait indefinitely instead of periodically checking
		error = vkWaitForFences(m_graphics_context.GetDevice(), 1, &frame_data->Fence, VK_TRUE, UINT64_MAX);    
		Utilities::CheckVkResult(error);

		error = vkResetFences(m_graphics_context.GetDevice(), 1, &frame_data->Fence);
		Utilities::CheckVkResult(error);
		
		m_current_frame_index = (m_current_frame_index + 1) % m_window_data.ImageCount;

		{ // Free resources in queue
			m_graphics_context.ProcessResourceFreeQueue(m_current_frame_index);
		}
		{
			// Free command buffers allocated by GetCommandBuffer
			// These use m_window_data.FrameIndex and not m_current_frame_index because they're tied to the swapchain image index
			auto& allocated_command_buffers = m_allocated_command_buffers[m_window_data.FrameIndex];
			if (allocated_command_buffers.size() > 0) {
				vkFreeCommandBuffers(m_graphics_context.GetDevice(), frame_data->CommandPool, (uint32_t)allocated_command_buffers.size(), allocated_command_buffers.data());
				allocated_command_buffers.clear();
			}

			error = vkResetCommandPool(m_graphics_context.GetDevice(), frame_data->CommandPool, 0);
			Utilities::CheckVkResult(error);
			
			VkCommandBufferBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			
			error = vkBeginCommandBuffer(frame_data->CommandBuffer, &info);
			Utilities::CheckVkResult(error);
		}
		{ // Begin render pass
			VkRenderPassBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderPass = m_window_data.RenderPass;
			info.framebuffer = frame_data->Framebuffer;
			info.renderArea.extent.width = m_window_data.Width;
			info.renderArea.extent.height = m_window_data.Height;
			info.clearValueCount = 1;
			info.pClearValues = &m_window_data.ClearValue;
			
			vkCmdBeginRenderPass(frame_data->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
		}
	}

	void CVulkanSwapchain::EndFrame(ImDrawData* a_draw_data) {
		if (m_needs_rebuild) {
			return;
		}

		VkResult error;
		ImGui_ImplVulkanH_Frame* frame_data = &m_window_data.Frames[m_window_data.FrameIndex];

		// Record dear imgui primitives into command buffer
		ImGui_ImplVulkan_RenderDrawData(a_draw_data, frame_data->CommandBuffer);

		// Submit command buffer
		vkCmdEndRenderPass(frame_data->CommandBuffer);
		{
			VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkSubmitInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			info.waitSemaphoreCount = 1;
			info.pWaitSemaphores = &m_window_data.FrameSemaphores[m_window_data.SemaphoreIndex].ImageAcquiredSemaphore;
			info.pWaitDstStageMask = &wait_stage;
			info.commandBufferCount = 1;
			info.pCommandBuffers = &frame_data->CommandBuffer;
			info.signalSemaphoreCount = 1;
			info.pSignalSemaphores = &m_window_data.FrameSemaphores[m_window_data.SemaphoreIndex].RenderCompleteSemaphore;

			error = vkEndCommandBuffer(frame_data->CommandBuffer);
			Utilities::CheckVkResult(error);
			
			error = vkQueueSubmit(m_graphics_context.GetQueue(), 1, &info, frame_data->Fence);
			Utilities::CheckVkResult(error);
		}
	}

	void CVulkanSwapchain::PresentFrame() {
		if (m_needs_rebuild) {
			return;
		}
		VkSemaphore render_complete_semaphore = m_window_data.FrameSemaphores[m_window_data.SemaphoreIndex].RenderCompleteSemaphore;
		
		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &render_complete_semaphore;
		info.swapchainCount = 1;
		info.pSwapchains = &m_window_data.Swapchain;
		info.pImageIndices = &m_window_data.FrameIndex;
		
		VkResult error = vkQueuePresentKHR(m_graphics_context.GetQueue(), &info);
		
		if (error == VK_ERROR_OUT_OF_DATE_KHR || error == VK_SUBOPTIMAL_KHR) {
			m_needs_rebuild = true;
			return;
		}
		Utilities::CheckVkResult(error);
		m_window_data.SemaphoreIndex = (m_window_data.SemaphoreIndex + 1) % m_window_data.ImageCount; // Now we can use the next set of semaphores
	}

	VkCommandBuffer CVulkanSwapchain::GetCommandBuffer(bool a_begin_flag) {
		ImGui_ImplVulkanH_Frame* frame_data = &m_window_data.Frames[m_window_data.FrameIndex];
		VkCommandBuffer& command_buffer = m_allocated_command_buffers[m_window_data.FrameIndex].emplace_back();

		command_buffer = m_graphics_context.AllocateCommandBuffer(frame_data->CommandPool, a_begin_flag);
		return command_buffer;
	}
}