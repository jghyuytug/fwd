/**
 * 数据库配置
 * 支持双数据库连接：
 * 1. GM工具数据库（读写）
 * 2. 镜像数据库（只读）
 */

import mysql from 'mysql2/promise';
import { logger } from '../utils/logger.util';

interface DatabaseConfig {
  host: string;
  port: number;
  user: string;
  password: string;
  database: string;
  waitForConnections: boolean;
  connectionLimit: number;
  queueLimit: number;
  timezone: string;
  charset: string;
}

// GM工具数据库配置（读写）
const gmToolsConfig: DatabaseConfig = {
  host: process.env.DB_HOST || 'localhost',
  port: parseInt(process.env.DB_PORT || '3307'),
  user: process.env.DB_USER || 'root',
  password: process.env.DB_PASSWORD || '',
  database: process.env.DB_NAME || 'gm_tools',
  waitForConnections: true,
  connectionLimit: parseInt(process.env.DB_POOL_MAX || '10'),
  queueLimit: 0,
  timezone: process.env.DB_TIMEZONE || '+08:00',
  charset: 'utf8mb4'
};

// 镜像数据库配置（只读）
const mirrorConfig: Omit<DatabaseConfig, 'database'> = {
  host: process.env.MIRROR_DB_HOST || 'localhost',
  port: parseInt(process.env.MIRROR_DB_PORT || '3307'),
  user: process.env.MIRROR_DB_USER || 'gm_tools_reader',
  password: process.env.MIRROR_DB_PASSWORD || '',
  waitForConnections: true,
  connectionLimit: parseInt(process.env.MIRROR_DB_POOL_MAX || '10'),
  queueLimit: 0,
  timezone: process.env.DB_TIMEZONE || '+08:00',
  charset: 'utf8mb4'
};

// 创建连接池
export const gmToolsPool = mysql.createPool(gmToolsConfig);
export const cainMirrorPool = mysql.createPool({
  ...mirrorConfig,
  database: process.env.MIRROR_DB_CAIN || 'taiwan_cain_mirror'
});
export const loginMirrorPool = mysql.createPool({
  ...mirrorConfig,
  database: process.env.MIRROR_DB_LOGIN || 'taiwan_login_mirror'
});

// 别名导出（兼容旧代码）
export const cainPool = cainMirrorPool;
export const loginPool = loginMirrorPool;

/**
 * 测试数据库连接
 */
export async function testDatabaseConnection(): Promise<void> {
  try {
    // 测试GM工具数据库
    await gmToolsPool.query('SELECT 1 as test');
    logger.info('GM工具数据库连接成功', { database: gmToolsConfig.database });

    // 测试角色镜像数据库
    await cainMirrorPool.query('SELECT 1 as test');
    logger.info('角色镜像数据库连接成功', { database: process.env.MIRROR_DB_CAIN });

    // 测试登录镜像数据库
    await loginMirrorPool.query('SELECT 1 as test');
    logger.info('登录镜像数据库连接成功', { database: process.env.MIRROR_DB_LOGIN });

  } catch (error) {
    logger.error('数据库连接失败', { error });
    throw error;
  }
}

/**
 * 关闭数据库连接
 */
export async function closeDatabaseConnection(): Promise<void> {
  await Promise.all([
    gmToolsPool.end(),
    cainMirrorPool.end(),
    loginMirrorPool.end()
  ]);
  logger.info('所有数据库连接已关闭');
}

// 处理进程退出
process.on('SIGINT', async () => {
  await closeDatabaseConnection();
  process.exit(0);
});
