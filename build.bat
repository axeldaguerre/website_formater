@echo off
setlocal
cls

if not exist build mkdir build
pushd build


del *.pdb > NUL 2> NUL

set CommonCompilerFlags= /I..\src\ /nologo /FC 
::set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib Wininet.lib Wininet.lib
set CommonLinkerFlags= /INCREMENTAL:NO
set warn_disabled=-wd4201 -wd4505 -wd4100
cl /I..\src\ -Zi -W4  %warn_disabled% -nologo ..\src\web_parser\web_parser_main.c -Feweb_parser_dm.exe /link %CommonLinkerFlags%

popd
