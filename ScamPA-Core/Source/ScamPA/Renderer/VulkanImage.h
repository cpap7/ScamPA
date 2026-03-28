#pragma once

#include <string>

#include <vulkan/vulkan.h>

namespace SPA {

	enum class EImageFormat {
		None = 0,
		RGBA,
		RGBA32F
	};

	class CVulkanImage {
	private:
		std::string m_file_path;

		VkImage m_image							= nullptr;
		VkImageView m_image_view				= nullptr;
		VkDeviceMemory m_memory					= nullptr;
		VkSampler m_sampler						= nullptr;
		VkBuffer m_staging_buffer				= nullptr;
		VkDeviceMemory m_staging_buffer_memory	= nullptr;
		VkDescriptorSet m_descriptor_set		= nullptr;

		size_t m_aligned_size = 0;

		EImageFormat m_format = EImageFormat::None;

		uint32_t m_width = 0, m_height = 0;

	public:
		CVulkanImage(std::string_view a_path);
		CVulkanImage(uint32_t a_width, uint32_t height, EImageFormat a_format, const void* a_data = nullptr);
		~CVulkanImage();

		// Getters
		inline uint32_t GetWidth() const				{ return m_width;			}
		inline uint32_t GetHeight() const				{ return m_height;			}
		inline VkDescriptorSet GetDescriptorSet() const { return m_descriptor_set;	}

		// Setters
		void SetData(const void* a_data);

		static void* Decode(const void* a_buffer, uint64_t a_length, uint32_t& a_out_width, uint32_t& a_out_height);
		void Resize(uint32_t a_width, uint32_t a_height);

	private:
		void AllocateMemory(uint64_t size);
		void Release();
	
	};

}



