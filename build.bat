@echo off
setlocal enabledelayedexpansion
cd /D "%~dp0"

for %%a in (%*) do set "%%a=1"
if not "%release%"=="1" set debug=1
if "%debug%"=="1"       set release=0 && echo [debug mode]
if "%release%"=="1"     set debug=0 && echo [release mode]

set compile_common=-I..\src\ -g -std=c99 -pedantic -Wall -Wextra -Wno-unused-function -Wno-zero-length-array
set compile_debug=call clang -O0 %compile_common%
set compile_release=call clang -O2 %compile_common%
set link=
set out=-o

if "%debug%"=="1"   set compile=%compile_debug%
if "%release%"=="1" set compile=%compile_release%

if not exist build mkdir build

pushd build
%compile% ..\src\main.c %link% %out%excalibur.exe || exit /b 1
if "%run%"=="1" call excalibur.exe
popd
