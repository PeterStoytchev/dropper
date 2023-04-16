@echo off
mkdir build
cd build

set srcprefix=..\src\
set compiler_flags=/Zi /W4 /Od /D_CRT_SECURE_NO_WARNINGS /Qvec-report:1 /FAs
set include_dir=/I ..\include\
set files_to_compile=%srcprefix%main.c %srcprefix%psocket-win32.c %srcprefix%communication.c %srcprefix%pstring.c %srcprefix%utils.c
set executable_name=dropper-win32-debug.exe
set linker_libs=kernel32.lib user32.lib shell32.lib Ws2_32.lib Mswsock.lib AdvApi32.lib

cl %compiler_flags%  %include_dir%  %files_to_compile% /Fe%executable_name% /link %linker_libs%

cd ..