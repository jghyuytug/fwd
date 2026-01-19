# DNF GM Tools - 前端应用

DNF 游戏管理工具的前端界面，基于 React 18 + TypeScript + Vite + Ant Design 5 构建。

## 📋 项目信息

- **技术栈**: React 18 + TypeScript 5 + Vite 5 + Ant Design 5
- **状态管理**: Zustand + React Query
- **路由**: React Router 6
- **构建工具**: Vite 5
- **包管理器**: npm (推荐) 或 yarn
- **Node.js 版本**: ≥18.0.0

## 🚀 快速开始

### 1. 安装依赖

```bash
npm install
# 或
yarn install
```

### 2. 启动开发服务器

```bash
npm run dev
# 或
yarn dev
```

应用将在 http://localhost:3000 启动，并自动代理 API 请求到后端服务器 (http://localhost:3001)。

### 3. 构建生产版本

```bash
npm run build
# 或
yarn build
```

构建产物将输出到 `dist/` 目录。

### 4. 预览生产构建

```bash
npm run preview
# 或
yarn preview
```

## 📁 项目结构

```
frontend/
├── public/              # 静态资源
│   └── vite.svg
├── src/
│   ├── components/      # React 组件
│   │   ├── common/      # 通用组件
│   │   ├── layout/      # 布局组件
│   │   ├── player/      # 玩家相关组件
│   │   ├── gm/          # GM 操作组件
│   │   ├── audit/       # 审计日志组件
│   │   └── system/      # 系统管理组件
│   ├── pages/           # 页面组件
│   ├── services/        # API 服务
│   ├── hooks/           # 自定义 Hooks
│   ├── stores/          # Zustand 状态管理
│   ├── types/           # TypeScript 类型定义
│   ├── utils/           # 工具函数
│   ├── styles/          # 全局样式
│   ├── App.tsx          # 根组件
│   ├── main.tsx         # 应用入口
│   └── vite-env.d.ts    # Vite 环境变量类型声明
├── .env.development     # 开发环境变量
├── .env.production      # 生产环境变量
├── .eslintrc.cjs        # ESLint 配置
├── .prettierrc          # Prettier 配置
├── .gitignore           # Git 忽略文件
├── index.html           # HTML 模板
├── package.json         # 项目配置和依赖
├── tsconfig.json        # TypeScript 配置
├── tsconfig.node.json   # TypeScript Node 配置
├── vite.config.ts       # Vite 配置
└── README.md            # 本文档
```

## 🛠️ 可用脚本

| 命令 | 说明 |
|------|------|
| `npm run dev` | 启动开发服务器 (端口 3000) |
| `npm run build` | 构建生产版本 |
| `npm run preview` | 预览生产构建 |
| `npm run lint` | 运行 ESLint 检查 |
| `npm run lint:fix` | 自动修复 ESLint 问题 |
| `npm run format` | 格式化代码 (Prettier) |
| `npm run type-check` | TypeScript 类型检查 |
| `npm run test` | 运行单元测试 (Vitest) |
| `npm run test:ui` | 运行测试 UI 界面 |
| `npm run test:coverage` | 生成测试覆盖率报告 |
| `npm run e2e` | 运行 E2E 测试 (Playwright) |
| `npm run e2e:ui` | 运行 E2E 测试 UI 界面 |

## 📦 核心依赖

### 运行时依赖

- **React 生态**:
  - `react` ^18.2.0 - React 核心库
  - `react-dom` ^18.2.0 - React DOM 渲染
  - `react-router-dom` ^6.20.0 - 路由管理

- **UI 框架**:
  - `antd` ^5.12.0 - Ant Design 组件库
  - `@ant-design/icons` ^5.2.6 - Ant Design 图标
  - `@ant-design/pro-components` ^2.6.43 - Ant Design Pro 组件

- **状态管理**:
  - `zustand` ^4.4.7 - 轻量级状态管理
  - `@tanstack/react-query` ^5.12.2 - 服务器状态管理

- **表单处理**:
  - `react-hook-form` ^7.48.2 - 表单管理
  - `zod` ^3.22.4 - Schema 验证
  - `@hookform/resolvers` ^3.3.2 - 表单验证集成

- **网络请求**:
  - `axios` ^1.6.2 - HTTP 客户端

- **工具库**:
  - `dayjs` ^1.11.10 - 日期处理
  - `lodash-es` ^4.17.21 - 工具函数
  - `qrcode.react` ^3.1.0 - 二维码生成
  - `recharts` ^2.10.3 - 图表组件
  - `ahooks` ^3.7.8 - React Hooks 集合

