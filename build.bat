@echo off
cls
rem --- Unpack Arguments ------------------------------------------------------
for %%a in (%*) do set "%%a=1"

rem --- Settings --------------------------------------------------------------
set compiler=cl
set cl_flags=/Zi /nologo /FC /I..\code\ /I..\local\
set gfx=/DOS_FEATURE_GFX=1
set net=/DOS_FEATURE_NET=1
set console=/DBUILD_CONSOLE_OUTPUT=1
set telemetry=/DBUILD_TELEMETRY=1

rem --- Prep Build Directory --------------------------------------------------
if not exist build mkdir build
xcopy /y /q /s /e /i data .\build\data

rem --- Prep Local Directory --------------------------------------------------
if not exist local mkdir local

rem --- Build & Run Metaprogram -----------------------------------------------
@REM pushd build
@REM %compiler% %cl_flags% %console% ..\src\metagen\metagen_main.c /link /out:metagen.exe
@REM metagen.exe
@REM popd

rem --- Build Everything ------------------------------------------------------
pushd build
if "%web_formater%"=="1" %compiler% %cl_flags%   ..\code\web_formater\web_formater_main.c  /link /out:web_formater_main.exe
popd

rem --- Unset Build Flags -----------------------------------------------------
for %%a in (%*) do set "%%a=0"
