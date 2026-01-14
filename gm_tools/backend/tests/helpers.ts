/**
 * 测试辅助工具
 * 提供通用的测试工具函数
 */

import request from 'supertest';
import jwt from 'jsonwebtoken';
import app from '../src/app';

/**
 * 测试用户接口
 */
export interface TestUser {
  gm_id: number;
  username: string;
  gm_level: number;
  permissions: string[];
}

/**
 * 预定义的测试用户
 */
export const TEST_USERS = {
  // 超级管理员
  superAdmin: {
    gm_id: 1,
    username: 'test_super_admin',
    gm_level: 3,
    permissions: [
      'player:read',
      'player:read:sensitive',
      'item:grant',
      'account:ban',
      'account:unban',
      'player:modify',
      'audit:read',
      'audit:export'
    ]
  },

  // 高级GM
  seniorGM: {
    gm_id: 2,
    username: 'test_senior_gm',
    gm_level: 2,
    permissions: [
      'player:read',
      'player:read:sensitive',
      'item:grant',
      'account:ban',
      'account:unban',
      'audit:read'
    ]
  },

  // 普通GM
  normalGM: {
    gm_id: 3,
    username: 'test_normal_gm',
    gm_level: 1,
    permissions: [
      'player:read'
    ]
  }
};

/**
 * 生成测试用的JWT Token
 */
export function generateTestToken(user: TestUser): string {
  const secret = process.env.JWT_SECRET || 'test-secret-key';
  const expiresIn = process.env.JWT_EXPIRES_IN || '1h';

  return jwt.sign(
    {
      gm_id: user.gm_id,
      username: user.username,
      gm_level: user.gm_level,
      permissions: user.permissions
    },
    secret,
    { expiresIn }
  );
}

/**
 * 创建带认证的请求
 */
export function authenticatedRequest(
  method: 'get' | 'post' | 'put' | 'delete',
  url: string,
  user: TestUser = TEST_USERS.superAdmin
) {
  const token = generateTestToken(user);
  return request(app)[method](url).set('Authorization', `Bearer ${token}`);
}

/**
 * GET请求（带认证）
 */
export function authGet(url: string, user: TestUser = TEST_USERS.superAdmin) {
  return authenticatedRequest('get', url, user);
}

/**
 * POST请求（带认证）
 */
export function authPost(url: string, user: TestUser = TEST_USERS.superAdmin) {
  return authenticatedRequest('post', url, user);
}

/**
 * PUT请求（带认证）
 */
export function authPut(url: string, user: TestUser = TEST_USERS.superAdmin) {
  return authenticatedRequest('put', url, user);
}

/**
 * DELETE请求（带认证）
 */
export function authDelete(url: string, user: TestUser = TEST_USERS.superAdmin) {
  return authenticatedRequest('delete', url, user);
}

/**
 * 未认证的请求
 */
export function unauthenticatedRequest(
  method: 'get' | 'post' | 'put' | 'delete',
  url: string
) {
  return request(app)[method](url);
}

/**
 * 生成随机字符串
 */
export function randomString(length: number = 10): string {
  const chars = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789';
  let result = '';
  for (let i = 0; i < length; i++) {
    result += chars.charAt(Math.floor(Math.random() * chars.length));
  }
  return result;
}

/**
 * 生成随机数字
 */
export function randomNumber(min: number = 1, max: number = 1000000): number {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

/**
 * 延迟执行
 */
export function delay(ms: number): Promise<void> {
  return new Promise(resolve => setTimeout(resolve, ms));
}

/**
 * 期望API成功响应
 */
export function expectSuccess(response: any, expectedCode: number = 200) {
  expect(response.status).toBe(expectedCode);
  expect(response.body).toHaveProperty('code', expectedCode);
  expect(response.body).toHaveProperty('message');
  expect(response.body).toHaveProperty('data');
}

/**
 * 期望API错误响应
 */
export function expectError(
  response: any,
  expectedStatus: number,
  expectedErrorCode?: string
) {
  expect(response.status).toBe(expectedStatus);
  expect(response.body).toHaveProperty('code', expectedStatus);
  expect(response.body).toHaveProperty('message');

  if (expectedErrorCode) {
    expect(response.body).toHaveProperty('error_code', expectedErrorCode);
  }
}

/**
 * 期望权限拒绝
 */
export function expectPermissionDenied(response: any) {
  expectError(response, 403, 'PERMISSION_DENIED');
}

/**
 * 期望未认证
 */
export function expectUnauthorized(response: any) {
  expectError(response, 401);
}

/**
 * 期望资源未找到
 */
export function expectNotFound(response: any) {
  expectError(response, 404);
}

/**
 * 期望参数错误
 */
export function expectBadRequest(response: any, errorCode?: string) {
  expectError(response, 400, errorCode);
}

/**
 * 测试数据清理器
 */
export class TestDataCleaner {
  private cleanupTasks: Array<() => Promise<void>> = [];

  /**
   * 添加清理任务
   */
  addTask(task: () => Promise<void>) {
    this.cleanupTasks.push(task);
  }

  /**
   * 执行所有清理任务
   */
  async cleanup() {
    for (const task of this.cleanupTasks.reverse()) {
      try {
        await task();
      } catch (error) {
        console.error('清理任务失败:', error);
      }
    }
    this.cleanupTasks = [];
  }
}

/**
 * 性能测试辅助
 */
export class PerformanceMonitor {
  private startTime: number = 0;

  start() {
    this.startTime = Date.now();
  }

  end(): number {
    return Date.now() - this.startTime;
  }

  expectUnder(maxMs: number) {
    const duration = this.end();
    expect(duration).toBeLessThan(maxMs);
    return duration;
  }
}

/**
 * 批量请求测试
 */
export async function concurrentRequests<T>(
  requestFn: () => Promise<T>,
  count: number
): Promise<T[]> {
  const promises: Promise<T>[] = [];
  for (let i = 0; i < count; i++) {
    promises.push(requestFn());
  }
  return Promise.all(promises);
}

/**
 * 测试数据生成器
 */
export const TestDataGenerator = {
  /**
   * 生成测试角色数据
   */
  character: () => ({
    char_no: randomNumber(100000, 999999),
    char_name: `测试角色_${randomString(6)}`,
    job: randomNumber(0, 10),
    lev: randomNumber(1, 100)
  }),

  /**
   * 生成测试账号数据
   */
  account: () => ({
    m_id: `test_${randomString(10)}`,
    username: `测试用户_${randomString(6)}`
  }),

  /**
   * 生成测试物品数据
   */
  item: () => ({
    item_id: randomNumber(100000, 199999),
    amount: randomNumber(1, 999)
  }),

  /**
   * 生成测试日期范围
   */
  dateRange: (daysAgo: number = 7) => {
    const endDate = new Date();
    const startDate = new Date();
    startDate.setDate(startDate.getDate() - daysAgo);

    return {
      start_date: startDate.toISOString(),
      end_date: endDate.toISOString()
    };
  }
};
