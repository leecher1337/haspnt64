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

type %systemroot%\sysWOW64\config.nt | find /i "haspdos.sys"
if not errorlevel 1 (
  find /V "haspdos.sys" %systemroot%\sysWOW64\config.nt >%TEMP%\config.nt
  move /y %TEMP%\config.nt %systemroot%\sysWOW64\config.nt
)
rundll32 setupapi,InstallHinfSection DefaultUninstall 132 .\haspnt64.inf
