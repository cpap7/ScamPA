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
	  "%{IncludeDir.spdlog}"
   }

   links
   {
       "ImGui",
       "GLFW",

       "%{Library.Vulkan}",
   }

   targetdir ("bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
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