### 开发依赖

- **构建工具**:
  - `vite` ^5.0.8 - 构建工具
  - `@vitejs/plugin-react` ^4.2.1 - React 插件

- **TypeScript**:
  - `typescript` ^5.3.3 - TypeScript 编译器
  - `@types/*` - 类型声明文件

- **代码质量**:
  - `eslint` ^8.55.0 - 代码检查
  - `prettier` ^3.1.1 - 代码格式化

- **测试**:
  - `vitest` ^1.0.4 - 单元测试框架
  - `@testing-library/react` ^14.1.2 - React 测试工具
  - `@playwright/test` ^1.40.1 - E2E 测试框架

## 🔧 配置说明

### 环境变量

在 `.env.development` 和 `.env.production` 中配置：

```env
VITE_APP_TITLE=应用标题
VITE_API_BASE_URL=API 基础 URL
VITE_API_TIMEOUT=API 超时时间 (毫秒)
VITE_ENABLE_MOCK=是否启用 Mock 数据
```

### API 代理

开发环境下，Vite 会自动将 `/api` 开头的请求代理到 `http://localhost:3001`。

配置位置: `vite.config.ts` → `server.proxy`

### 路径别名

项目配置了路径别名，可以使用 `@/` 前缀导入模块：

```typescript
import Button from '@/components/common/Button';
import { login } from '@/services/auth';
import useAuth from '@/hooks/useAuth';
```

配置位置: `vite.config.ts` 和 `tsconfig.json`

## 📝 开发规范

### 代码风格

- 使用 **TypeScript** 进行类型安全开发
- 遵循 **ESLint** 规则进行代码检查
- 使用 **Prettier** 统一代码格式
- 组件使用 **函数式组件** + **Hooks**
- 文件命名使用 **PascalCase** (组件) 或 **camelCase** (工具函数)

### Git 提交规范

```
feat: 新功能
fix: 修复 Bug
docs: 文档更新
style: 代码格式调整
refactor: 代码重构
test: 测试相关
chore: 构建/工具链更新
```

### 组件开发

1. 每个组件一个目录
2. 组件文件命名与组件名一致
3. 导出类型定义
4. 编写单元测试

示例:

```typescript
// components/common/Button/Button.tsx
import { FC } from 'react';

interface ButtonProps {
  text: string;
  onClick: () => void;
}

const Button: FC<ButtonProps> = ({ text, onClick }) => {
  return <button onClick={onClick}>{text}</button>;
};

export default Button;
export type { ButtonProps };
```

## 🎨 待实现功能

Phase 1 (项目初始化) 已完成，接下来将实现：

- [ ] **Phase 2**: 基础架构搭建
  - 路由配置
  - 布局组件 (MainLayout)
  - HTTP 客户端封装
  - 认证状态管理

- [ ] **Phase 3**: 认证模块
  - 登录页面
  - 2FA 验证
  - 权限守卫

- [ ] **Phase 4**: 控制台页面
  - 统计卡片
  - 趋势图表
  - 快速操作

- [ ] **Phase 5**: 玩家查询功能
  - 搜索表单
  - 结果表格
  - 玩家详情页

- [ ] **Phase 6**: GM 操作功能
  - 道具发放
  - 账号管理
  - 级别/金币修改

- [ ] **Phase 7**: 审计日志
  - 操作日志查询
  - 登录日志查询
  - 日志导出

- [ ] **Phase 8**: 系统管理
  - 权限管理
  - 角色管理

- [ ] **Phase 9**: 优化与测试
  - 性能优化
  - 单元测试
  - E2E 测试

- [ ] **Phase 10**: 部署与文档
  - Docker 配置
  - Nginx 配置
  - 部署文档

详细开发计划请参考: `WEEK5_FRONTEND_DEVELOPMENT_PLAN.md`

## 🔗 相关链接

- **后端项目**: `../backend/`
- **开发计划**: `WEEK5_FRONTEND_DEVELOPMENT_PLAN.md`
- **架构设计**: `../../docs/design/HYBRID_ARCHITECTURE_DESIGN_2025-11-09.md`
- **状态报告**: `../../docs/generated/HYBRID_ARCHITECTURE_STATUS_REPORT_2025-11-10.md`

## 📞 联系方式

如有问题，请联系开发团队或查看项目文档。

---

**最后更新**: 2025-11-10
**项目阶段**: Phase 1 完成 (项目初始化)
**下一步**: Phase 2 - 基础架构搭建
