@echo off

nasm %~dpn1.asm
call build >nul
build\execute.exe %~dpn1 > %~dpn1_out.txt
fc %~dpn1_out.txt %~dpn1.txt
del %~dpn1
del %~dpn1_out.txt
