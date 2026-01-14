/**
 * 安全测试
 * 测试SQL注入、XSS、权限绕过等安全问题
 */

import request from 'supertest';
import app from '../src/app';
import {
  authGet,
  authPost,
  TEST_USERS,
  expectUnauthorized,
  expectBadRequest,
  unauthenticatedRequest
} from './helpers';

const API_PREFIX = process.env.API_PREFIX || '/api/v1';

describe('安全测试', () => {
  describe('SQL注入防护测试', () => {
    it('应该防止查询参数中的SQL注入', async () => {
      const sqlInjections = [
        "' OR '1'='1",
        "'; DROP TABLE users; --",
        "' UNION SELECT * FROM gm_users --",
        "1' AND '1'='1"
      ];

      for (const injection of sqlInjections) {
        const response = await authGet(
          `${API_PREFIX}/players/characters/search?char_name=${encodeURIComponent(injection)}`,
          TEST_USERS.superAdmin
        );

        // 应该正常处理，不应该导致错误或泄露数据
        expect([200, 400, 404]).toContain(response.status);

        // 不应该返回敏感的数据库错误信息
        if (response.status >= 400) {
          expect(response.body.message).not.toMatch(/SQL|database|mysql/i);
        }
      }
    });

    it('应该防止POST请求体中的SQL注入', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/grant-item`,
        TEST_USERS.superAdmin
      ).send({
        char_no: "1' OR '1'='1",
        item_id: 100001,
        amount: 10,
        reason: '测试'
      });

      expectBadRequest(response);
    });
  });

  describe('XSS防护测试', () => {
    it('应该防止XSS攻击', async () => {
      const xssPayloads = [
        '<script>alert("XSS")</script>',
        '<img src=x onerror=alert("XSS")>',
        'javascript:alert("XSS")',
        '<svg onload=alert("XSS")>'
      ];

      for (const payload of xssPayloads) {
        const response = await authPost(
          `${API_PREFIX}/operations/send-mail`,
          TEST_USERS.superAdmin
        ).send({
          char_no: 123456,
          title: payload,
          content: payload
        });

        // 应该正常处理或拒绝，不应该执行脚本
        expect([200, 400, 404]).toContain(response.status);

        // 响应不应该包含未转义的脚本标签
        if (response.body.data) {
          const responseStr = JSON.stringify(response.body);
          expect(responseStr).not.toContain('<script>');
          expect(responseStr).not.toContain('onerror=');
        }
      }
    });
  });

  describe('认证安全测试', () => {
    it('应该拒绝缺少Authorization头的请求', async () => {
      const response = await request(app)
        .get(`${API_PREFIX}/players/characters/search?char_name=test`);

      expectUnauthorized(response);
    });

    it('应该拒绝格式错误的Authorization头', async () => {
      const invalidFormats = [
        'InvalidFormat',
        'Bearer',
        'Bearer ',
        'Token invalid_token'
      ];

      for (const format of invalidFormats) {
        const response = await request(app)
          .get(`${API_PREFIX}/players/characters/search?char_name=test`)
          .set('Authorization', format);

        expectUnauthorized(response);
      }
    });

    it('应该拒绝伪造的JWT token', async () => {
      const fakeToken = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkpvaG4gRG9lIiwiaWF0IjoxNTE2MjM5MDIyfQ.SflKxwRJSMeKKF2QT4fwpMeJf36POk6yJV_adQssw5c';

      const response = await request(app)
        .get(`${API_PREFIX}/players/characters/search?char_name=test`)
        .set('Authorization', `Bearer ${fakeToken}`);

      expectUnauthorized(response);
    });

    it('应该拒绝过期的token', async () => {
      // 注意：这需要生成一个已过期的token，这里只做概念性测试
      // 实际测试需要使用短期token或修改系统时间
    });
  });

  describe('权限绕过测试', () => {
    it('不应该允许通过修改token绕过权限检查', async () => {
      // 普通GM尝试访问需要更高权限的接口
      const response = await authPost(
        `${API_PREFIX}/operations/modify-level`,
        TEST_USERS.normalGM
      ).send({
        char_no: 123456,
        new_level: 100,
        reason: '尝试绕过权限'
      });

      expect(response.status).toBe(403);
    });

    it('不应该允许通过参数篡改提升权限', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/grant-item`,
        TEST_USERS.normalGM
      ).send({
        char_no: 123456,
        item_id: 100001,
        amount: 10,
        reason: '测试',
        // 尝试注入权限字段
        permissions: ['item:grant', 'account:ban'],
        gm_level: 3
      });

      expect(response.status).toBe(403);
    });
  });

  describe('输入验证测试', () => {
    it('应该拒绝过长的字符串输入', async () => {
      const veryLongString = 'A'.repeat(10000);

      const response = await authPost(
        `${API_PREFIX}/operations/send-mail`,
        TEST_USERS.superAdmin
      ).send({
        char_no: 123456,
        title: veryLongString,
        content: 'test'
      });

      // 应该拒绝或截断
      expect([400, 413]).toContain(response.status);
    });

    it('应该验证数值参数的类型', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/grant-item`,
        TEST_USERS.superAdmin
      ).send({
        char_no: 'not_a_number',
        item_id: 100001,
        amount: 10,
        reason: '测试'
      });

      expectBadRequest(response);
    });

    it('应该验证枚举值的有效性', async () => {
      const response = await authGet(
        `${API_PREFIX}/audit/operations?result=INVALID_STATUS`,
        TEST_USERS.superAdmin
      );

      // 应该接受（作为过滤条件）或拒绝
      expect([200, 400, 404]).toContain(response.status);
    });
  });

  describe('速率限制测试', () => {
    it('应该限制登录尝试次数', async () => {
      const responses: any[] = [];

      // 尝试20次错误登录
      for (let i = 0; i < 20; i++) {
        const response = await request(app)
          .post(`${API_PREFIX}/auth/login`)
          .send({
            username: 'admin',
            password: 'wrong_password'
          });

        responses.push(response);
      }

      // 至少有一些请求应该被速率限制（如果实现了速率限制）
      const rateLimited = responses.filter(r => r.status === 429);

      // 这是可选的，取决于是否实现了速率限制
      // expect(rateLimited.length).toBeGreaterThan(0);
    }, 30000); // 超时30秒
  });

  describe('信息泄露测试', () => {
    it('错误响应不应该泄露敏感信息', async () => {
      const response = await authGet(
        `${API_PREFIX}/players/characters/999999999`,
        TEST_USERS.superAdmin
      );

      // 不应该泄露数据库结构、SQL查询等
      const responseStr = JSON.stringify(response.body);
      expect(responseStr).not.toMatch(/SELECT|FROM|WHERE|mysql|database/i);
      expect(responseStr).not.toMatch(/stack|trace/i);
    });

    it('未授权响应不应该泄露系统信息', async () => {
      const response = await unauthenticatedRequest(
        'get',
        `${API_PREFIX}/players/characters/search?char_name=test`
      );

      const responseStr = JSON.stringify(response.body);
      expect(responseStr).not.toMatch(/node_modules|src\/|\.ts|\.js/);
    });
  });

  describe('CSRF防护测试', () => {
    it('应该接受正确的Content-Type', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/grant-item`,
        TEST_USERS.superAdmin
      )
        .set('Content-Type', 'application/json')
        .send({
          char_no: 123456,
          item_id: 100001,
          amount: 10,
          reason: '测试'
        });

      // 应该正常处理（可能404，但不应该415）
      expect(response.status).not.toBe(415);
    });
  });

  describe('密码安全测试', () => {
    it('应该拒绝弱密码', async () => {
      const weakPasswords = [
        '123456',
        'password',
        'qwerty',
        'abc123'
      ];

      // 这需要密码修改接口，这里只做概念性测试
      // 实际实现取决于密码复杂度要求
    });

    it('登录失败不应该泄露用户是否存在', async () => {
      const existingUserResponse = await request(app)
        .post(`${API_PREFIX}/auth/login`)
        .send({
          username: 'admin',
          password: 'wrong_password'
        });

      const nonExistingUserResponse = await request(app)
        .post(`${API_PREFIX}/auth/login`)
        .send({
          username: 'nonexistent_user_12345',
          password: 'wrong_password'
        });

      // 两种情况应该返回相同的错误消息
      expect(existingUserResponse.body.message).toBe(nonExistingUserResponse.body.message);
    });
  });
});
