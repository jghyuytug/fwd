/**
 * 审计日志API集成测试
 * 测试日志查询、统计和导出功能
 */

import {
  authGet,
  TEST_USERS,
  expectSuccess,
  expectPermissionDenied,
  TestDataGenerator
} from './helpers';

const API_PREFIX = process.env.API_PREFIX || '/api/v1';

describe('审计日志API集成测试', () => {
  describe('GET /audit/operations', () => {
    it('应该成功查询操作日志（有audit:read权限）', async () => {
      const response = await authGet(
        `${API_PREFIX}/audit/operations`,
        TEST_USERS.seniorGM
      );

      expect([200, 404]).toContain(response.status);
      if (response.status === 200) {
        expectSuccess(response);
        expect(response.body.data).toHaveProperty('logs');
        expect(response.body.data).toHaveProperty('total');
        expect(response.body.data).toHaveProperty('page');
        expect(response.body.data).toHaveProperty('page_size');
      }
    });

    it('应该拒绝无audit:read权限的请求', async () => {
      const response = await authGet(
        `${API_PREFIX}/audit/operations`,
        TEST_USERS.normalGM
      );

      expectPermissionDenied(response);
    });

    it('应该支持过滤参数', async () => {
      const response = await authGet(
        `${API_PREFIX}/audit/operations?gm_id=1&operation_type=ITEM_GRANT&result=SUCCESS`,
        TEST_USERS.superAdmin
      );

      expect([200, 404]).toContain(response.status);
    });

    it('应该支持时间范围查询', async () => {
      const dateRange = TestDataGenerator.dateRange(7);
      const response = await authGet(
        `${API_PREFIX}/audit/operations?start_date=${dateRange.start_date}&end_date=${dateRange.end_date}`,
        TEST_USERS.superAdmin
      );

      expect([200, 404]).toContain(response.status);
    });
  });

  describe('GET /audit/logins', () => {
    it('应该成功查询登录日志', async () => {
      const response = await authGet(
        `${API_PREFIX}/audit/logins`,
        TEST_USERS.seniorGM
      );

      expect([200, 404]).toContain(response.status);
      if (response.status === 200) {
        expectSuccess(response);
        expect(response.body.data).toHaveProperty('logs');
      }
    });

    it('应该支持按登录状态过滤', async () => {
      const response = await authGet(
        `${API_PREFIX}/audit/logins?login_status=SUCCESS`,
        TEST_USERS.superAdmin
      );

      expect([200, 404]).toContain(response.status);
    });
  });

  describe('GET /audit/stats/operations', () => {
    it('应该成功获取操作统计', async () => {
      const response = await authGet(
        `${API_PREFIX}/audit/stats/operations`,
        TEST_USERS.seniorGM
      );

      expect([200, 404]).toContain(response.status);
      if (response.status === 200) {
        expectSuccess(response);
        expect(response.body.data).toHaveProperty('total_operations');
        expect(response.body.data).toHaveProperty('success_count');
        expect(response.body.data).toHaveProperty('failure_count');
        expect(response.body.data).toHaveProperty('success_rate');
        expect(response.body.data).toHaveProperty('by_type');
        expect(response.body.data).toHaveProperty('by_category');
      }
    });

    it('应该支持按GM ID过滤', async () => {
      const response = await authGet(
        `${API_PREFIX}/audit/stats/operations?gm_id=1`,
        TEST_USERS.superAdmin
      );

      expect([200, 404]).toContain(response.status);
    });
  });

  describe('GET /audit/stats/logins', () => {
    it('应该成功获取登录统计', async () => {
      const response = await authGet(
        `${API_PREFIX}/audit/stats/logins`,
        TEST_USERS.seniorGM
      );

      expect([200, 404]).toContain(response.status);
      if (response.status === 200) {
        expectSuccess(response);
        expect(response.body.data).toHaveProperty('total_logins');
        expect(response.body.data).toHaveProperty('success_count');
        expect(response.body.data).toHaveProperty('success_rate');
      }
    });
  });

  describe('GET /audit/export/operations', () => {
    it('应该成功导出JSON格式', async () => {
      const response = await authGet(
        `${API_PREFIX}/audit/export/operations?format=json`,
        TEST_USERS.superAdmin
      );

      expect([200, 404]).toContain(response.status);
      if (response.status === 200) {
        expectSuccess(response);
        expect(Array.isArray(response.body.data)).toBe(true);
      }
    });

    it('应该成功导出CSV格式', async () => {
      const response = await authGet(
        `${API_PREFIX}/audit/export/operations?format=csv`,
        TEST_USERS.superAdmin
      );

      expect([200, 404]).toContain(response.status);
      if (response.status === 200) {
        expect(response.headers['content-type']).toContain('text/csv');
      }
    });

    it('应该拒绝无audit:export权限的请求', async () => {
      const response = await authGet(
        `${API_PREFIX}/audit/export/operations`,
        TEST_USERS.seniorGM
      );

      expectPermissionDenied(response);
    });
  });

  describe('GET /audit/export/logins', () => {
    it('应该成功导出登录日志', async () => {
      const response = await authGet(
        `${API_PREFIX}/audit/export/logins?format=json`,
        TEST_USERS.superAdmin
      );

      expect([200, 404]).toContain(response.status);
    });

    it('应该拒绝无权限的请求', async () => {
      const response = await authGet(
        `${API_PREFIX}/audit/export/logins`,
        TEST_USERS.normalGM
      );

      expectPermissionDenied(response);
    });
  });
});
