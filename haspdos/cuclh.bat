@echo off
tasm /m2 /i.\uclhasp /i..\uclhasp\asm\src uclhasp\*.ASM
tasm /i.\uclhasp /i..\uclhasp\asm\src uclhasp*.asm
tlink /3 uclhasph.obj overall.obj haspmem.obj fn-emu.obj decrypt.obj uclhaspf.obj, uclhasp.exe
exe2bin uclhasp.exe uclhasp.sys
del *.obj
del *.exe 
del *.map
