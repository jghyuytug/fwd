@echo off
REM 构建脚本 - 生成Visual Studio项目并编译

echo ========================================
echo DNF Client - Build Script
echo ========================================
echo.

REM 检查CMake是否安装
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake not found! Please install CMake first.
    echo Download: https://cmake.org/download/
    pause
    exit /b 1
)

REM 创建build目录
if not exist build mkdir build
cd build

echo [1/3] Generating Visual Studio project...
cmake .. -G "Visual Studio 17 2022" -A Win32
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake generation failed!
    cd ..
    pause
    exit /b 1
)

echo.
echo [2/3] Building project (Debug)...
cmake --build . --config Debug
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed!
    cd ..
    pause
    exit /b 1
)

echo.
echo [3/3] Build completed successfully!
echo.
echo Output: build\bin\Debug\DnfClient.exe
echo.
echo ========================================
echo To run the program:
echo   cd build\bin\Debug
echo   DnfClient.exe
echo ========================================
echo.

cd ..
pause
