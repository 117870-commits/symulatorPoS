@echo off
setlocal
pushd "%~dp0"
set "POS_CORE=Walidator.cpp Pliki.cpp Symulator.cpp Statystyki.cpp Przebiegi.cpp"
set "POS_FLAGS=-std=c++17 -O2 -Wall -Wextra -pedantic -static"
g++ %POS_FLAGS% main.cpp %POS_CORE% -o symulator.exe -lshell32
if errorlevel 1 goto fail
if not exist build mkdir build
if errorlevel 1 goto fail
windres gui.rc -o build\gui_resource.o
if errorlevel 1 goto fail
g++ %POS_FLAGS% -municode -mwindows Gui.cpp %POS_CORE% build\gui_resource.o -o symulator_gui.exe -lcomctl32 -lcomdlg32 -lgdi32 -lshell32
if errorlevel 1 goto fail
g++ %POS_FLAGS% Eksperymenty.cpp %POS_CORE% -o eksperymenty.exe
if errorlevel 1 goto fail
if /i not "%~1"=="--tests" goto done
call testy.cmd
if errorlevel 1 goto fail
:done
popd
exit /b 0
:fail
echo Build or verification failed.
popd
exit /b 1
