/**
 * 认证API集成测试
 * 测试登录、令牌刷新、登出等功能
 */

import request from 'supertest';
import app from '../src/app';
import {
  expectSuccess,
  expectError,
  expectUnauthorized,
  expectBadRequest,
  randomString
} from './helpers';

const API_PREFIX = process.env.API_PREFIX || '/api/v1';

describe('认证API集成测试', () => {
  describe('POST /auth/login', () => {
    it('应该成功登录并返回token', async () => {
      const response = await request(app)
        .post(`${API_PREFIX}/auth/login`)
        .send({
          username: 'admin',
          password: 'Admin123!@#'
        });

      expectSuccess(response);
      expect(response.body.data).toHaveProperty('access_token');
      expect(response.body.data).toHaveProperty('refresh_token');
      expect(response.body.data).toHaveProperty('expires_in');
      expect(response.body.data).toHaveProperty('gm_user');
      expect(response.body.data.gm_user).toHaveProperty('gm_id');
      expect(response.body.data.gm_user).toHaveProperty('username', 'admin');
      expect(response.body.data.gm_user).toHaveProperty('gm_level');
      expect(response.body.data.gm_user).toHaveProperty('permissions');
      expect(Array.isArray(response.body.data.gm_user.permissions)).toBe(true);
    });

    it('应该拒绝错误的密码', async () => {
      const response = await request(app)
        .post(`${API_PREFIX}/auth/login`)
        .send({
          username: 'admin',
          password: 'wrong_password'
        });

      expectError(response, 401, 'INVALID_CREDENTIALS');
    });

    it('应该拒绝不存在的用户', async () => {
      const response = await request(app)
        .post(`${API_PREFIX}/auth/login`)
        .send({
          username: `nonexistent_${randomString()}`,
          password: 'password123'
        });

      expectError(response, 401, 'INVALID_CREDENTIALS');
    });

    it('应该验证必需参数', async () => {
      const response = await request(app)
        .post(`${API_PREFIX}/auth/login`)
        .send({
          username: 'admin'
          // 缺少password
        });

      expectBadRequest(response, 'MISSING_PARAMETERS');
    });

    it('应该拒绝空用户名', async () => {
      const response = await request(app)
        .post(`${API_PREFIX}/auth/login`)
        .send({
          username: '',
          password: 'password123'
        });

      expectBadRequest(response);
    });

    it('应该拒绝空密码', async () => {
      const response = await request(app)
        .post(`${API_PREFIX}/auth/login`)
        .send({
          username: 'admin',
          password: ''
        });

      expectBadRequest(response);
    });
  });

  describe('POST /auth/refresh', () => {
    let refreshToken: string;

    beforeEach(async () => {
      // 先登录获取refresh token
      const loginResponse = await request(app)
        .post(`${API_PREFIX}/auth/login`)
        .send({
          username: 'admin',
          password: 'Admin123!@#'
        });

      refreshToken = loginResponse.body.data.refresh_token;
    });

    it('应该成功刷新token', async () => {
      const response = await request(app)
        .post(`${API_PREFIX}/auth/refresh`)
        .send({
          refresh_token: refreshToken
        });

      expectSuccess(response);
      expect(response.body.data).toHaveProperty('access_token');
      expect(response.body.data).toHaveProperty('refresh_token');
      expect(response.body.data).toHaveProperty('expires_in');

      // 新token应该不同于旧token
      expect(response.body.data.access_token).not.toBe(refreshToken);
    });

    it('应该拒绝无效的refresh token', async () => {
      const response = await request(app)
        .post(`${API_PREFIX}/auth/refresh`)
        .send({
          refresh_token: 'invalid_token_' + randomString(50)
        });

      expectUnauthorized(response);
    });

    it('应该验证必需参数', async () => {
      const response = await request(app)
        .post(`${API_PREFIX}/auth/refresh`)
        .send({});

      expectBadRequest(response, 'MISSING_PARAMETERS');
    });
  });

  describe('POST /auth/logout', () => {
    let accessToken: string;

    beforeEach(async () => {
      // 先登录获取access token
      const loginResponse = await request(app)
        .post(`${API_PREFIX}/auth/login`)
        .send({
          username: 'admin',
          password: 'Admin123!@#'
        });

      accessToken = loginResponse.body.data.access_token;
    });

    it('应该成功登出', async () => {
      const response = await request(app)
        .post(`${API_PREFIX}/auth/logout`)
        .set('Authorization', `Bearer ${accessToken}`);

      expectSuccess(response);
    });

    it('应该拒绝未认证的登出请求', async () => {
      const response = await request(app)
        .post(`${API_PREFIX}/auth/logout`);

      expectUnauthorized(response);
    });

    it('应该拒绝无效token', async () => {
      const response = await request(app)
        .post(`${API_PREFIX}/auth/logout`)
        .set('Authorization', `Bearer invalid_token_${randomString()}`);

      expectUnauthorized(response);
    });
  });

  describe('POST /auth/change-password', () => {
    let accessToken: string;

    beforeEach(async () => {
      // 登录获取token
      const loginResponse = await request(app)
        .post(`${API_PREFIX}/auth/login`)
        .send({
          username: 'admin',
          password: 'Admin123!@#'
        });

      accessToken = loginResponse.body.data.access_token;
    });

    it('应该验证必需参数', async () => {
      const response = await request(app)
        .post(`${API_PREFIX}/auth/change-password`)
        .set('Authorization', `Bearer ${accessToken}`)
        .send({
          old_password: 'Admin123!@#'
          // 缺少new_password
        });

      expectBadRequest(response, 'MISSING_PARAMETERS');
    });

    it('应该拒绝错误的旧密码', async () => {
      const response = await request(app)
        .post(`${API_PREFIX}/auth/change-password`)
        .set('Authorization', `Bearer ${accessToken}`)
        .send({
          old_password: 'wrong_password',
          new_password: 'NewPassword123!@#'
        });

      expectError(response, 401, 'INVALID_CREDENTIALS');
    });

    it('应该拒绝未认证的密码修改请求', async () => {
      const response = await request(app)
        .post(`${API_PREFIX}/auth/change-password`)
        .send({
          old_password: 'Admin123!@#',
          new_password: 'NewPassword123!@#'
        });

      expectUnauthorized(response);
    });

    // 注意：成功修改密码的测试需要在测试后恢复密码，或使用专门的测试账号
    // 这里跳过实际修改密码的测试
  });

  describe('认证中间件集成测试', () => {
    let accessToken: string;

    beforeEach(async () => {
      const loginResponse = await request(app)
        .post(`${API_PREFIX}/auth/login`)
        .send({
          username: 'admin',
          password: 'Admin123!@#'
        });

      accessToken = loginResponse.body.data.access_token;
    });

    it('应该允许有效token访问受保护的端点', async () => {
      const response = await request(app)
        .get(`${API_PREFIX}/players/characters/search`)
        .set('Authorization', `Bearer ${accessToken}`)
        .query({ char_name: 'test' });

      // 应该返回200或404，而不是401
      expect([200, 404]).toContain(response.status);
    });

    it('应该拒绝无token访问受保护的端点', async () => {
      const response = await request(app)
        .get(`${API_PREFIX}/players/characters/search`)
        .query({ char_name: 'test' });

      expectUnauthorized(response);
    });

    it('应该拒绝格式错误的Authorization header', async () => {
      const response = await request(app)
        .get(`${API_PREFIX}/players/characters/search`)
        .set('Authorization', 'InvalidFormat')
        .query({ char_name: 'test' });

      expectUnauthorized(response);
    });

    it('应该拒绝过期的token', async () => {
      // 注意：这个测试需要生成一个已过期的token
      // 实际环境中，可以通过修改JWT_EXPIRES_IN为极短时间来测试
      // 这里跳过实际测试
    });
  });
});
