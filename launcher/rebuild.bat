@echo off
chcp 65001
set PATH=C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.6.0\mingw_64\bin;%PATH%
echo 开始编译...
qmake DnfLogin.pro
mingw32-make -j4
echo.
echo 编译完成！
pause
