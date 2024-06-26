workspace "ManiTests"
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    configurations { "Debug" }
    startproject "SampleProject"
    architecture "x64"
    language "C++"
    cppdialect "C++20"
    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)
    symbols "On"

    includedirs { "include/" }

project "SampleProject"
    kind "ConsoleApp"
    location "%{prj.name}"

    files { "%{prj.name}/**.h", "%{prj.name}/**.cpp" }
