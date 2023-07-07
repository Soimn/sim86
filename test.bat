@echo off
nasm test.asm
build\disassemble.exe test > test_out.asm
nasm test_out.asm
type test_out.asm
echo:
fc test test_out
