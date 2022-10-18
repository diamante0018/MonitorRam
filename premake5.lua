workspace "MonitorRam"
startproject "MonitorRam"
location "./build"
objdir "%{wks.location}/obj"
targetdir "%{wks.location}/bin/%{cfg.platform}/%{cfg.buildcfg}"

configurations {"Debug", "Release"}

platforms {"x86", "x64"}

filter "platforms:x86"
architecture "x86"
filter {}

filter "platforms:x64"
architecture "x86_64"
filter {}

buildoptions {"-std=gnu11"}

symbols "On"
staticruntime "On"
editandcontinue "Off"
warnings "Extra"
characterset "ASCII"

if os.istarget("linux") then
	buildoptions {"-pthread"}
	linkoptions {"-pthread"}
end

if os.getenv("CI") then
	defines {"CI"}
end

flags {"NoIncrementalLink", "NoMinimalRebuild", "MultiProcessorCompile", "No64BitChecks"}

filter "configurations:Release"
	optimize "Speed"
	defines {"NDEBUG"}
	flags {"FatalCompileWarnings"}
filter {}

filter "configurations:Debug"
	optimize "Debug"
	defines {"DEBUG", "_DEBUG"}
filter {}

project "MonitorRam"
kind "ConsoleApp"
language "C"

targetname "MonitorRam"

files {"./src/**.h", "./src/**.c"}

includedirs {"./src", "%{prj.location}/src"}

resincludedirs {"%{_MAIN_SCRIPT_DIR}/src"}
