
set BuildDir=./.build
set VCPKG_TOOLCHAIN_FILE="C:\WorkSpace\MyProgram\Repository\vcpkg\scripts\buildsystems\vcpkg.cmake"
cmake.exe -B %BuildDir% -S . -DCMAKE_TOOLCHAIN_FILE=%VCPKG_TOOLCHAIN_FILE%