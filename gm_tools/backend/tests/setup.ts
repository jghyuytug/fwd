/**
 * 测试环境设置
 * 在所有测试运行前执行
 */

import dotenv from 'dotenv';
import path from 'path';

// 加载测试环境变量
dotenv.config({ path: path.join(__dirname, '../.env.test') });

// 设置测试环境
process.env.NODE_ENV = 'test';

// 设置默认测试环境变量（如果未在.env.test中设置）
if (!process.env.JWT_SECRET) {
  process.env.JWT_SECRET = 'test-secret-key-for-jwt-testing-only';
}

if (!process.env.JWT_EXPIRES_IN) {
  process.env.JWT_EXPIRES_IN = '1h';
}

if (!process.env.API_PREFIX) {
  process.env.API_PREFIX = '/api/v1';
}

// 延长测试超时时间
jest.setTimeout(30000);

// 全局测试钩子
beforeAll(async () => {
  console.log('🧪 开始集成测试...');
});

afterAll(async () => {
  console.log('✅ 集成测试完成');
  // 关闭数据库连接等清理工作
  // await closeDatabaseConnections();
});

// 每个测试前后的钩子
beforeEach(() => {
  // 清除所有mock
  jest.clearAllMocks();
});

afterEach(() => {
  // 清理工作
});
