@echo off
set PATH=%PATH%;%CD%\openssl\
c:\php\php.exe -S localhost:80 -t .
