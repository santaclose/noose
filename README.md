# noose

node based image editor. 

![alt 
text](https://raw.githubusercontent.com/santaclose/noose/master/demo/demo.png)

### Dependencies:

SFML: https://www.sfml-dev.org/

nativefiledialog: https://github.com/mlabbe/nativefiledialog

### How to build for windows

- Clone the repository with: `git clone --recursive https://github.com/santaclose/noose.git`.
- Build SFML
- Use premake to generate the visual studio solution
- Build noose

To run the program, copy the assets folder to the executable path along with the necessary dlls:

- Release: `sfml-system-2.dll`, `sfml-window-2.dll`, `sfml-graphics-2.dll`
- Debug: `sfml-system-d-2.dll`, `sfml-window-d-2.dll`, `sfml-graphics-d-2.dll`