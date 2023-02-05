# noose

node based image editor. 

![alt 
text](https://raw.githubusercontent.com/santaclose/noose/master/demo/demo.png)

### Dependencies:

SFML: https://www.sfml-dev.org/

portable-file-dialogs: https://github.com/samhocevar/portable-file-dialogs

### How to build for windows

- Clone the repository with: `git clone --recursive https://github.com/santaclose/noose.git`.
	+ If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.
- Build SFML
	+ If you have issues with cmake, try to provide generator and platform, e.g: `cmake -G "Visual Studio 16 2019" -A x64`
- Use premake to generate the visual studio solution: `premake5 vs2019`
- Open the sln file and build noose

To run the program, copy the necessary dlls to the executable path:

- Release:
`sfml-system-3.dll`, `sfml-window-3.dll`, `sfml-graphics-3.dll` from `vendor/SFML/bin/Release/` to `bin/Release-windows-x86_64/noose/`
- Debug:
`sfml-system-d-3.dll`, `sfml-window-d-3.dll`, `sfml-graphics-d-3.dll` from `vendor/SFML/bin/Debug/` to `bin/Debug-windows-x86_64/noose/`