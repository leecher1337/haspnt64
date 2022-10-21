@echo off

:StartExec
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
if '%errorlevel%' NEQ '0' (
  echo Requesting administrative privileges...
  goto UACPrompt
) else ( goto gotAdmin )
:UACPrompt
echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
echo UAC.ShellExecute "%~s0", "", "", "runas", 1 >> "%temp%\getadmin.vbs"
"%temp%\getadmin.vbs"
exit /B
:gotAdmin
if exist "%temp%\getadmin.vbs" ( del "%temp%\getadmin.vbs" )
pushd "%CD%"
CD /D "%~dp0"

rundll32 setupapi,InstallHinfSection DefaultInstall 132 .\uclhasp.inf
if exist %systemroot%\sysWOW64\config.nt (
  type %systemroot%\sysWOW64\config.nt | find /i "haspdos.sys"
  if errorlevel 1 echo device=%SystemRoot%\system32\haspdos.sys >>%systemroot%\sysWOW64\config.nt
)
pause
:fini