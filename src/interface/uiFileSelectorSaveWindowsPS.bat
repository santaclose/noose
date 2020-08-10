:: chooser.bat
:: launches a save file dialog and outputs choice to the console
@echo off
setlocal enabledelayedexpansion

:: Does powershell.exe exist within %PATH%?
for %%I in (powershell.exe) do if "%%~$PATH:I" neq "" (
    set chooser=powershell "Add-Type -AssemblyName System.windows.forms|Out-Null;$f=New-Object System.Windows.Forms.SaveFileDialog;$f.InitialDirectory='%cd%';$f.Filter='All Files (*.*)|*.*';$f.showHelp=$true;$f.ShowDialog()|Out-Null;$f.FileName"
) else (
rem :: If not, compose and link C# application to open file browser dialog
    set chooser=%temp%\chooser.exe
    >"%temp%\c.cs" echo using System;using System.Windows.Forms;
    >>"%temp%\c.cs" echo class dummy{
    >>"%temp%\c.cs" echo public static void Main^(^){
    >>"%temp%\c.cs" echo SaveFileDialog f=new SaveFileDialog^(^);
    >>"%temp%\c.cs" echo f.InitialDirectory=Environment.CurrentDirectory;
    >>"%temp%\c.cs" echo f.Filter="All Files (*.*)|*.*";
    >>"%temp%\c.cs" echo f.ShowHelp=true;
    >>"%temp%\c.cs" echo f.ShowDialog^(^);
    >>"%temp%\c.cs" echo Console.Write^(f.FileName^);}}
    for /f "delims=" %%I in ('dir /b /s "%windir%\microsoft.net\*csc.exe"') do (
        if not exist "!chooser!" "%%I" /nologo /out:"!chooser!" "%temp%\c.cs" 2>NUL
    )
    del "%temp%\c.cs"
    if not exist "!chooser!" (
        echo Error: Please install .NET 2.0 or newer, or install PowerShell.
        goto :EOF
    )
)

:: capture choice to a variable
for /f "delims=" %%I in ('%chooser%') do set "filename=%%I"

echo %filename%
pause

:: Clean up the mess
del "%temp%\chooser.exe" 2>NUL
goto :EOF