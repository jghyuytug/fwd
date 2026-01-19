# DNF GM工具系统

**版本**: v1.0.0-dev  
**状态**: 🚧 开发中 (Week 5 - 前端开发阶段)  
**最后更新**: 2025-11-10

---

## 🎯 项目简介

DNF GM工具系统是一个基于Web的游戏管理后台，用于DNF游戏服务器的运维和管理。系统采用前后端分离架构，提供玩家查询、物品发放、账号管理、审计日志等核心功能。

### 核心特性

- ✅ **认证系统**: JWT + 2FA双因子认证 + 设备信任机制
- ✅ **权限管理**: 基于角色的访问控制 (GM等级1-5)
- ✅ **玩家查询**: 多维度查询玩家数据 (账号、角色、装备)
- ✅ **GM操作**: 物品发放、账号管理、服务器管理
- ✅ **审计日志**: 完整的操作记录和追溯
- ✅ **数据同步**: Canal实时同步生产数据库到镜像库
- ✅ **监控告警**: Prometheus + Grafana监控系统

---

## 🚀 快速启动

### 前置条件

- **Docker** 20.10+
- **Docker Compose** 1.29+
- **Node.js** 18+ (本地开发)

### 启动步骤

\`\`\`bash
# 1. 启动MySQL数据库
cd docker/
docker-compose up -d gm-mirror-db

# 2. 等待数据库初始化完成 (30-60秒)
docker-compose logs -f gm-mirror-db

# 3. 启动后端
cd ../backend/
npm install
npm run dev

# 4. 启动前端 (新终端)
cd ../frontend/
npm install
npm run dev

# 访问: http://localhost:3000
\`\`\`

### 默认账号

- **用户名**: \`admin\`
- **密码**: \`Admin@123456\`
- **GM等级**: 5 (超级管理员)

⚠️ **首次登录后请立即修改密码！**

---

## 📚 完整文档

**👉 [点击查看完整文档索引](docs/INDEX.md)**

文档包含：
- 快速入门指南
- 完整的开发计划
- API接口文档
- 部署运维手册
- 测试文档
- 历史归档

---

## 📂 项目结构

\`\`\`
gm_tools/
├── backend/          # 后端API服务 (Node.js + TypeScript)
├── frontend/         # 前端Web应用 (React + TypeScript)
├── database/         # 数据库脚本
├── docker/           # Docker配置
├── docs/             # 📚 项目文档 (从INDEX.md开始)
│   ├── INDEX.md      # 📖 文档导航入口
│   ├── active/       # 当前活跃文档
│   └── archive/      # 历史归档
└── README.md         # 本文档
\`\`\`

---

## 🌐 服务访问

| 服务 | 地址 | 说明 |
|------|------|------|
| 前端界面 | http://localhost:3000 | Web管理界面 |
| 后端API | http://localhost:3001/api/v1 | RESTful API |
| API文档 | http://localhost:3001/api/v1/docs | Swagger文档 |
| MySQL | localhost:3307 | root / TestRootPass123456 |

---

## 🛠️ 技术栈

**后端**: Node.js 18 + TypeScript 5 + Express 4 + MySQL 5.7 + Jest  
**前端**: React 18 + TypeScript 5 + Vite 5 + Ant Design 5 + Zustand  
**基础设施**: Docker + Canal + Nginx + Prometheus + Grafana

---

## 📊 当前进度

| 阶段 | 状态 | 进度 |
|------|------|------|
| Week 3: 基础设施 | ✅ 完成 | 100% |
| Week 4: 后端API | ✅ 完成 | 100% |
| **Week 5: 前端开发** | **🔄 进行中** | **20%** |

**当前任务**: Phase 3 - 登录页面开发

---

## 🔧 常用命令

\`\`\`bash
# Docker
docker-compose up -d gm-mirror-db      # 启动MySQL
docker-compose ps                       # 查看状态
docker-compose logs -f gm-mirror-db    # 查看日志

# 后端
cd backend/
npm run dev                            # 开发模式
npm test                               # 运行测试

# 前端
cd frontend/
npm run dev                            # 开发模式
npm run build                          # 构建生产版本
\`\`\`

---

## 📞 获取帮助

- **完整文档**: [docs/INDEX.md](docs/INDEX.md)
- **API文档**: http://localhost:3001/api/v1/docs
- **问题反馈**: 通过项目Issue跟踪

---

**💡 从 [文档索引](docs/INDEX.md) 开始，获取完整的项目文档！**
