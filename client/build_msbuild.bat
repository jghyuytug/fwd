@echo off
REM 简化构建脚本 - 直接使用MSBuild

echo ========================================
echo DNF Client - Build Script (MSBuild)
echo ========================================
echo.

REM 查找MSBuild
set MSBUILD_PATH=
for %%i in (
    "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
    "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
    "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
    "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"
    "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe"
) do (
    if exist %%i (
        set MSBUILD_PATH=%%i
        goto :found_msbuild
    )
)

echo [ERROR] MSBuild not found!
echo Please install Visual Studio 2019 or 2022 with C++ workload.
pause
exit /b 1

:found_msbuild
echo [INFO] Found MSBuild: %MSBUILD_PATH%
echo.

REM 检查DirectX SDK环境变量
if not defined DXSDK_DIR (
    echo [WARNING] DXSDK_DIR environment variable not set!
    echo Trying default path...
    set "DXSDK_DIR=C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\"
)

if not exist "%DXSDK_DIR%Include\d3d9.h" (
    echo [ERROR] DirectX SDK not found at: %DXSDK_DIR%
    echo Please install DirectX SDK (June 2010) or set DXSDK_DIR environment variable.
    pause
    exit /b 1
)

echo [INFO] DirectX SDK found: %DXSDK_DIR%
echo.

REM 创建输出目录
if not exist bin mkdir bin
if not exist bin\Debug mkdir bin\Debug
if not exist bin\Release mkdir bin\Release
if not exist obj mkdir obj

echo [1/2] Building Debug configuration...
"%MSBUILD_PATH%" DnfClient.sln /p:Configuration=Debug /p:Platform=Win32 /m
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Debug build failed!
    pause
    exit /b 1
)

echo.
echo [2/2] Building Release configuration...
"%MSBUILD_PATH%" DnfClient.sln /p:Configuration=Release /p:Platform=Win32 /m
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Release build failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Debug:   bin\Debug\DnfClient.exe
echo Release: bin\Release\DnfClient.exe
echo.
echo To run:
echo   bin\Debug\DnfClient.exe
echo.
echo To open in Visual Studio:
echo   DnfClient.sln
echo ========================================
echo.

pause
