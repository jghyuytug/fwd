# 🎮 DNF装备浏览器

**Web可视化界面，用于浏览和查看DNF PVF文件中的装备数据**

---

## ✨ 功能特点

### 核心功能
- 📁 **文件浏览**: 树形结构浏览PVF中的所有装备文件
- 📊 **属性展示**: 详细展示装备的29个属性
  - 基本信息（品级、稀有度、等级、重量）
  - 四维属性（力量、智力、体力、精神）
  - 生命魔法（HP MAX、MP MAX）
  - 防御属性（物理防御、魔法防御、装备攻防）
  - 恢复速度（HP恢复、MP恢复）
  - 特殊属性（驱魔、物理暴击、魔法暴击）
  - 速度属性（移动、攻击、施放速度）
  - 价格信息（基础价格、修理价格、出售价格）
  - 其他信息（冷却时间、负重限制）

### 高级功能
- 🔍 **快速搜索**: 实时搜索文件名
- 🎯 **高级过滤**: 按稀有度、等级、品级筛选
- 📤 **数据导出**: 一键导出装备JSON数据
- 🎨 **稀有度配色**: 按照DNF标准显示不同稀有度颜色
  - 普通（灰）、高级（蓝）、稀有（紫）
  - 神器（粉）、传说（橙）、史诗（金）

### 用户体验
- 💻 **现代UI**: 渐变背景、圆角卡片、流畅动画
- 📱 **响应式设计**: 适配不同屏幕尺寸
- ⚡ **实时加载**: 无需刷新页面
- 🌐 **跨平台**: 支持Windows/Linux/macOS

---

## 📦 项目结构

```
pvf_browser/
├── backend/              # C语言后端
│   ├── pvf_api.c        # API主程序
│   ├── Makefile         # 编译脚本
│   └── pvf_api(.exe)    # 编译后的可执行文件
│
├── frontend/            # Web前端
│   ├── index.html       # 主页面
│   ├── style.css        # 样式表
│   └── app.js           # JavaScript逻辑
│
├── server.py            # Python Web服务器
├── start.bat            # Windows启动脚本
├── start.sh             # Linux/macOS启动脚本
└── README.md            # 本文档
```

---

## 🚀 快速开始

### 前置要求

**编译环境**:
- GCC编译器（支持32位）
- zlib开发库
- Python 3.6+ (用于Web服务器)

**运行环境**:
- 现代浏览器（Chrome、Firefox、Edge、Safari）

### 安装步骤

#### 1. 编译后端 (首次使用)

**在Docker环境中编译**（推荐）:
```bash
cd tools/pvf_browser
docker run -v "C:\Users\waw\Desktop\game":/build dnf_game_builder bash -c "
    yum install -y zlib-devel.i686 > /dev/null 2>&1 &&
    cd /build/tools/pvf_browser/backend &&
    make
"
```

**在本地编译**（如果有编译环境）:
```bash
cd backend
make
```

编译成功后会生成 `pvf_api` (Linux/macOS) 或 `pvf_api.exe` (Windows)

#### 2. 配置PVF路径

编辑 `server.py`，修改PVF文件路径：
```python
PVF_PATH = "../../Script.pvf"  # 修改为实际路径
```

#### 3. 启动服务器

**Windows**:
```cmd
start.bat
```

**Linux/macOS**:
```bash
chmod +x start.sh
./start.sh
```

或直接运行Python:
```bash
python server.py
# 或
python3 server.py
```

#### 4. 打开浏览器

访问: **http://localhost:8000**

---

## 📖 使用指南

### 基本操作

1. **浏览文件**
   - 左侧边栏显示所有装备文件
   - 点击文件名查看详情
   - 使用搜索框快速查找

2. **查看属性**
   - 中间面板显示完整装备属性
   - 颜色编码：绿色(正值)、红色(负值)、灰色(0/空)
   - 稀有度按DNF标准配色

3. **高级搜索**
   - 右侧面板提供多条件筛选
   - 支持关键词、稀有度、等级过滤
   - 点击搜索结果快速跳转

4. **导出数据**
   - 点击"📤 导出JSON"按钮
   - 下载完整装备数据
   - 用于数据分析或二次开发

### 快捷操作

| 操作 | 说明 |
|------|------|
| 点击文件 | 查看装备详情 |
| 搜索框输入 | 实时过滤文件 |
| 点击✕ | 清除搜索 |
| 切换文件类型 | 切换装备/消耗品/全部 |
| 导出JSON | 保存装备数据 |

