#pragma once
// TODO: Remove
#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort

#include <vulkan/vulkan.h>

namespace SPA {

	namespace Utilities {

#ifdef SPA_DEBUG
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReport(VkDebugReportFlagsEXT a_flags, VkDebugReportObjectTypeEXT a_object_type, uint64_t a_object,
			size_t a_location, int32_t a_message_code, const char* a_p_layer_prefix,
			const char* a_p_message, void* a_p_user_data) {

			// Unused arguments
			(void)a_flags;
			(void)a_object;
			(void)a_location;
			(void)a_message_code;
			(void)a_p_user_data;
			(void)a_p_layer_prefix;


			fprintf(stderr, "[Vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", a_object_type, a_p_message);
			return VK_FALSE;
		}
#endif // SPA_DEBUG

		static void CheckVkResult(VkResult a_error) {
			if (a_error == 0) {
				return;
			}

			fprintf(stderr, "[Vulkan] Error: VkResult = %d\n", a_error);

			if (a_error < 0) {
				abort();
			}
		}

		

	}
}