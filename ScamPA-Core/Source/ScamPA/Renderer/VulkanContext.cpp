#include "spapch.h"
#include "VulkanContext.h"

namespace SPA {

	CVulkanContext::CVulkanContext(const SConfig& a_config) 
		: m_config(a_config) {

	}

	CVulkanContext::~CVulkanContext() {
		Shutdown();
	}

	void CVulkanContext::Init(const char** a_extensions, uint32_t a_extension_count) {
		// Setup Vulkan core
		CreateInstance(a_extensions, a_extension_count);
		SelectPhysicalDevice();
		CreateLogicalDevice();
		CreateDescriptorPool();

		// Initialize resource free queues
		m_resource_free_queues.resize(m_config.m_min_image_count);
	}

	VkCommandBuffer CVulkanContext::AllocateCommandBuffer(VkCommandPool a_command_pool, bool a_begin_flag) {
		VkCommandBufferAllocateInfo cmd_buf_allocate_info = {};
		cmd_buf_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmd_buf_allocate_info.commandPool = a_command_pool;
		cmd_buf_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmd_buf_allocate_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		auto error = vkAllocateCommandBuffers(m_device, &cmd_buf_allocate_info, &command_buffer);
		Utilities::CheckVkResult(error);

		if (a_begin_flag) {
			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			error = vkBeginCommandBuffer(command_buffer, &begin_info);
			Utilities::CheckVkResult(error);
		}

		return command_buffer;
	}

	void CVulkanContext::FlushCommandBuffer(VkCommandBuffer a_command_buffer) {
		const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;

		VkSubmitInfo end_info = {};
		end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		end_info.commandBufferCount = 1;
		end_info.pCommandBuffers = &a_command_buffer;

		auto error = vkEndCommandBuffer(a_command_buffer);
		Utilities::CheckVkResult(error);

		// Create fence to ensure that the command buffer has finished executing
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = 0;
		VkFence fence;
		error = vkCreateFence(m_device, &fenceCreateInfo, nullptr, &fence);
		Utilities::CheckVkResult(error);

		error = vkQueueSubmit(m_queue, 1, &end_info, fence);
		Utilities::CheckVkResult(error);

		error = vkWaitForFences(m_device, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT);
		Utilities::CheckVkResult(error);

		vkDestroyFence(m_device, fence, nullptr);
	}

	void CVulkanContext::SubmitResourceFree(uint32_t a_current_frame_index, std::function<void()>&& a_function) {
		m_resource_free_queues[a_current_frame_index].emplace_back(a_function);
	}

	void CVulkanContext::ProcessResourceFreeQueue(uint32_t a_frame_index) {
		for (auto& function : m_resource_free_queues[a_frame_index]) {
			function();
		}
		m_resource_free_queues[a_frame_index].clear();
	}

	void CVulkanContext::CreateInstance(const char** a_extensions, uint32_t a_extension_count) {
		VkResult error;

		// Create Vulkan Instance
		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.enabledExtensionCount = a_extension_count;
		create_info.ppEnabledExtensionNames = a_extensions;

#ifdef SPA_DEBUG
		// Enabling validation layers
		const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
		create_info.enabledLayerCount = 1;
		create_info.ppEnabledLayerNames = layers;

		// Enable debug report extension (we need additional storage, so we duplicate the user array to add our new extension to it)
		const char** extensions_ext = (const char**)malloc(sizeof(const char*) * (a_extension_count + 1));
		memcpy(extensions_ext, a_extensions, a_extension_count * sizeof(const char*));
		extensions_ext[a_extension_count] = "VK_EXT_debug_report";
		create_info.enabledExtensionCount = a_extension_count + 1;
		create_info.ppEnabledExtensionNames = extensions_ext;

		// Create Vulkan Instance
		error = vkCreateInstance(&create_info, m_allocator, &m_instance);
		Utilities::CheckVkResult(error);
		free(extensions_ext);

		// Get the function pointer (required for any extensions)
		auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT");
		SPA_CORE_ASSERT(vkCreateDebugReportCallbackEXT != NULL, "(Vulkan) Debug report callback error!");

		// Setup the debug report callback
		VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
		debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		debug_report_ci.pfnCallback = Utilities::DebugReport;
		debug_report_ci.pUserData = NULL;
		error = vkCreateDebugReportCallbackEXT(m_instance, &debug_report_ci, m_allocator, &m_debug_report);
		Utilities::CheckVkResult(error);
#else
		// Create Vulkan Instance without any debug feature
		error = vkCreateInstance(&create_info, m_allocator, &m_instance);
		CheckVkResult(error);
		IM_UNUSED(m_debug_report);
#endif

	}

