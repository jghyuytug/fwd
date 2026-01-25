# DNF 自定义客户端开发文档

> 基于 C++ + DirectX 9 的完全复刻方案
> 开始时间: 2026-01-25

---

## 项目概述

### 技术栈

```
语言:       C++ (C++17/20)
编译器:     Microsoft Visual C++ (Visual Studio 2019/2022)
图形API:    DirectX 9 (June 2010 SDK)
输入:       DirectInput 8
音频:       FMOD Ex
网络:       WinSock 2
架构:       x86 (32位)
```

### 设计目标

1. **完全复刻原版技术栈** - 与2013年DNF客户端保持一致
2. **与服务端协同** - 对接game服务端的211个协议
3. **数据兼容** - 支持PVF/NPK原版数据格式
4. **可维护性** - 清晰的代码结构，便于扩展

---

## 项目结构

```
client/
├── README.md                 # 本文档
├── PROGRESS.md              # 进度追踪
├── docs/                    # 详细文档
│   ├── setup.md            # 环境搭建指南
│   ├── architecture.md     # 架构设计
│   ├── protocol.md         # 网络协议
│   └── pvf_format.md       # PVF文件格式
├── src/                     # 源代码
│   ├── core/               # 核心框架
│   ├── graphics/           # 图形渲染
│   ├── input/              # 输入处理
│   ├── network/            # 网络通信
│   ├── data/               # 数据解析
│   ├── resource/           # 资源管理
│   ├── game/               # 游戏逻辑
│   ├── ui/                 # 用户界面
│   └── main.cpp            # 程序入口
├── include/                 # 公共头文件
├── lib/                     # 第三方库
├── build/                   # 构建输出
└── DnfClient.sln           # Visual Studio解决方案
```

---

## 开发阶段

### Phase 0: 环境搭建 ✅
- [x] 安装 DirectX 9 SDK (June 2010)
- [x] 配置 Visual Studio 项目
- [ ] 创建 Hello DirectX9 示例
- [ ] 验证编译和运行

### Phase 1: 核心框架 (Week 1-2)
- [ ] 游戏窗口 (Win32)
- [ ] 渲染循环 (Direct3D9)
- [ ] 输入管理 (DirectInput8)
- [ ] 帧率控制 (60fps)

### Phase 2: 数据层 (Week 3-5)
- [ ] PVF文件解析器
- [ ] NPK/IMG资源加载
- [ ] 技能数据结构
- [ ] 字符串表管理

### Phase 3: 网络层 (Week 6-7)
- [ ] TCP客户端 (WinSock2)
- [ ] 协议编解码
- [ ] 消息分发器
- [ ] 登录协议实现

### Phase 4: UI层 (Week 8-11)
- [ ] 登录界面
- [ ] 角色选择
- [ ] 技能树UI
- [ ] 基础HUD

---

## 编译说明

### 环境要求

- Windows 10/11
- Visual Studio 2019 或 2022
- DirectX 9 SDK (June 2010)
- Windows SDK 10.0

### 编译步骤

```bash
# 1. 打开解决方案
start DnfClient.sln

# 2. 选择配置
#    - Debug x86 (开发调试)
#    - Release x86 (发布版本)

# 3. 构建
#    Ctrl+Shift+B 或 菜单: 生成 -> 生成解决方案

# 4. 运行
#    F5 (调试) 或 Ctrl+F5 (不调试)
```

### 依赖库

- d3d9.lib - Direct3D 9
- d3dx9.lib - DirectX 辅助库
- dinput8.lib - DirectInput 8
- dxguid.lib - DirectX GUID
- ws2_32.lib - WinSock 2
- winmm.lib - Windows 多媒体

---

## 与服务端协同

### 服务端信息

```
位置: ../src/
架构: C语言，模块化设计
协议: 211个已实现
服务: Bridge, Channel, Game, Guild, Monitor
```

### 协议对接

客户端需要实现与以下服务的通信：

1. **Bridge服务** - 网关，负责连接分发
2. **Channel服务** - 频道选择
3. **Game服务** - 游戏逻辑主服务

协议定义参考：
- `../include/network/headers/packet_types.h`
- `../include/network/headers/packet_enums.h`

---

## 数据文件

### PVF文件

```
位置: C:\Users\waw\Desktop\DNF\Script.pvf
格式: 加密+压缩的文件包
解密: XOR + RotateRight 6
密钥: 2175242257
```

参考实现：
- `C:\Users\waw\Desktop\DNF\SkillTreeViewer\Services\PvfService.cs`

### NPK文件

```
位置: C:\Users\waw\Desktop\DNF\ImagePacks2\
格式: Neople Pack (图像资源包)
包含: IMG文件 (精灵图集)
```

参考实现：
- `C:\Users\waw\Desktop\DNF\SkillTreeViewer\Services\NpkService.cs`

---

## 开发规范

### 代码风格

- 使用4空格缩进
- 类名: PascalCase (例: `TextureManager`)
- 函数名: PascalCase (例: `LoadTexture`)
- 变量名: camelCase (例: `textureCache`)
- 成员变量: m_前缀 (例: `m_device`)
- 常量: UPPER_CASE (例: `MAX_TEXTURES`)

### 内存管理

- 优先使用智能指针 (`std::unique_ptr`, `std::shared_ptr`)
- COM对象使用 `ComPtr<T>` (Windows Runtime Library)
- 手动管理时确保配对 (new/delete, malloc/free)
- 使用RAII模式管理资源

### 错误处理

- DirectX函数检查 `HRESULT`
- 使用 `FAILED()` 宏判断
- 关键错误抛出异常
- 非关键错误记录日志

---

## 调试工具

### Visual Studio

- 断点调试 (F9)
- 监视窗口 (Watch)
- 调用堆栈 (Call Stack)
- 内存窗口 (Memory)

### DirectX

- DirectX Control Panel (SDK工具)
- PIX for Windows (性能分析)
- Debug Runtime (D3D9d.dll)

### 第三方

- RenderDoc (图形调试)
- Visual Leak Detector (内存泄漏检测)

---

## 参考资料

### 官方文档

- [DirectX 9 SDK Documentation](https://docs.microsoft.com/en-us/windows/win32/directx)
- [MSDN Win32 API](https://docs.microsoft.com/en-us/windows/win32/api/)

### 教程

- [DirectX Tutorial](http://www.directxtutorial.com/)
- [Rastertek DirectX 9 Tutorials](http://www.rastertek.com/dx9tut01.html)

### 社区

- [GameDev.net](https://www.gamedev.net/)
- [Stack Overflow - DirectX](https://stackoverflow.com/questions/tagged/directx)

---

## 许可证

本项目仅用于学习和研究目的。

---

*最后更新: 2026-01-25*
*维护者: Development Team*
