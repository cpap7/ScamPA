#pragma once
#include <vector>
#include <functional>

#include "VulkanUtils.h"

namespace SPA {
	// Render context
	// Manages core Vulkan state

	class CVulkanContext {
	public:
		struct SConfig {
			bool m_enable_validation = true;
			uint32_t m_min_image_count = 2;
		};

	private:
		std::vector<std::vector<std::function<void()>>> m_resource_free_queues;
		SConfig m_config;

		VkAllocationCallbacks* m_allocator				= nullptr;
		VkInstance m_instance							= VK_NULL_HANDLE;
		VkPhysicalDevice m_physical_device				= VK_NULL_HANDLE;
		VkDevice m_device								= VK_NULL_HANDLE;
		VkDescriptorPool m_descriptor_pool				= VK_NULL_HANDLE;
		VkDebugReportCallbackEXT m_debug_report			= VK_NULL_HANDLE;
		VkQueue m_queue									= VK_NULL_HANDLE;
		
		uint32_t m_queue_family = (uint32_t)-1;

	public:
		CVulkanContext(const SConfig& a_config);
		~CVulkanContext();

		void Init(const char** a_extensions, uint32_t a_extension_count);

		// Command buffer management
		VkCommandBuffer AllocateCommandBuffer(VkCommandPool a_pool, bool a_begin_flag = true);
		void FlushCommandBuffer(VkCommandBuffer a_command_buffer);

		// Resource management
		void SubmitResourceFree(uint32_t a_frame_index, std::function<void()>&& a_function);
		void ProcessResourceFreeQueue(uint32_t a_frame_index);

		// Getters for Vulkan handles
		inline SConfig GetConfig() const					{ return m_config;			}
		inline VkAllocationCallbacks* GetAllocator() const	{ return m_allocator;		}
		inline VkInstance GetInstance() const				{ return m_instance;		}
		inline VkPhysicalDevice GetPhysicalDevice() const	{ return m_physical_device; }
		inline VkDevice GetDevice() const					{ return m_device;			}
		inline VkDescriptorPool GetDescriptorPool() const	{ return m_descriptor_pool; }
		inline VkQueue GetQueue() const						{ return m_queue;			}
		inline uint32_t GetQueueFamily() const				{ return m_queue_family;	}

	private:
		// Initialization helpers
		void CreateInstance(const char** a_extensions, uint32_t a_extension_count);
		void SelectPhysicalDevice();
		void CreateLogicalDevice();
		void CreateDescriptorPool();
		
	public:
		// Cleanup helper
		void Shutdown();
	};

}
