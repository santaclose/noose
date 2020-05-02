<# : chooser.bat
:: launches a File... Open sort of file chooser and outputs choice(s) to the console

@echo off
setlocal

for /f "delims=" %%I in ('powershell -noprofile "iex (${%~f0} | out-string)"') do (
    echo %%~I
)
goto :EOF

: end Batch portion / begin PowerShell hybrid chimera #>

Add-Type -AssemblyName System.Windows.Forms
$f = new-object Windows.Forms.OpenFileDialog
$f.InitialDirectory = pwd
$f.Filter = "Image Files(*.bmp;*.png;*.tga;*jpg;*gif;*psd;*hdr;*pic)|*.bmp;*.png;*.tga;*jpg;*gif;*psd;*hdr;*pic|All files (*.*)|*.*"
$f.ShowHelp = $true
$f.Multiselect = $false
[void]$f.ShowDialog()
if ($f.Multiselect) { $f.FileNames } else { $f.FileName }