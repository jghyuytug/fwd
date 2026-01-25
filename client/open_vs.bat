@echo off
REM 快速打开Visual Studio解决方案

echo Opening Visual Studio solution...

if exist build\DnfClient.sln (
    start build\DnfClient.sln
    echo Solution opened!
) else (
    echo [ERROR] Solution not found!
    echo Please run build.bat first to generate the project.
    pause
)
