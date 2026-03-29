project "ScamPA-Core"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"
   pchheader "spapch.h"
   pchsource "Source/spapch.cpp"
   
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

   targetdir ("bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")


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