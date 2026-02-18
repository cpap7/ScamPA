-- ScamPAExternal.lua

VULKAN_SDK = os.getenv("VULKAN_SDK")

-- Dependency folders relative to the root folder
IncludeDir = {}
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["glm"] = "../Vendor/glm"
IncludeDir["imgui"] = "../Vendor/imgui"
IncludeDir["glfw"] = "../Vendor/glfw/include"
IncludeDir["stb_image"] = "../Vendor/stb_image"
IncludeDir["spdlog"] = "../Vendor/spdlog/include"

-- Binary libs relative to the root folder
LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"

group "Dependencies"
   include "vendor/imgui"
   include "vendor/glfw"
group ""

group "Core"
	include "ScamPA-Core"
group ""