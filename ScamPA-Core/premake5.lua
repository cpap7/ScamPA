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

   postbuildcommands
   {
      -- VoxBox-WhisperAPI
      '{COPYFILE} "' .. whisper_bin .. '/VoxBox-WhisperAPI.dll" "bin/%{cfg.targetdir}"',

      -- VoxBox-PiperAPI + dependencies
      '{COPYFILE} "' .. piper_bin .. '/VoxBox-PiperAPI.dll" "bin/%{cfg.targetdir}"',
      '{COPYFILE} "' .. piper_bin .. '/onnxruntime.dll" "bin/%{cfg.targetdir}"',
      '{COPYFILE} "' .. piper_bin .. '/espeak-ng.dll" "bin/%{cfg.targetdir}"',
      '{COPYFILE} "' .. piper_bin .. '/piper_phonemize.dll" "%bin/{cfg.targetdir}"',
      '{COPYDIR} "'  .. piper_bin .. '/espeak" "bin/%{cfg.targetdir}"',

      -- VoxBox-LlamaAPI
      '{COPYFILE} "' .. llama_bin .. '/VoxBox-LlamaAPI.dll" "bin/%{cfg.targetdir}"',
   }

   


   filter "files:Vendor/yaml-cpp/src/**.cpp"
	  flags { "NoPCH" }
   
   filter "system:windows"
      systemversion "latest"
      toolset "v145" -- VS 2026
      defines { "SPA_PLATFORM_WINDOWS" }

   filter "configurations:Debug"
      defines { "SPA_DEBUG" }
      buildoptions { "/utf-8" }
      runtime "Debug"
      symbols "On"

   filter "configurations:Release"
      defines { "SPA_RELEASE" }
      buildoptions { "/utf-8" }
      runtime "Release"
      optimize "On"
      symbols "On"

   filter "configurations:Dist"
      defines { "SPA_DIST" }
      buildoptions { "/utf-8" }
      runtime "Release"
      optimize "On"
      symbols "Off"