/**
 * Jest测试环境设置
 * 在所有测试之前运行
 */

import dotenv from 'dotenv';

// 加载测试环境变量
dotenv.config({ path: '.env.test' });

// 设置测试超时
jest.setTimeout(10000);

// Mock环境变量
process.env.NODE_ENV = 'test';
process.env.JWT_SECRET = 'test-jwt-secret-key-for-testing-only';
process.env.JWT_EXPIRATION = '1800';
process.env.JWT_REFRESH_EXPIRATION = '604800';
process.env.LOGIN_MAX_FAILED_ATTEMPTS = '5';
process.env.LOGIN_LOCKOUT_DURATION = '600';
process.env.TWO_FACTOR_REQUIRED_LEVEL = '3';
process.env.DEVICE_TRUST_DURATION = '2592000';

// 全局测试工具
global.console = {
  ...console,
  // 在测试期间抑制某些日志
  log: jest.fn(),
  debug: jest.fn(),
  info: jest.fn(),
  warn: jest.fn(),
  error: jest.fn()
};
