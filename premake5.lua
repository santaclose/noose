workspace "Workspace"
   configurations { "Debug", "Release" }
   location "build"

project "noose"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   files { "noose/**.*" }

   filter "configurations:*"
      defines { "SFML_STATIC", "LINUX" }
      includedirs { "dependencies/SFML/include" }
      libdirs { "dependencies/SFML/lib" }
      links
      {
         "opengl32",
         "freetype",
         "winmm",
         "gdi32",
         "flac",
         "vorbisenc",
         "vorbisfile",
         "vorbis",
         "ogg",
         "ws2_32"
      }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      links
      {   
         "sfml-graphics-s-d",
         "sfml-window-s-d",
         "sfml-system-s-d",
      }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      links
      {   
         "sfml-graphics-s",
         "sfml-window-s",
         "sfml-system-s",
      }