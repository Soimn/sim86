@echo off

nasm test_execute.asm
call build >null
build\execute.exe test_execute > test_execute_out.txt
fc test_execute_out.txt test_execute.txt

REM del test_execute
REM del test_execute_out.txt
