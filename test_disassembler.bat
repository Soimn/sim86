@echo off

nasm test_disassembler.asm
call build >null
build\disassemble.exe test_disassembler > test_disassembler_out.asm
nasm test_disassembler_out.asm
fc test_disassembler test_disassembler_out

REM del test_disassembler
REM del test_disassembler_out
REM del test_disassembler_out.asm
