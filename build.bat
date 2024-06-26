@echo off
setlocal

if not exist build mkdir build
cd build


cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release -- /verbosity:minimal
cd ..

endlocal
echo Build Completed.
