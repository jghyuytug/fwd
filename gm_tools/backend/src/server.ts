/**
 * HTTP服务器启动
 * 负责启动Express应用和数据库连接
 */

import dotenv from 'dotenv';
import path from 'path';
import { logger } from './utils/logger.util';

// 加载环境变量
dotenv.config();

import app from './app';
import { testDatabaseConnection, closeDatabaseConnection } from './config/database';

const PORT = parseInt(process.env.PORT || '3001');
const NODE_ENV = process.env.NODE_ENV || 'development';

/**
 * 启动服务器
 */
async function startServer(): Promise<void> {
  try {
    // 测试数据库连接
    logger.info('正在连接数据库...');
    await testDatabaseConnection();
    logger.info('数据库连接成功');

    // 启动HTTP服务器
    app.listen(PORT, () => {
      logger.info('服务器启动成功', {
        port: PORT,
        environment: NODE_ENV,
        pid: process.pid
      });

      logger.info(`API地址: http://localhost:${PORT}${process.env.API_PREFIX || '/api/v1'}`);
      logger.info(`健康检查: http://localhost:${PORT}${process.env.HEALTH_CHECK_PATH || '/health'}`);

      if (process.env.SWAGGER_ENABLED === 'true') {
        logger.info(`API文档: http://localhost:${PORT}${process.env.API_PREFIX}/docs`);
      }
    });

  } catch (error) {
    logger.error('服务器启动失败', { error });
    process.exit(1);
  }
}

/**
 * 优雅关闭
 */
async function gracefulShutdown(signal: string): Promise<void> {
  logger.info(`收到${signal}信号，开始优雅关闭...`);

  try {
    // 关闭数据库连接
    await closeDatabaseConnection();
    logger.info('数据库连接已关闭');

    logger.info('服务器已关闭');
    process.exit(0);
  } catch (error) {
    logger.error('优雅关闭失败', { error });
    process.exit(1);
  }
}

// 处理进程信号
process.on('SIGTERM', () => gracefulShutdown('SIGTERM'));
process.on('SIGINT', () => gracefulShutdown('SIGINT'));

// 处理未捕获的异常
process.on('uncaughtException', (error) => {
  logger.error('未捕获的异常', { error });
  process.exit(1);
});

process.on('unhandledRejection', (reason, promise) => {
  logger.error('未处理的Promise拒绝', { reason, promise });
  process.exit(1);
});

// 启动服务器
startServer();
