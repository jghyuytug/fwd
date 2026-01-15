@echo off
REM 快速编译测试脚本
REM 用于验证df_game_r能否编译

echo ========================================
echo df_game_r 快速编译测试
echo ========================================
echo.

cd /d "%~dp0src_split"

echo [1/3] 测试编译环境...
where g++ >nul 2>&1
if %errorlevel% neq 0 (
    echo [!] 错误: 未找到g++编译器
    echo [*] 需要安装MinGW或使用Docker环境
    pause
    exit /b 1
)

echo [+] 编译器:
g++ --version | findstr "g++"
echo.

echo [2/3] 测试编译单个文件...
make df_game_r_part01.o 2>&1

if %errorlevel% neq 0 (
    echo.
    echo [!] 编译失败！查看错误信息
    pause
    exit /b 1
) else (
    echo.
    echo [+] 单文件编译成功！
    echo.
)

echo [3/3] 是否继续全量编译？ (这将需要15-30分钟)
echo 按任意键继续，或Ctrl+C取消...
pause >nul

echo.
echo [*] 开始全量编译...
make -j4 all 2>&1 | tee build.log

if %errorlevel% neq 0 (
    echo.
    echo [!] 编译过程中有错误，查看 build.log
) else (
    echo.
    echo [+] 编译完成！
    echo [+] 检查生成的文件...
    ls -lh df_game_r 2>nul || dir df_game_r
)

pause
