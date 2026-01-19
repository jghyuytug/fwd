# DNF GM工具系统 - 后端API

基于Node.js + Express + TypeScript开发的GM工具后端API系统。

## 📋 目录

- [功能特性](#功能特性)
- [技术栈](#技术栈)
- [项目结构](#项目结构)
- [快速开始](#快速开始)
- [环境配置](#环境配置)
- [API文档](#api文档)
- [开发指南](#开发指南)
- [测试](#测试)
- [部署](#部署)

---

## 🚀 功能特性

- ✅ **用户认证系统**: JWT认证、双因素认证、设备信任
- ✅ **权限控制**: 基于角色的访问控制（RBAC）、细粒度权限管理
- ✅ **玩家数据查询**: 角色信息、装备、仓库、任务查询
- ✅ **GM操作**: 物品赠送、账号管理、封禁/解封
- ✅ **操作审计**: 完整的操作日志记录和追踪
- ✅ **安全防护**: Helmet安全头、CORS、限流、SQL注入防护
- ✅ **日志系统**: 结构化日志、日志分级、日志轮转

---

## 🛠️ 技术栈

**核心框架**:
- Node.js 18+ LTS
- Express 4.x
- TypeScript 5.x

**数据库**:
- MySQL 5.7
- MySQL2 (连接池)

**认证授权**:
- jsonwebtoken (JWT)
- bcryptjs (密码哈希)
- speakeasy (2FA)

**工具库**:
- Winston (日志)
- Morgan (HTTP日志)
- Helmet (安全)
- Cors (跨域)
- Joi (数据验证)

**测试**:
- Jest (单元测试)
- Supertest (集成测试)

---

## 📁 项目结构

```
backend/
├── src/
│   ├── config/              # 配置文件
│   │   └── database.ts      # 数据库配置
│   ├── middlewares/         # 中间件
│   │   ├── auth.ts          # 认证中间件
│   │   ├── rbac.ts          # 权限控制
│   │   ├── audit.ts         # 审计日志
│   │   └── errorHandler.ts # 错误处理
│   ├── routes/              # 路由定义
│   ├── controllers/         # 控制器
│   ├── services/            # 业务逻辑
│   ├── models/              # 数据模型
│   ├── utils/               # 工具函数
│   │   ├── logger.util.ts   # 日志工具
│   │   └── jwt.util.ts      # JWT工具
│   ├── types/               # TypeScript类型
│   ├── app.ts               # Express应用
│   └── server.ts            # 服务器启动
├── tests/                   # 测试文件
│   ├── unit/               # 单元测试
│   └── integration/        # 集成测试
├── docs/                    # 文档
│   └── api/                # API文档
├── logs/                    # 日志文件
├── package.json
├── tsconfig.json
├── .env.example
└── README.md
```

---

## ⚡ 快速开始

### 1. 安装依赖

```bash
cd backend
npm install
```

### 2. 配置环境变量

```bash
cp .env.example .env
# 编辑.env文件，配置数据库连接等信息
```

### 3. 启动开发服务器

```bash
npm run dev
```

服务器将在 `http://localhost:3001` 启动。

---

## ⚙️ 环境配置

### 必需配置

```bash
# 数据库配置
DB_HOST=localhost
DB_PORT=3307
DB_NAME=gm_tools
DB_USER=root
DB_PASSWORD=your_password

# JWT配置
JWT_SECRET=your_secret_key_min_32_characters
JWT_EXPIRATION=1800
```

### 完整配置

查看 `.env.example` 文件获取所有可配置项。

---

## 📖 API文档

### 健康检查

```bash
GET /health
```

响应:
```json
{
  "status": "healthy",
  "timestamp": "2025-11-09T10:00:00.000Z",
  "uptime": 123.456,
  "environment": "development"
}
```

### 认证API

查看 `docs/api/` 目录获取完整API文档。

---

## 💻 开发指南

### 运行开发服务器

```bash
npm run dev
```

### 构建生产版本

```bash
npm run build
```

### 启动生产服务器

```bash
npm start
```

### 代码检查

```bash
npm run lint
npm run lint:fix
```

---

## 🧪 测试

### 运行所有测试

```bash
npm test
```

### 运行单元测试

```bash
npm run test:unit
```

### 运行集成测试

```bash
npm run test:integration
```

### 监视模式

```bash
npm run test:watch
```

---

## 🚀 部署

### Docker部署

```bash
# 构建Docker镜像
docker build -t dnf-gm-backend .

# 运行容器
docker run -d \
  --name dnf-gm-backend \
  -p 3001:3001 \
  --env-file .env \
  dnf-gm-backend
```

### 使用Docker Compose

```bash
docker-compose -f docker-compose-backend.yml up -d
```

---

## 📝 开发规范

### 提交规范

- `feat`: 新功能
- `fix`: 修复bug
- `docs`: 文档更新
- `style`: 代码格式
- `refactor`: 重构
- `test`: 测试
- `chore`: 构建/工具

示例:
```
feat(auth): 实现JWT认证
fix(player): 修复角色查询分页问题
```

---

## 🔒 安全

- **认证**: JWT + 设备指纹
- **加密**: bcrypt密码哈希
- **防护**: Helmet安全头、SQL注入防护
- **限流**: 防止暴力破解
- **审计**: 完整操作日志

---

## 📞 支持

- **文档**: 查看 `docs/` 目录
- **问题**: 提交Issue
- **开发**: 查看 `WEEK4_DEVELOPMENT_PLAN.md`

---

## 📄 许可证

MIT License

---

**版本**: 1.0.0
**更新日期**: 2025-11-09
