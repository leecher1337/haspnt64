@echo off
tasm %1.asm
tlink %1
exe2bin %1 %1.sys
del *.map 
del *.obj 
del *.exe
