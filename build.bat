@echo off
setlocal
:: remedybg.exe stop-debugging
cls
::cd /D "%~dp0"

if not exist build mkdir build
pushd build

del *.pdb > NUL 2> NUL

:: set CommonCompilerFlags= /I..\src\ -MTd -nologo -fp:fast -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -DINTERNAL=1  -DWIN32=1 -FC -Z7
set CommonCompilerFlags= /I..\src\ /nologo /FC 
:: set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib Wininet.lib Wininet.lib
set CommonLinkerFlags= /INCREMENTAL:NO
set warn_disabled=-wd4201 -wd4505 -wd4100
cl /I..\src\ -Zi -W4  %warn_disabled% -nologo ..\src\web_parser\web_parser_main.c -Feweb_parser_dm.exe /link %CommonLinkerFlags%
:: cl %CommonCompilerFlags% /Zi ..\src\web_parser\web_parser_main.c -Fmwin32_web_parser_main.map /link %CommonLinkerFlags%   || exit /b 1

popd
