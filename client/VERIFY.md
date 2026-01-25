# 环境验证指南

## 快速验证步骤

### 方法1: 使用Visual Studio (推荐)

1. **打开解决方案**
   ```
   双击: C:\Users\waw\Desktop\game\client\DnfClient.sln
   ```

2. **检查配置**
   - 确保平台选择: `Win32` (不是x64)
   - 确保配置选择: `Debug`

3. **构建项目**
   - 按 `Ctrl+Shift+B` 或
   - 菜单: 生成 → 生成解决方案

4. **运行程序**
   - 按 `F5` (调试运行) 或
   - 按 `Ctrl+F5` (不调试运行)

5. **预期结果**
   - 打开一个800x600窗口
   - 窗口标题: "DNF Client - Hello DirectX 9"
   - 窗口背景: 蓝色
   - 按ESC键退出

---

### 方法2: 使用命令行

1. **打开Developer Command Prompt**
   ```
   开始菜单 → Visual Studio 2022 → Developer Command Prompt for VS 2022
   ```

2. **导航到项目目录**
   ```cmd
   cd C:\Users\waw\Desktop\game\client
   ```

3. **构建项目**
   ```cmd
   msbuild DnfClient.sln /p:Configuration=Debug /p:Platform=Win32
   ```

4. **运行程序**
   ```cmd
   bin\Debug\DnfClient.exe
   ```

---

### 方法3: 使用构建脚本

1. **运行构建脚本**
   ```cmd
   cd C:\Users\waw\Desktop\game\client
   build_msbuild.bat
   ```

2. **运行程序**
   ```cmd
   bin\Debug\DnfClient.exe
   ```

---

## 验证检查清单

### ✅ 环境检查

- [ ] Visual Studio 2019/2022 已安装
- [ ] C++桌面开发工作负载已安装
- [ ] DirectX SDK (June 2010) 已安装
- [ ] DXSDK_DIR 环境变量已设置（或使用默认路径）

### ✅ 文件检查

- [ ] `DnfClient.sln` 存在
- [ ] `DnfClient.vcxproj` 存在
- [ ] `src/main.cpp` 存在
- [ ] DirectX SDK 路径正确: `C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)`

### ✅ 编译检查

- [ ] 无编译错误
- [ ] 无链接错误
- [ ] 生成了 `bin\Debug\DnfClient.exe`

### ✅ 运行检查

- [ ] 程序启动无错误
- [ ] 窗口正常显示
- [ ] 背景为蓝色
- [ ] ESC键可以退出

---

## ���见问题排查

### 问题1: 打开.sln文件时提示"无法打开项目"

**原因**: Visual Studio版本不匹配

**解决**:
1. 右键 `DnfClient.sln` → 打开方式 → Visual Studio 2022
2. 或修改 `.sln` 文件第一行的版本号

### 问题2: 编译错误 "无法打开包括文件: 'd3d9.h'"

**原因**: DirectX SDK路径未配置

**解决**:
1. 检查环境变量 `DXSDK_DIR`:
   ```cmd
   echo %DXSDK_DIR%
   ```
2. 如果未设置，手动设置:
   ```cmd
   setx DXSDK_DIR "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\"
   ```
3. 重启Visual Studio

### 问题3: 链接错误 "无法解析的外部符号"

**原因**: 库路径或库文件未正确链接

**解决**:
1. 在Visual Studio中:
   - 右键项目 → 属性
   - VC++目录 → 库目录
   - 确认包含: `$(DXSDK_DIR)Lib\x86`
2. 链接器 → 输入 → 附加依赖项
   - 确认包含: `d3d9.lib;d3dx9.lib;dinput8.lib;dxguid.lib`

### 问题4: 运行时错误 "找不到d3dx9_43.dll"

**原因**: 缺少DirectX运行时DLL

**解决方案A** (推荐):
1. 安装 DirectX End-User Runtime
   - 下载: https://www.microsoft.com/en-us/download/details.aspx?id=35
   - 运行安装程序

**解决方案B**:
1. 复制DLL到程序目录:
   ```cmd
   copy "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Developer Runtime\x86\D3DX9_43.dll" bin\Debug\
   ```

### 问题5: 窗口显示黑屏

**原因**: Direct3D设备初始化失败

**调试步骤**:
1. 在Visual Studio中按F5调试运行
2. 查看输出窗口的错误信息
3. 检查显卡驱动是否支持DirectX 9
4. 尝试使用软件渲染（修改代码中的D3DDEVTYPE_HAL为D3DDEVTYPE_REF）

---

## 验证成功标志

当您看到以下情况时，说明环境搭建成功：

```
✅ Visual Studio可以打开项目
✅ 编译无错误无警告
✅ 程序运行显示蓝色窗口
✅ ESC键可以正常退出
```

---

## 下一步

环境验证成功后，可以：

1. **修改代码测试**
   - 修改 `main.cpp` 中的 `D3DCOLOR_XRGB(0, 0, 255)`
   - 改为 `D3DCOLOR_XRGB(255, 0, 0)` (红色)
   - 重新编译运行，验证修改生效

2. **开始Phase 1开发**
   - 创建Window类
   - 创建Renderer类
   - 创建Input类
   - 实现游戏主循环

3. **阅读文档**
   - `README.md` - 项目概览
   - `PROGRESS.md` - 进度追踪
   - `docs/setup.md` - 详细环境指南

---

## 获取帮助

如果遇到问题：

1. 查看 `docs/setup.md` 的常见问题部分
2. 检查 Visual Studio 输出窗口的错误信息
3. 确认 DirectX SDK 安装完整
4. 确认 Visual Studio C++ 工作负载已安装

---

*最后更新: 2026-01-25*
