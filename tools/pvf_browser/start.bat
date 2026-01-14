@echo off
REM DNF装备浏览器启动脚本 (Windows)

echo ========================================
echo DNF装备浏览器
echo ========================================
echo.

REM 检查后端是否已编译
if not exist "backend\pvf_api.exe" (
    echo [错误] 找不到backend\pvf_api.exe
    echo 请先编译后端:
    echo   cd backend
    echo   make
    echo.
    pause
    exit /b 1
)

REM 检查Python
python --version >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 找不到Python
    echo 请先安装Python 3.6+
    echo.
    pause
    exit /b 1
)

echo [启动] 正在启动Web服务器...
echo.

REM 启动服务器
python server.py

pause
