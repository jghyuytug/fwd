/**
 * Express应用配置
 * 配置中间件、路由和错误处理
 */

import express, { Application } from 'express';
import cors from 'cors';
import helmet from 'helmet';
import 'express-async-errors'; // 自动捕获async/await错误
import { errorHandler, notFoundHandler } from './middlewares/errorHandler';
import { requestLogger } from './middlewares/audit';
import { logger } from './utils/logger.util';

// 导入路由模块
import authRoutes from './routes/auth.routes';
import dashboardRoutes from './routes/dashboard.routes'; 
import playerRoutes from './routes/player.routes';
import operationRoutes from './routes/operation.routes';
import auditRoutes from './routes/audit.routes';

const app: Application = express();

// ========================================
// 安全中间件
// ========================================
app.use(helmet());

// CORS配置
const corsOptions = {
  origin: (process.env.CORS_ORIGIN || 'http://localhost:3000').split(','),
  credentials: process.env.CORS_CREDENTIALS === 'true',
  optionsSuccessStatus: 200
};
app.use(cors(corsOptions));

// ========================================
// 请求解析中间件
// ========================================
app.use(express.json({ limit: '10mb' }));
app.use(express.urlencoded({ extended: true, limit: '10mb' }));

// ========================================
// 字符编码中间件
// ========================================
app.use((req, res, next) => {
  res.setHeader('Content-Type', 'application/json; charset=utf-8');
  next();
});

// ========================================
// 日志中间件
// ========================================
app.use(requestLogger);

// ========================================
// 健康检查
// ========================================
app.get(process.env.HEALTH_CHECK_PATH || '/health', (req, res) => {
  res.status(200).json({
    status: 'healthy',
    timestamp: new Date().toISOString(),
    uptime: process.uptime(),
    environment: process.env.NODE_ENV
  });
});

// ========================================
// API路由
// ========================================
const API_PREFIX = process.env.API_PREFIX || '/api/v1';

// 注册路由
app.use(`${API_PREFIX}/auth`, authRoutes);
app.use(`${API_PREFIX}/dashboard`, dashboardRoutes);  
app.use(`${API_PREFIX}/players`, playerRoutes);
app.use(`${API_PREFIX}/operations`, operationRoutes);
app.use(`${API_PREFIX}/audit`, auditRoutes);

// 根路径
app.get('/', (req, res) => {
  res.json({
    name: 'DNF GM Tools API',
    version: '1.0.0',
    status: 'running',
    docs: process.env.SWAGGER_ENABLED === 'true' ? `${API_PREFIX}/docs` : undefined
  });
});

// ========================================
// Swagger API文档
// ========================================
if (process.env.SWAGGER_ENABLED !== 'false') {
  // 默认启用Swagger文档
  import('swagger-ui-express').then((swaggerUi) => {
    import('./config/swagger').then(({ swaggerSpec, swaggerUiOptions }) => {
      app.use(`${API_PREFIX}/docs`, swaggerUi.default.serve);
      app.use(`${API_PREFIX}/docs`, swaggerUi.default.setup(swaggerSpec, swaggerUiOptions));

      // Swagger JSON端点
      app.get(`${API_PREFIX}/docs.json`, (req, res) => {
        res.setHeader('Content-Type', 'application/json');
        res.send(swaggerSpec);
      });

      logger.info('Swagger API文档已启用', {
        path: `${API_PREFIX}/docs`,
        jsonPath: `${API_PREFIX}/docs.json`
      });
    });
  }).catch((err) => {
    logger.warn('无法加载Swagger文档', { error: err.message });
  });
}

// ========================================
// 错误处理
// ========================================
app.use(notFoundHandler);
app.use(errorHandler);

export default app;
