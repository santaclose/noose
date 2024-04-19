# noose

node based image editor. 

![alt 
text](https://raw.githubusercontent.com/santaclose/noose/master/demo/demo.png)

### Dependencies:

SFML: https://www.sfml-dev.org/

portable-file-dialogs: https://github.com/samhocevar/portable-file-dialogs

clip: https://github.com/dacap/clip

### How to build for windows

- Clone the repository with: `git clone --recursive https://github.com/santaclose/noose.git`.
	+ If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.
- Build SFML
	+ `cmake -DSFML_USE_STATIC_STD_LIBS:BOOL=TRUE .`
	+ Or providing generator and platform: `cmake -G "Visual Studio 17 2022" -A x64 -DSFML_USE_STATIC_STD_LIBS:BOOL=TRUE .`
- Use premake to generate the visual studio solution: `premake5 vs2019`
- Open the sln file and build noose