	void CVulkanContext::SelectPhysicalDevice() {
		VkResult error;

		// Select GPU device
		uint32_t gpu_count;
		error = vkEnumeratePhysicalDevices(m_instance, &gpu_count, NULL);
		Utilities::CheckVkResult(error);
		SPA_CORE_ASSERT(gpu_count > 0, "(Vulkan) No GPUs detected!");

		VkPhysicalDevice* gpus = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpu_count);
		error = vkEnumeratePhysicalDevices(m_instance, &gpu_count, gpus);
		Utilities::CheckVkResult(error);

		// If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
		// most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
		// dedicated GPUs) is out of scope here
		int use_gpu = 0;
		for (int i = 0; i < (int)gpu_count; i++) {
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(gpus[i], &properties);
			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				use_gpu = i;
				break;
			}
		}

		m_physical_device = gpus[use_gpu];
		free(gpus);
	}

	void CVulkanContext::CreateLogicalDevice() {
		VkResult error;

		{ // Select graphics queue family
			uint32_t count;
			vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &count, NULL);

			VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
			vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &count, queues);

			for (uint32_t i = 0; i < count; i++)
				if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					m_queue_family = i;
					break;
				}
			free(queues);
			SPA_CORE_ASSERT(m_queue_family != (uint32_t)-1, "(Vulkan) Queue family error!");
		}

		{ // Create Logical Device (with 1 queue)
			int device_extension_count = 1;
			const char* device_extensions[] = { "VK_KHR_swapchain" };
			const float queue_priority[] = { 1.0f };

			VkDeviceQueueCreateInfo queue_info[1] = {};
			queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_info[0].queueFamilyIndex = m_queue_family;
			queue_info[0].queueCount = 1;
			queue_info[0].pQueuePriorities = queue_priority;

			VkDeviceCreateInfo create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
			create_info.pQueueCreateInfos = queue_info;
			create_info.enabledExtensionCount = device_extension_count;
			create_info.ppEnabledExtensionNames = device_extensions;

			error = vkCreateDevice(m_physical_device, &create_info, m_allocator, &m_device);
			Utilities::CheckVkResult(error);
			vkGetDeviceQueue(m_device, m_queue_family, 0, &m_queue);
		}
	}
	
	void CVulkanContext::CreateDescriptorPool() {
		VkResult error;
		
		// Create Descriptor Pool
		VkDescriptorPoolSize pool_sizes[] = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * SPA_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)SPA_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		error = vkCreateDescriptorPool(m_device, &pool_info, m_allocator, &m_descriptor_pool);
		Utilities::CheckVkResult(error);
	}

	void CVulkanContext::Shutdown() {
		// Cleanup

		if (m_device) {
			//VkResult error = vkDeviceWaitIdle(m_device);
			//Utilities::CheckVkResult(error);

			// Free resources in queue
			for (auto& queue : m_resource_free_queues) {
				for (auto& func : queue) {
					func();
				}
			}
			m_resource_free_queues.clear();

			if (m_descriptor_pool) {
				vkDestroyDescriptorPool(m_device, m_descriptor_pool, m_allocator);
				m_descriptor_pool = VK_NULL_HANDLE;
			}

			vkDestroyDevice(m_device, m_allocator);
			m_device = VK_NULL_HANDLE;
		}

#ifdef SPA_DEBUG
		if (m_debug_report) {
			auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT");
			vkDestroyDebugReportCallbackEXT(m_instance, m_debug_report, m_allocator);
			m_debug_report = VK_NULL_HANDLE;
		}
#endif // SPA_DEBUG

		if (m_instance) {
			vkDestroyInstance(m_instance, m_allocator);
			m_instance = VK_NULL_HANDLE;
		}

	}
}