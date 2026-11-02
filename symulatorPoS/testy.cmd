@echo off
setlocal
pushd "%~dp0"
if not exist build mkdir build
if errorlevel 1 goto fail
set "POS_CORE=Walidator.cpp Pliki.cpp Symulator.cpp Statystyki.cpp Przebiegi.cpp"
for %%T in (tests\test_*.cpp) do (
    echo Test: %%~nT
    g++ -std=c++17 -O2 -Wall -Wextra -pedantic -UNDEBUG -I. "%%T" %POS_CORE% -o "build\%%~nT.exe"
    if errorlevel 1 goto fail
    "build\%%~nT.exe"
    if errorlevel 1 goto fail
    echo OK: %%~nT
)
echo Wszystkie testy C++ przeszly.
popd
exit /b 0
:fail
echo Blad kompilacji lub testu C++.
popd
exit /b 1
