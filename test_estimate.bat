@echo off

nasm %~dpn1.asm
call build >nul
build\estimate.exe %~dpn1 %2 > %~dpn1_out_%2.txt
fc %~dpn1_out_%2.txt %~dpn1_%2.txt
del %~dpn1
del %~dpn1_out_%2.txt