---

## 🔧 API参考

后端提供4个JSON API接口：

### 1. 获取统计信息
```
GET /api?action=stats
```

响应:
```json
{
  "success": true,
  "total_files": 378621,
  "equipment": 69432,
  "stackable": 12000,
  "animation": 213708,
  "other": 83481
}
```

### 2. 列出文件
```
GET /api?action=list&path=equipment/&type=equ
```

参数:
- `path`: 目录路径（可选，默认为根目录）
- `type`: 文件类型（equ/stk/all）

响应:
```json
{
  "success": true,
  "path": "equipment/",
  "items": [
    {
      "name": "equipment/bracelet/brac_2choro994.equ",
      "size": 487,
      "crc32": "0x12345678"
    }
  ],
  "count": 1000
}
```

### 3. 解析装备
```
GET /api?action=parse&file=equipment/bracelet/brac_2choro994.equ
```

响应:
```json
{
  "success": true,
  "filepath": "equipment/bracelet/brac_2choro994.equ",
  "basic": {
    "item_id": "0x000786BF",
    "grade": 65,
    "rarity": 5,
    "min_level": 65,
    "weight": 800
  },
  "stats": { ... },
  "defense": { ... },
  "special": { ... }
}
```

### 4. 搜索装备
```
GET /api?action=search&keyword=belt&criteria=epic
```

参数:
- `keyword`: 关键词（可选）
- `criteria`: 筛选条件（all/epic/legendary/high_level）

响应:
```json
{
  "success": true,
  "keyword": "belt",
  "results": [
    {
      "path": "equipment/.../belt_xxx.equ",
      "grade": 65,
      "rarity": 5,
      "level": 65
    }
  ],
  "count": 100
}
```

---

## 🎨 技术栈

### 后端
- **语言**: C (C99)
- **库**: zlib (解压缩)
- **架构**: 命令行工具 + JSON输出

### 前端
- **HTML5**: 语义化标签
- **CSS3**: Flexbox、Grid、渐变、动画
- **JavaScript**: ES6+、Fetch API、异步处理

### 服务器
- **Python**: http.server + subprocess
- **通信**: HTTP + JSON

---

## ⚙️ 配置选项

### 修改端口号

编辑 `server.py`:
```python
PORT = 8080  # 改为你想要的端口
```

### 修改PVF路径

编辑 `server.py`:
```python
PVF_PATH = "/path/to/your/Script.pvf"
```

### 限制返回数量

编辑 `backend/pvf_api.c`，修改限制：
```c
if (count >= 1000) {  // 改为你想要的数量
    printf(",\n  {\"warning\":\"结果超过1000条，已截断\"}");
    break;
}
```

---

## 🐛 故障排除

### 问题1: 编译失败

**错误**: `cannot find -lz`

**解决**:
```bash
# Ubuntu/Debian
sudo apt-get install zlib1g-dev:i386

# CentOS/RHEL
sudo yum install zlib-devel.i686
```

### 问题2: 服务器启动失败

**错误**: `找不到API程序`

**解决**: 先编译后端
```bash
cd backend && make
```

### 问题3: 无法加载文件

**错误**: `无法打开PVF文件`

**解决**: 检查PVF路径是否正确
```bash
ls -l ../../Script.pvf
```

### 问题4: 端口占用

**错误**: `Address already in use`

**解决**: 修改端口号或关闭占用该端口的程序
```bash
# 查找占用端口的进程
netstat -ano | findstr :8000  # Windows
lsof -i :8000                 # Linux/macOS
```

---

## 📊 性能指标

| 指标 | 数值 |
|------|------|
| 支持的装备数量 | 69,000+ |
| 单个装备解析时间 | <10ms |
| 文件列表加载 | <500ms |
| 内存占用 | ~50MB |
| 浏览器兼容性 | Chrome 80+, Firefox 75+, Edge 80+, Safari 13+ |

---

## 🔮 未来计划

- [ ] 支持更多文件类型（.stk消耗品, .skl技能, .qst任务）
- [ ] 元素属性和异常状态抗性显示
- [ ] 装备对比功能
- [ ] 数据统计和图表
- [ ] 装备编辑功能
- [ ] 批量导出
- [ ] 暗黑模式
- [ ] 多语言支持

---

## 📄 许可证

本项目仅供学习和研究使用。

---

## 🤝 贡献

欢迎提交问题和改进建议！

---

**祝你使用愉快！**🎉
