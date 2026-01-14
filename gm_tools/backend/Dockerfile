# 多阶段构建 - Node.js 后端
# 阶段1: 构建
FROM node:18-alpine AS builder

WORKDIR /app

# 复制 package 文件
COPY package*.json ./
COPY tsconfig.json ./

# 安装所有依赖（包括 devDependencies）
RUN npm ci

# 复制源代码
COPY src ./src

# 编译 TypeScript
RUN npm run build

# 阶段2: 生产运行
FROM node:18-alpine

WORKDIR /app

# 安装 dumb-init
RUN apk add --no-cache dumb-init

# 复制 package 文件
COPY package*.json ./

# 仅安装生产依赖
RUN npm ci --only=production && npm cache clean --force

# 从构建阶段复制编译后的代码
COPY --from=builder /app/dist ./dist

# 创建日志目录
RUN mkdir -p logs

# 设置环境变量
ENV NODE_ENV=production \
    PORT=3001

# 暴露端口
EXPOSE 3001

# 健康检查
HEALTHCHECK --interval=30s --timeout=10s --start-period=40s --retries=3 \
  CMD node -e "require('http').get('http://localhost:3001/health', (r) => {process.exit(r.statusCode === 200 ? 0 : 1)})"

# 使用 dumb-init 运行应用
ENTRYPOINT ["dumb-init", "--"]
CMD ["node", "dist/server.js"]
