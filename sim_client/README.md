# sim_client - DNF 资源解析工具

离线资源解析工具，用于从游戏资源包 (PVF/NPK) 中提取和预览美术资源。

## 功能

- **ANI 动画解码**: 从 PVF 提取动画定义，解析 NPK 图像包，逐帧解码为 PPM 格式
- **ANI 文件列表**: 搜索 PVF 中所有 ANI 文件，支持关键词过滤
- **PVF 文件转储**: 提取 PVF 内任意文件内容（十六进制）

## 支持的图像格式

- ARGB1555 (16-bit)
- ARGB4444 (16-bit)
- ARGB8888 (32-bit)
- INDEX8 (8-bit 索引色)
- DXT1 (DDS 压缩)
- DXT3 (DDS 压缩 + 显式 Alpha)
- DXT5 (DDS 压缩 + 插值 Alpha)

## 构建

### 依赖

- GCC (MinGW/MSYS2 on Windows)
- zlib

### 编译

```bash
cd sim_client
make
```

或使用 PowerShell 脚本:

```powershell
.\build.ps1
```

### 输出

编译后生成 `bin/sim_client.exe`

## 使用方法

### 解码 ANI 动画

```bash
sim_client decode-ani <pvf_path> <imagepacks2_dir> <ani_path> [max_frames] [out_dir]
```

参数:
- `pvf_path`: PVF 文件路径 (如 `Script.pvf`)
- `imagepacks2_dir`: ImagePacks2 目录路径
- `ani_path`: PVF 内的 ANI 文件路径
- `max_frames`: 每个动作最大解码帧数 (默认 5)
- `out_dir`: 输出目录 (默认 `output`)

示例:

```bash
sim_client decode-ani "D:/DNF/Script.pvf" "D:/DNF/ImagePacks2" "character/swordman/animation/idle.ani" 10 output
```

### 列出 ANI 文件

```bash
sim_client list-ani <pvf_path> [filter] [limit]
```

参数:
- `pvf_path`: PVF 文件路径
- `filter`: 过滤关键词 (可选)
- `limit`: 最大显示数量 (默认 50)

示例:

```bash
sim_client list-ani "D:/DNF/Script.pvf" "swordman" 100
```

### 转储 PVF 文件

```bash
sim_client dump-pvf <pvf_path> <file_path> [bytes]
```

参数:
- `pvf_path`: PVF 文件路径
- `file_path`: PVF 内的文件路径
- `bytes`: 显示字节数 (默认 256)

示例:

```bash
sim_client dump-pvf "D:/DNF/Script.pvf" "character/swordman/animation/idle.ani" 512
```

## 输出格式

解码的图像以 PPM (Portable Pixmap) 格式保存，可用以下工具查看:
- IrfanView
- GIMP
- ImageMagick (`display` 命令)
- 任何支持 PPM 的图像查看器

## 目录结构

```
sim_client/
├── bin/                    # 编译输出
│   ├── sim_client.exe
│   └── zlib1.dll
├── build/obj/              # 目标文件
├── src/
│   ├── main.c              # CLI 入口
│   ├── sim_app.c           # 核心实现
│   └── sim_app.h           # 头文件
├── Makefile
├── build.ps1               # 构建脚本
├── run_decode.ps1          # 测试脚本
└── README.md
```

## 依赖模块

本工具复用以下模块:

- `client/src/resource/ani_loader.c` - ANI 动画加载器
- `client/src/resource/npk_parser.c` - NPK 解析器
- `client/src/resource/img_decoder.c` - IMG 图像解码器
- `src/pvf/` - PVF 解析库

## 技术说明

### NPK 文件查找策略

工具会根据 IMG 路径智能推断 NPK 文件名:

1. 尝试精确匹配: `sprite_category_subcategory_name.NPK`
2. 尝试二级匹配: `sprite_category_subcategory.NPK`
3. 尝试一级匹配: `sprite_category.NPK`
4. 遍历目录搜索

### 图像解码流程

```
PVF → ANI_LoadFromPVF() → 解析动画定义
              ↓
       IMG 路径提取 → NPK_Open() → NPK_FindFile()
              ↓
       NPK_ReadFile() → IMG_Parse() → IMG_DecodeFrame()
              ↓
       RGBA32 数据 → RGB 转换 → PPM 文件输出
```

## 许可证

内部开发工具，仅供学习研究使用。
