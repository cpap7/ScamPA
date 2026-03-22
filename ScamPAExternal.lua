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
IncludeDir["miniaudio"] = "../Vendor/miniaudio"

-- VoxBoxSDK
IncludeDir["VoxBoxCommon"] = "../Vendor/VoxBoxSDK/VoxBox-Common/Source"
IncludeDir["VoxBoxSTT"] = "../Vendor/VoxBoxSDK/VoxBox-WhisperAPI/Source/API"
IncludeDir["VoxBoxTTS"] = "../Vendor/VoxBoxSDK/VoxBox-PiperAPI/Source/API"
IncludeDir["VoxBoxLLM"] = "../Vendor/VoxBoxSDK/VoxBox-LlamaAPI/Source/API"

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

group "Application"
	include "ScamPA-App"
group ""

group "VoxBoxSDK"
	include "Vendor/VoxBoxSDK/VoxBox-Common"
	include "Vendor/VoxBoxSDK/VoxBox-WhisperAPI"
	include "Vendor/VoxBoxSDK/VoxBox-PiperAPI"
	include "Vendor/VoxBoxSDK/VoxBox-LlamaAPI"
group ""