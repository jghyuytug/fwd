@echo off
REM DNF GM Tools - 启动MySQL数据库
REM 此脚本启动Docker MySQL容器并等待初始化完成

echo ========================================
echo DNF GM Tools - 启动MySQL数据库
echo ========================================
echo.

REM 检查Docker是否运行
echo [1/5] 检查Docker状态...
docker info >nul 2>&1
if errorlevel 1 (
    echo [错误] Docker Desktop未运行！
    echo.
    echo 请执行以下步骤:
    echo 1. 在Windows开始菜单中找到 "Docker Desktop"
    echo 2. 启动Docker Desktop
    echo 3. 等待Docker Desktop完全启动（托盘图标不再旋转）
    echo 4. 重新运行此脚本
    echo.
    pause
    exit /b 1
)
echo [OK] Docker正在运行
echo.

REM 切换到docker目录
cd /d "%~dp0docker"

REM 检查MySQL容器是否已经运行
echo [2/5] 检查MySQL容器状态...
docker ps --filter "name=dnf-gm-mirror-db" | findstr "dnf-gm-mirror-db" >nul 2>&1
if not errorlevel 1 (
    echo [OK] MySQL容器已经在运行
    goto :verify
)

REM 检查容器是否存在但未运行
docker ps -a --filter "name=dnf-gm-mirror-db" | findstr "dnf-gm-mirror-db" >nul 2>&1
if not errorlevel 1 (
    echo [INFO] MySQL容器存在但未运行，正在启动...
    docker start dnf-gm-mirror-db
    if errorlevel 1 (
        echo [错误] 启动失败！
        pause
        exit /b 1
    )
    goto :wait
)

REM 创建并启动新容器
echo [INFO] MySQL容器不存在，正在创建...
docker-compose up -d gm-mirror-db
if errorlevel 1 (
    echo [错误] 创建容器失败！
    echo.
    echo 常见问题:
    echo - 端口3307已被占用
    echo - Docker配置文件错误
    echo - 磁盘空间不足
    echo.
    pause
    exit /b 1
)

:wait
echo [OK] 容器已启动
echo.

echo [3/5] 等待MySQL初始化 (这可能需要30-60秒)...
timeout /t 5 /nobreak >nul

REM 等待MySQL健康检查通过
set RETRY=0
:check_health
set /a RETRY+=1
if %RETRY% GTR 30 (
    echo [错误] MySQL初始化超时！
    echo 请检查容器日志: docker logs dnf-gm-mirror-db
    pause
    exit /b 1
)

docker inspect dnf-gm-mirror-db --format="{{.State.Health.Status}}" 2>nul | findstr "healthy" >nul 2>&1
if errorlevel 1 (
    echo [等待] 初始化中... (尝试 %RETRY%/30)
    timeout /t 2 /nobreak >nul
    goto :check_health
)

:verify
echo [OK] MySQL已就绪
echo.

echo [4/5] 验证数据库连接...
cd /d "%~dp0backend"
node scripts\test-connect.js
if errorlevel 1 (
    echo [警告] 数据库连接测试失败
    echo 容器可能还在初始化中，请稍后重试
) else (
    echo [OK] 数据库连接成功
)
echo.

echo [5/5] 检查数据库表...
node scripts\check-db-pool.js
if errorlevel 1 (
    echo [警告] 无法检查数据库表
) else (
    echo [OK] 数据库表检查完成
)
echo.

echo ========================================
echo MySQL数据库已启动
echo ========================================
echo.
echo 连接信息:
echo   主机: localhost
echo   端口: 3307
echo   用户: root
echo   密码: TestRootPass123456
echo   数据库: gm_tools
echo.
echo 默认管理员账号:
echo   用户名: admin
echo   密码: Admin@123456
echo   GM等级: 5 (超级管理员)
echo.
echo 现在可以启动后端和前端服务器了！
echo.
pause
