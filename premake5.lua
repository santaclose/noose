workspace "noose"
	architecture "x64"
	startproject "noose"

	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "noose"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/clip/**.h",
		"vendor/clip/clip.cpp",
		"vendor/clip/clip_win.cpp",
		"vendor/clip/image.cpp",
		"vendor/base64/base64.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
		--"SFML_STATIC"
	}

	includedirs
	{
		"src",
		"vendor/SFML/include",
		"vendor/portable-file-dialogs",
		"vendor/clip",
		"vendor/base64",
		"vendor/cpp-subprocess"
	}

	filter "system:Windows"
		system "windows"
		systemversion "latest"

		files
		{
		}

		defines
		{
			"NOOSE_PLATFORM_WINDOWS"
		}

	filter "system:Unix"
		system "linux"
		systemversion "latest"

		files
		{
		}

		defines
		{
			"NOOSE_PLATFORM_LINUX"
		}

	filter "system:Mac"
		system "macosx"
		systemversion "latest"

		files
		{
		}

		defines
		{
			"NOOSE_PLATFORM_MACOS"
		}

	filter "configurations:Debug"
		defines "NOOSE_DEBUG"
		runtime "Debug"
		symbols "on"

		libdirs
		{
			"vendor/SFML/lib/Debug"
		}
		links
		{
			"sfml-graphics-d",
			"sfml-window-d",
			"sfml-system-d",
			"shlwapi"
		}

	filter "configurations:Release"
		defines "NOOSE_RELEASE"
		runtime "Release"
		optimize "on"

		libdirs
		{
			"vendor/SFML/lib/Release"
		}
		links
		{
			"sfml-graphics",
			"sfml-window",
			"sfml-system",
			"shlwapi"
		}