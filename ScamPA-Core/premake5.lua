project "ScamPA-Core"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"
   pchheader "spapch.h"
   pchsource "Source/spapch.cpp"
   targetdir ("bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

   files 
   { 
        "Source/**.h", 
        "Source/**.cpp"	
   }

   includedirs
   {
        "Source",
        "%{IncludeDir.VulkanSDK}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.spdlog}",
		"%{IncludeDir.miniaudio}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.json}",
		"%{IncludeDir.pjsip}",

        -- VoxBoxSDK
        "%{IncludeDir.VoxBoxCommon}",
        "%{IncludeDir.VoxBoxSTT}",
        "%{IncludeDir.VoxBoxTTS}",
        "%{IncludeDir.VoxBoxLLM}",
   }

   links
   {
        "ImGui",
        "GLFW",
		"yaml-cpp",

        "%{Library.Vulkan}",

        -- VoxBoxSDK
        "VoxBox-WhisperAPI",
        "VoxBox-PiperAPI",
        "VoxBox-LlamaAPI",
   }

   local whisper_bin = "../Vendor/VoxBoxSDK/VoxBox-WhisperAPI/bin/%{cfg.buildcfg}-%{cfg.architecture}"
   local piper_bin   = "../Vendor/VoxBoxSDK/VoxBox-PiperAPI/bin/%{cfg.buildcfg}-%{cfg.architecture}"
   local llama_bin   = "../Vendor/VoxBoxSDK/VoxBox-LlamaAPI/bin/%{cfg.buildcfg}-%{cfg.architecture}"
   local app_outdir  = "../bin/" .. outputdir .. "/ScamPA-App"

   postbuildcommands
   {
      -- VoxBox-WhisperAPI
      '{COPYFILE} "' .. whisper_bin .. '/VoxBox-WhisperAPI.dll" "' .. app_outdir .. '"',

      -- VoxBox-PiperAPI + dependencies
      '{COPYFILE} "' .. piper_bin .. '/VoxBox-PiperAPI.dll" "' .. app_outdir .. '"',
      '{COPYFILE} "' .. piper_bin .. '/onnxruntime.dll" "' .. app_outdir .. '"',
      '{COPYFILE} "' .. piper_bin .. '/espeak-ng.dll" "' .. app_outdir .. '"',
      '{COPYFILE} "' .. piper_bin .. '/piper_phonemize.dll" "' .. app_outdir .. '"',
      '{COPYDIR} "'  .. piper_bin .. '/espeak" "' .. app_outdir .. '"',

      -- VoxBox-LlamaAPI
      '{COPYFILE} "' .. llama_bin .. '/VoxBox-LlamaAPI.dll" "' .. app_outdir .. '"',
   }

   filter "files:Vendor/yaml-cpp/src/**.cpp"
	  flags { "NoPCH" }
   
   filter "system:windows"
      systemversion "latest"
      toolset "v145" -- VS 2026
      defines { "SPA_PLATFORM_WINDOWS" }
      links { "ws2_32", "ole32", "winmm" }

   filter "configurations:Debug"
      defines { "SPA_DEBUG" }
      buildoptions { "/utf-8" }
      runtime "Debug"
      symbols "On"
      links
      {
         "%{Library.pjsip_pjsua2_lib_debug}",
         "%{Library.pjsip_pjsua_lib_debug}",
         "%{Library.pjsip_ua_debug}",
         "%{Library.pjsip_simple_debug}",
         "%{Library.pjsip_core_debug}",
         "%{Library.pjsip_pjmedia_codec_debug}",
         "%{Library.pjsip_pjmedia_audiodev_debug}",
         "%{Library.pjsip_pjmedia_debug}",
         "%{Library.pjsip_pjnath_debug}",
         "%{Library.pjsip_pjlib_util_debug}",
         "%{Library.pjsip_pjlib_debug}",
         "%{Library.pjsip_libsrtp_debug}",
         "%{Library.pjsip_libgsmcodec_debug}",
         "%{Library.pjsip_libilbccodec_debug}",
         "%{Library.pjsip_libresample_debug}",
         "%{Library.pjsip_libspeex_debug}",

      }

   filter "configurations:Release"
      defines { "SPA_RELEASE" }
      buildoptions { "/utf-8" }
      runtime "Release"
      optimize "On"
      symbols "On"
      links
      {
         "%{Library.pjsip_pjsua2_lib_release}",
         "%{Library.pjsip_pjsua_lib_release}",
         "%{Library.pjsip_ua_release}",
         "%{Library.pjsip_simple_release}",
         "%{Library.pjsip_core_release}",
         "%{Library.pjsip_pjmedia_codec_release}",
         "%{Library.pjsip_pjmedia_audiodev_release}",
         "%{Library.pjsip_pjmedia_release}",
         "%{Library.pjsip_pjnath_release}",
         "%{Library.pjsip_pjlib_util_release}",
         "%{Library.pjsip_pjlib_release}",
         "%{Library.pjsip_libsrtp_release}",
         "%{Library.pjsip_libgsmcodec_release}",
         "%{Library.pjsip_libilbccodec_release}",
         "%{Library.pjsip_libresample_release}",
         "%{Library.pjsip_libspeex_release}",
      }

   filter "configurations:Dist"
      defines { "SPA_DIST" }
      buildoptions { "/utf-8" }
      runtime "Release"
      optimize "On"
      symbols "Off"
      links
      {
         "%{Library.pjsip_pjsua2_lib_release}",
         "%{Library.pjsip_pjsua_lib_release}",
         "%{Library.pjsip_ua_release}",
         "%{Library.pjsip_simple_release}",
         "%{Library.pjsip_core_release}",
         "%{Library.pjsip_pjmedia_codec_release}",
         "%{Library.pjsip_pjmedia_audiodev_release}",
         "%{Library.pjsip_pjmedia_release}",
         "%{Library.pjsip_pjnath_release}",
         "%{Library.pjsip_pjlib_util_release}",
         "%{Library.pjsip_pjlib_release}",
         "%{Library.pjsip_libsrtp_release}",
      }