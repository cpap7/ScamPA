project "ScamPA-App"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   files { "Source/**.h", "Source/**.cpp" }

   includedirs
   {
      "../ScamPA-Core/Source",

      "%{IncludeDir.VulkanSDK}",
      "%{IncludeDir.glm}",
	  "%{IncludeDir.glfw}",
      "%{IncludeDir.imgui}",
      "%{IncludeDir.stb_image}",
	  "%{IncludeDir.spdlog}",
      "%{IncludeDir.miniaudio}",

      -- VoxBoxSDK
      "%{IncludeDir.VoxBoxCommon}",
      "%{IncludeDir.VoxBoxSTT}",
      "%{IncludeDir.VoxBoxTTS}",
      "%{IncludeDir.VoxBoxLLM}",
   }

   links
   {
       "ScamPA-Core"
   }

   targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
   local voxbox_root   = "../Vendor/VoxBoxSDK"
   local voxbox_bindir = "/bin/%{cfg.buildcfg}-%{cfg.architecture}"

   local whisper_bin = voxbox_root .. "/VoxBox-WhisperAPI" .. voxbox_bindir
   local piper_bin   = voxbox_root .. "/VoxBox-PiperAPI"   .. voxbox_bindir
   local llama_bin   = voxbox_root .. "/VoxBox-LlamaAPI"   .. voxbox_bindir

   postbuildcommands
   {
      -- VoxBox-WhisperAPI
      '{COPYFILE} "' .. whisper_bin .. '/VoxBox-WhisperAPI.dll" "%{cfg.targetdir}"',

      -- VoxBox-PiperAPI + dependencies
      '{COPYFILE} "' .. piper_bin .. '/VoxBox-PiperAPI.dll" "%{cfg.targetdir}"',
      '{COPYFILE} "' .. piper_bin .. '/onnxruntime.dll" "%{cfg.targetdir}"',
      '{COPYFILE} "' .. piper_bin .. '/espeak-ng.dll" "%{cfg.targetdir}"',
      '{COPYFILE} "' .. piper_bin .. '/piper_phonemize.dll" "%{cfg.targetdir}"',
      '{COPYDIR} "'  .. piper_bin .. '/espeak" "%{cfg.targetdir}"',

      -- VoxBox-LlamaAPI
      '{COPYFILE} "' .. llama_bin .. '/VoxBox-LlamaAPI.dll" "%{cfg.targetdir}"',
   }

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
      kind "WindowedApp"
      defines { "SPA_DIST" }
      buildoptions { "/utf-8" }
      runtime "Release"
      optimize "On"
      symbols "Off"