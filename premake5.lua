-- premake5.lua
workspace "ScamPA"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "ScamPA-App"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "ScamPAExternal.lua"
include "ScamPA-App"