#include "spapch.h"
#include "VulkanImage.h"
#include "ScamPA/Core/Application.h"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace SPA {

	namespace Utilities {

		static uint32_t GetVulkanMemoryType(VkMemoryPropertyFlags a_properties, uint32_t a_type_bits) {
			VkPhysicalDeviceMemoryProperties properties;
			vkGetPhysicalDeviceMemoryProperties(CApplication::GetApplicationInstance().GetRenderer().GetGraphicsContext().GetPhysicalDevice(), &properties);
			for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
				if ((properties.memoryTypes[i].propertyFlags & a_properties) == a_properties && a_type_bits & (1 << i)) {
					return i;
				}
			}
			SPA_CORE_ERROR("(Image) Failed to find suitable memory type!");
			return 0xffffffff;
		}

		static uint32_t GetBytesPerPixel(EImageFormat format) {
			switch (format) {
				case EImageFormat::RGBA:    return 4;
				case EImageFormat::RGBA32F: return 16;
			}
			SPA_CORE_ERROR("(Image) Unknown image format!");
			return 0;
		}
		
		static VkFormat ScamPAFormatToVulkanFormat(EImageFormat a_format) {
			switch (a_format) {
				case EImageFormat::RGBA:    return VK_FORMAT_R8G8B8A8_UNORM;
				case EImageFormat::RGBA32F: return VK_FORMAT_R32G32B32A32_SFLOAT;
			}
			SPA_CORE_ERROR("(Image) Unknown image format!");
			return (VkFormat)0;
		}

	}

	CVulkanImage::CVulkanImage(std::string_view a_path)
		: m_file_path(a_path) {
		int width, height, channels;
		uint8_t* data = nullptr;

		if (stbi_is_hdr(m_file_path.c_str())) {
			data = (uint8_t*)stbi_loadf(m_file_path.c_str(), &width, &height, &channels, 4);
			m_format = EImageFormat::RGBA32F;
		}
		else {
			data = stbi_load(m_file_path.c_str(), &width, &height, &channels, 4);
			m_format = EImageFormat::RGBA;
		}

		m_width = width;
		m_height = height;
		
		AllocateMemory(m_width * m_height * Utilities::GetBytesPerPixel(m_format));
		SetData(data);
		stbi_image_free(data);
	}

	CVulkanImage::CVulkanImage(uint32_t a_width, uint32_t a_height, EImageFormat a_format, const void* a_data)
		: m_width(a_width), m_height(a_height), m_format(a_format) {
	
		AllocateMemory(m_width * m_height * Utilities::GetBytesPerPixel(m_format));
		if (a_data) { 
			SetData(a_data);
		}
	}

	CVulkanImage::~CVulkanImage() {
		Release();
	}

	void CVulkanImage::AllocateMemory(uint64_t size) {
		CApplication& app = CApplication::GetApplicationInstance();
		VkDevice device = app.GetRenderer().GetGraphicsContext().GetDevice();
		VkResult error_result;
		VkFormat vulkan_format = Utilities::ScamPAFormatToVulkanFormat(m_format);
		
		{ // Create the Image:
			VkImageCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			info.imageType = VK_IMAGE_TYPE_2D;
			info.format = vulkan_format;
			info.extent.width = m_width;
			info.extent.height = m_height;
			info.extent.depth = 1;
			info.mipLevels = 1;
			info.arrayLayers = 1;
			info.samples = VK_SAMPLE_COUNT_1_BIT;
			info.tiling = VK_IMAGE_TILING_OPTIMAL;
			info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			
			error_result = vkCreateImage(device, &info, nullptr, &m_image);
			Utilities::CheckVkResult(error_result);
			
			VkMemoryRequirements requirements;
			vkGetImageMemoryRequirements(device, m_image, &requirements);
			
			VkMemoryAllocateInfo alloc_info = {};
			alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			alloc_info.allocationSize = requirements.size;
			alloc_info.memoryTypeIndex = Utilities::GetVulkanMemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, requirements.memoryTypeBits);
			
			error_result = vkAllocateMemory(device, &alloc_info, nullptr, &m_memory);
			Utilities::CheckVkResult(error_result);
			
			error_result = vkBindImageMemory(device, m_image, m_memory, 0);
			Utilities::CheckVkResult(error_result);
		}

		{ // Create the Image View:
			VkImageViewCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.image = m_image;
			info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			info.format = vulkan_format;
			info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			info.subresourceRange.levelCount = 1;
			info.subresourceRange.layerCount = 1;
			
			error_result = vkCreateImageView(device, &info, nullptr, &m_image_view);
			Utilities::CheckVkResult(error_result);
		}

		{ 	// Create sampler:
			VkSamplerCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			info.magFilter = VK_FILTER_LINEAR;
			info.minFilter = VK_FILTER_LINEAR;
			info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			info.minLod = -1000;
			info.maxLod = 1000;
			info.maxAnisotropy = 1.0f;
			
			VkResult error_result = vkCreateSampler(device, &info, nullptr, &m_sampler);
			Utilities::CheckVkResult(error_result);
		}

		// Create the Descriptor Set for ImGui:
		m_descriptor_set = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(m_sampler, m_image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void CVulkanImage::Release() {
		CApplication& app = CApplication::GetApplicationInstance();

		app.GetRenderer().SubmitResourceFree([sampler = m_sampler, image_view = m_image_view, image = m_image,
			memory = m_memory, staging_buffer = m_staging_buffer, staging_buffer_memory = m_staging_buffer_memory]()
		{
			VkDevice device = CApplication::GetApplicationInstance().GetRenderer().GetGraphicsContext().GetDevice();

			vkDestroySampler(device, sampler, nullptr);
			vkDestroyImageView(device, image_view, nullptr);
			vkDestroyImage(device, image, nullptr);
			vkFreeMemory(device, memory, nullptr);
			vkDestroyBuffer(device, staging_buffer, nullptr);
			vkFreeMemory(device, staging_buffer_memory, nullptr);
		});

		m_sampler = nullptr;
		m_image_view = nullptr;
		m_image = nullptr;
		m_memory = nullptr;
		m_staging_buffer = nullptr;
		m_staging_buffer_memory = nullptr;
	}

	void CVulkanImage::SetData(const void* data) {
		CApplication& app = CApplication::GetApplicationInstance();
		VkDevice device = app.GetRenderer().GetGraphicsContext().GetDevice();
		size_t upload_size = m_width * m_height * Utilities::GetBytesPerPixel(m_format);
		VkResult error_result;

		if (!m_staging_buffer) {
			{ // Create the Upload Buffer

				VkBufferCreateInfo buffer_info = {};
				buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				buffer_info.size = upload_size;
				buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				
				error_result = vkCreateBuffer(device, &buffer_info, nullptr, &m_staging_buffer);
				Utilities::CheckVkResult(error_result);
				
				VkMemoryRequirements req;
				vkGetBufferMemoryRequirements(device, m_staging_buffer, &req);
				m_aligned_size = req.size;
				
				VkMemoryAllocateInfo alloc_info = {};
				alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				alloc_info.allocationSize = req.size;
				alloc_info.memoryTypeIndex = Utilities::GetVulkanMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
				
				error_result = vkAllocateMemory(device, &alloc_info, nullptr, &m_staging_buffer_memory);
				Utilities::CheckVkResult(error_result);
				
				error_result = vkBindBufferMemory(device, m_staging_buffer, m_staging_buffer_memory, 0);
				Utilities::CheckVkResult(error_result);
			}

		}

		{ // Upload to Buffer

			char* map = NULL;
			
			error_result = vkMapMemory(device, m_staging_buffer_memory, 0, m_aligned_size, 0, (void**)(&map));
			Utilities::CheckVkResult(error_result);
			memcpy(map, data, upload_size);

			VkMappedMemoryRange range[1] = {};
			range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			range[0].memory = m_staging_buffer_memory;
			range[0].size = m_aligned_size;
			
			error_result = vkFlushMappedMemoryRanges(device, 1, range);
			Utilities::CheckVkResult(error_result);
			vkUnmapMemory(device, m_staging_buffer_memory);
		}

		{ // Copy to Image

			VkCommandBuffer command_buffer = app.GetRenderer().GetCommandBuffer(true);

			VkImageMemoryBarrier copy_barrier = {};
			copy_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			copy_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			copy_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			copy_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			copy_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			copy_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			copy_barrier.image = m_image;
			copy_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copy_barrier.subresourceRange.levelCount = 1;
			copy_barrier.subresourceRange.layerCount = 1;
			vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &copy_barrier);

			VkBufferImageCopy region = {};
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.layerCount = 1;
			region.imageExtent.width = m_width;
			region.imageExtent.height = m_height;
			region.imageExtent.depth = 1;
			vkCmdCopyBufferToImage(command_buffer, m_staging_buffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			VkImageMemoryBarrier use_barrier = {};
			use_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			use_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			use_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			use_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			use_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			use_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			use_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			use_barrier.image = m_image;
			use_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			use_barrier.subresourceRange.levelCount = 1;
			use_barrier.subresourceRange.layerCount = 1;
			vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &use_barrier);

			app.GetRenderer().GetGraphicsContext().FlushCommandBuffer(command_buffer);
		}
	}

	void CVulkanImage::Resize(uint32_t a_width, uint32_t a_height) {
		if (m_image && m_width == a_width && m_height == a_height) {
			return;
		}
		// TODO: max size?

		m_width = a_width;
		m_height = a_height;

		Release();
		AllocateMemory(m_width * m_height * Utilities::GetBytesPerPixel(m_format));
	}

}
