rem Requires ResourceHacker directory in PATH

set /P version=Enter version: 

if not exist "%~dp0release" mkdir "%~dp0release"
if not exist "%~dp0release/assets" mkdir "%~dp0release/assets"

move /y "%~dp0bin\Release-windows-x86_64\noose\noose.exe" "%~dp0release\noose.exe"
move /y "%~dp0assets\fonts" "%~dp0release\assets\fonts"
move /y "%~dp0assets\images" "%~dp0release\assets\images"
move /y "%~dp0assets\autoUpdate" "%~dp0release\assets\autoUpdate"
move /y "%~dp0assets\nodeShaders" "%~dp0release\assets\nodeShaders"
move /y "%~dp0assets\shaders" "%~dp0release\assets\shaders"
move /y "%~dp0assets\icon.png" "%~dp0release\assets\icon.png"
move /y "%~dp0assets\nodes.dat" "%~dp0release\assets\nodes.dat"

echo|set /p=%version% > "%~dp0release\version.dat"

ResourceHacker -open "%~dp0release\noose.exe" -save "%~dp0release\noosei.exe" -action addskip -res "%~dp0assets\noose.ico" -mask ICONGROUP,MAINICON,
move /y "%~dp0release\noose.exe" "%~dp0bin\Release-windows-x86_64\noose\noose.exe"
move /y "%~dp0release\noosei.exe" "%~dp0release\noose.exe"

pushd release
powershell -Command "Compress-Archive -Path .\*" -DestinationPath "noose_windows_x64.zip"
popd
if exist "%~dp0noose_windows_x64.zip" del "%~dp0noose_windows_x64.zip"
move /y "%~dp0release\noose_windows_x64.zip" "%~dp0noose_windows_x64.zip"

move /y "%~dp0release\assets\fonts" "%~dp0assets\fonts"
move /y "%~dp0release\assets\images" "%~dp0assets\images"
move /y "%~dp0release\assets\autoUpdate" "%~dp0assets\autoUpdate"
move /y "%~dp0release\assets\nodeShaders" "%~dp0assets\nodeShaders"
move /y "%~dp0release\assets\shaders" "%~dp0assets\shaders"
move /y "%~dp0release\assets\icon.png" "%~dp0assets\icon.png"
move /y "%~dp0release\assets\nodes.dat" "%~dp0assets\nodes.dat"

del "%~dp0release\noose.exe"
del "%~dp0release\version.dat"
rmdir /q "%~dp0release\assets"
rmdir /q "%~dp0release"

pause