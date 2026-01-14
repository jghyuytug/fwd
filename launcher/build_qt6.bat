@echo off
chcp 65001 >nul
title DNF登录器 - Qt 6编译脚本
color 0A

echo ========================================
echo      DNF Qt 6登录器 - 编译脚本
echo ========================================
echo.

REM 设置Qt 6环境变量
set "QT_DIR=C:\Qt\6.6.0\mingw_64"
set "MINGW_DIR=C:\Qt\Tools\mingw1310_64"
set "CMAKE_DIR=C:\Qt\Tools\CMake_64"
set "PATH=%QT_DIR%\bin;%MINGW_DIR%\bin;%CMAKE_DIR%\bin;%PATH%"

echo [1/5] 检查Qt 6环境...
qmake --version
if %errorlevel% neq 0 (
    echo ❌ Qt 6未正确配置！
    pause
    exit /b 1
)
echo ✅ Qt 6环境正常
echo.

echo [2/5] 检查MinGW编译器...
gcc --version | findstr "13.1.0"
if %errorlevel% neq 0 (
    echo ❌ MinGW编译器未找到！
    pause
    exit /b 1
)
echo ✅ MinGW 13.1.0正常
echo.

echo [3/5] 清理旧构建...
if exist build_qt6 (
    rd /s /q build_qt6
)
mkdir build_qt6
cd build_qt6
echo ✅ 构建目录已创建
echo.

echo [4/5] 配置CMake项目...
cmake .. -G "MinGW Makefiles" ^
    -DCMAKE_PREFIX_PATH=%QT_DIR% ^
    -DCMAKE_BUILD_TYPE=Release

if %errorlevel% neq 0 (
    echo ❌ CMake配置失败！
    cd ..
    pause
    exit /b 1
)
echo ✅ CMake配置成功
echo.

echo [5/5] 开始编译...
cmake --build . --config Release

if %errorlevel% neq 0 (
    echo ❌ 编译失败！
    cd ..
    pause
    exit /b 1
)
echo ✅ 编译成功！
echo.

echo ========================================
echo             编译完成
echo ========================================
echo.
echo 可执行文件位置: build_qt6\DnfLogin.exe
echo.
echo 提示:
echo - Qt 6版本: 6.6.0
echo - 编译器: MinGW 13.1.0
echo - C++标准: C++17
echo.

cd ..
pause
