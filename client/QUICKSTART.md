# 快速开始指南

## 🚀 5分钟快速启动

### Step 1: 打开Visual Studio

```
双击: DnfClient.sln
```

### Step 2: 选择配置

- 平台: **Win32** (不是x64!)
- 配置: **Debug**

### Step 3: 构建并运行

按 **F5** 或 **Ctrl+F5**

### Step 4: 验证成功

看到蓝色窗口 = 成功！ ✅

---

## 📁 项目文件说明

```
client/
├── DnfClient.sln          ← 双击打开
├── DnfClient.vcxproj      ← VS项目文件
├── src/main.cpp           ← 源代码
├── README.md              ← 完整文档
├── PROGRESS.md            ← 进度追踪
├── VERIFY.md              ← 验证指南
└── docs/setup.md          ← 环境搭建
```

---

## 🔧 如果遇到问题

### 问题: 找不到d3d9.h

**解决**: 设置DirectX SDK环境变量

```cmd
setx DXSDK_DIR "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\"
```

重启Visual Studio

### 问题: 找不到d3dx9_43.dll

**解决**: 安装DirectX运行时

下载: https://www.microsoft.com/en-us/download/details.aspx?id=35

### 问题: 其他错误

查看: `VERIFY.md` 完整排查指南

---

## ✅ 验证成功后

### 下一步开发

1. **Task #2**: 实现游戏窗口和渲染循环
   - 创建Window类
   - 创建Renderer类
   - 创建Input类

2. **Task #3**: 实现PVF解析器
   - 移植C#代码到C++
   - 解析技能数据

3. **Task #4**: 实现NPK资源加载
   - 加载图标资源
   - 纹理管理

### 修改测试

尝试修改 `src/main.cpp`:

```cpp
// 改变清屏颜色
D3DCOLOR_XRGB(255, 0, 0)  // 红色
D3DCOLOR_XRGB(0, 255, 0)  // 绿色
D3DCOLOR_XRGB(255, 255, 0) // 黄色
```

---

## 📊 当前进度

- ✅ Phase 0: 环境搭建 (100%)
- ⏳ Phase 1: 核心框架 (0%)
- ⏳ Phase 2: 数据层 (0%)
- ⏳ Phase 3: 网络层 (0%)
- ⏳ Phase 4: UI层 (0%)

**总进度**: 14% (1/7任务完成)

---

## 📞 需要帮助？

- 查看 `README.md` - 完整开发文档
- 查看 `VERIFY.md` - 验证和排查指南
- 查看 `docs/setup.md` - 详细环境搭建

---

*祝开发顺利！🎮*
