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

reg query "HKLM\SYSTEM\CurrentControlSet\Services\hardlock" >nul 2>nul
if errorlevel 1 (
  echo hardlock driver has not been found on your system. This is required for
  echo this driver to work.
  echo Please download and install Hardlock and try again
  echo.
  echo Important: install with
  echo.
  echo    haspdinst -i -ld
  echo.
  pause
  start https://supportportal.thalesgroup.com/csm?sys_kb_id=979a4e21db92e78cfe0aff3dbf9619c6^&id=kb_article_view^&sysparm_rank=1^&sysparm_tsqueryId=7efc79bddb8e81105d310573f3961942^&sysparm_article=KB0018319
  goto fini
)

find "BEGIN" haspnt64.cer >nul
if errorlevel 1 (
  reg query "HKLM\SYSTEM\CurrentControlSet\Control" /v SystemStartOptions | find /i "TESTSIGN"
  if errorlevel 1 (
    echo Test signing mode not enabled, please enable test signing mode first.
    echo On systems without secure boot, this can be done with 
    echo.
    echo    bcdedit /set testsigning on
    echo.
    echo Do you want to try this now? On Windows with secure boot, this may fail.
    echo If it fails, boot to boot menu options and disable driver signature enforcement
    echo there.
    echo See: https://www.thewindowsclub.com/disable-driver-signature-enforcement-windows
    echo.
    echo.
    CHOICE /C YN /M "Try to enable test singing mode?"
    if errorlevel 2 goto fini
    bcdedit /set testsigning on
    if errorlevel 1 (
      echo It seems that it failed, please try to manually enable test signing mode
      pause
      start https://www.thewindowsclub.com/disable-driver-signature-enforcement-windows
      goto fini
    )
    echo You need to reboot now to enable test signing mode and then run the
    echo installer again.
    pause
    goto fini
  )
)

certutil -addstore "TrustedPublisher" haspnt64.cer
rundll32 setupapi,InstallHinfSection DefaultInstall 132 .\haspnt64.inf
if exist %systemroot%\sysWOW64\config.nt (
  type %systemroot%\sysWOW64\config.nt | find /i "haspdos.sys"
  if errorlevel 1 echo device=%SystemRoot%\system32\haspdos.sys >>%systemroot%\sysWOW64\config.nt
)
net start haspnt
pause
:fini