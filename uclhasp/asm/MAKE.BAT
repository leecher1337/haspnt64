tasm32 /m/ml /Isrc\ src\*.asm
rem Needed for stupid new Visual Studio releases that don't understand OMF format..??
call "%ProgramFiles%\Microsoft Visual Studio\VC98\bin\vcvars32.bat"
editbin *.obj

