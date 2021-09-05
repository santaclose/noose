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
- Use premake to generate the visual studio solution
- Build noose

To run the program, copy the necessary dlls to the executable path:

- Release: `sfml-system-2.dll`, `sfml-window-2.dll`, `sfml-graphics-2.dll`
- Debug: `sfml-system-d-2.dll`, `sfml-window-d-2.dll`, `sfml-graphics-d-2.dll`