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
IncludeDir["yaml_cpp"] = "../Vendor/yaml-cpp/include"
IncludeDir["json"] = "../Vendor/json"
IncludeDir["pjsip"] = "../Vendor/pjsip/include"

-- VoxBoxSDK
IncludeDir["VoxBoxCommon"] = "../Vendor/VoxBoxSDK/VoxBox-Common/Source"
IncludeDir["VoxBoxSTT"] = "../Vendor/VoxBoxSDK/VoxBox-WhisperAPI/Source/API"
IncludeDir["VoxBoxTTS"] = "../Vendor/VoxBoxSDK/VoxBox-PiperAPI/Source/API"
IncludeDir["VoxBoxLLM"] = "../Vendor/VoxBoxSDK/VoxBox-LlamaAPI/Source/API"

-- Binary libs relative to the root folder
LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["pjsip_debug"] = "../Vendor/pjsip/lib/Debug"
LibraryDir["pjsip_release"] = "../Vendor/pjsip/lib/Release"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"

-- PJSIP debug libs
Library["pjsip_libsrtp_debug"] = "%{LibraryDir.pjsip_debug}/libsrtp-x86_64-x64-vc14-Debug-Dynamic.lib"
Library["pjsip_libgsmcodec_debug"] = "%{LibraryDir.pjsip_debug}/libgsmcodec-x86_64-x64-vc14-Debug-Dynamic.lib"
Library["pjsip_libilbccodec_debug"] = "%{LibraryDir.pjsip_debug}/libilbccodec-x86_64-x64-vc14-Debug-Dynamic.lib"
Library["pjsip_libresample_debug"] = "%{LibraryDir.pjsip_debug}/libresample-x86_64-x64-vc14-Debug-Dynamic.lib"
Library["pjsip_libspeex_debug"] = "%{LibraryDir.pjsip_debug}/libspeex-x86_64-x64-vc14-Debug-Dynamic.lib"
Library["pjsip_pjlib_util_debug"] = "%{LibraryDir.pjsip_debug}/pjlib-util-x86_64-x64-vc14-Debug-Dynamic.lib"
Library["pjsip_pjlib_debug"] = "%{LibraryDir.pjsip_debug}/pjlib-x86_64-x64-vc14-Debug-Dynamic.lib"
Library["pjsip_pjmedia_audiodev_debug"] = "%{LibraryDir.pjsip_debug}/pjmedia-audiodev-x86_64-x64-vc14-Debug-Dynamic.lib"
Library["pjsip_pjmedia_codec_debug"] = "%{LibraryDir.pjsip_debug}/pjmedia-codec-x86_64-x64-vc14-Debug-Dynamic.lib"
Library["pjsip_pjmedia_debug"] = "%{LibraryDir.pjsip_debug}/pjmedia-x86_64-x64-vc14-Debug-Dynamic.lib"
Library["pjsip_pjnath_debug"] = "%{LibraryDir.pjsip_debug}/pjnath-x86_64-x64-vc14-Debug-Dynamic.lib"
Library["pjsip_core_debug"] = "%{LibraryDir.pjsip_debug}/pjsip-core-x86_64-x64-vc14-Debug-Dynamic.lib"
Library["pjsip_simple_debug"] = "%{LibraryDir.pjsip_debug}/pjsip-simple-x86_64-x64-vc14-Debug-Dynamic.lib"
Library["pjsip_ua_debug"] = "%{LibraryDir.pjsip_debug}/pjsip-ua-x86_64-x64-vc14-Debug-Dynamic.lib"
Library["pjsip_pjsua2_lib_debug"] = "%{LibraryDir.pjsip_debug}/pjsua2-lib-x86_64-x64-vc14-Debug-Dynamic.lib"
Library["pjsip_pjsua_lib_debug"] = "%{LibraryDir.pjsip_debug}/pjsua-lib-x86_64-x64-vc14-Debug-Dynamic.lib"

-- PJSIP release libs
Library["pjsip_libsrtp_release"] = "%{LibraryDir.pjsip_release}/libsrtp-x86_64-x64-vc14-Release-Dynamic.lib"
Library["pjsip_libgsmcodec_release"] = "%{LibraryDir.pjsip_release}/libgsmcodec-x86_64-x64-vc14-Release-Dynamic.lib"
Library["pjsip_libilbccodec_release"] = "%{LibraryDir.pjsip_release}/libilbccodec-x86_64-x64-vc14-Release-Dynamic.lib"
Library["pjsip_libresample_release"] = "%{LibraryDir.pjsip_release}/libresample-x86_64-x64-vc14-Release-Dynamic.lib"
Library["pjsip_libspeex_release"] = "%{LibraryDir.pjsip_release}/libspeex-x86_64-x64-vc14-Release-Dynamic.lib"
Library["pjsip_pjlib_util_release"] = "%{LibraryDir.pjsip_release}/pjlib-util-x86_64-x64-vc14-Release-Dynamic.lib"
Library["pjsip_pjlib_release"] = "%{LibraryDir.pjsip_release}/pjlib-x86_64-x64-vc14-Release-Dynamic.lib"
Library["pjsip_pjmedia_audiodev_release"] = "%{LibraryDir.pjsip_release}/pjmedia-audiodev-x86_64-x64-vc14-Release-Dynamic.lib"
Library["pjsip_pjmedia_codec_release"] = "%{LibraryDir.pjsip_release}/pjmedia-codec-x86_64-x64-vc14-Release-Dynamic.lib"
Library["pjsip_pjmedia_release"] = "%{LibraryDir.pjsip_release}/pjmedia-x86_64-x64-vc14-Release-Dynamic.lib"
Library["pjsip_pjnath_release"] = "%{LibraryDir.pjsip_release}/pjnath-x86_64-x64-vc14-Release-Dynamic.lib"
Library["pjsip_core_release"] = "%{LibraryDir.pjsip_release}/pjsip-core-x86_64-x64-vc14-Release-Dynamic.lib"
Library["pjsip_simple_release"] = "%{LibraryDir.pjsip_release}/pjsip-simple-x86_64-x64-vc14-Release-Dynamic.lib"
Library["pjsip_ua_release"] = "%{LibraryDir.pjsip_release}/pjsip-ua-x86_64-x64-vc14-Release-Dynamic.lib"
Library["pjsip_pjsua2_lib_release"] = "%{LibraryDir.pjsip_release}/pjsua2-lib-x86_64-x64-vc14-Release-Dynamic.lib"
Library["pjsip_pjsua_lib_release"] = "%{LibraryDir.pjsip_release}/pjsua-lib-x86_64-x64-vc14-Release-Dynamic.lib"



group "Dependencies"
   include "Vendor/imgui"
   include "Vendor/glfw"
   include "Vendor/yaml-cpp"
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