/**
 * 玩家查询API集成测试
 * 测试角色查询、账号查询、装备查询等功能
 */

import {
  authGet,
  TEST_USERS,
  expectSuccess,
  expectPermissionDenied,
  expectUnauthorized,
  expectBadRequest,
  expectNotFound,
  unauthenticatedRequest,
  TestDataGenerator
} from './helpers';

const API_PREFIX = process.env.API_PREFIX || '/api/v1';

describe('玩家查询API集成测试', () => {
  describe('GET /players/characters/search', () => {
    it('应该成功查询角色（有player:read权限）', async () => {
      const response = await authGet(
        `${API_PREFIX}/players/characters/search?char_name=test`,
        TEST_USERS.normalGM
      );

      // 可能返回空列表，但应该成功
      expect([200, 404]).toContain(response.status);
      if (response.status === 200) {
        expectSuccess(response);
        expect(response.body.data).toHaveProperty('characters');
        expect(response.body.data).toHaveProperty('total');
        expect(response.body.data).toHaveProperty('page');
        expect(response.body.data).toHaveProperty('page_size');
        expect(Array.isArray(response.body.data.characters)).toBe(true);
      }
    });

    it('应该支持分页参数', async () => {
      const response = await authGet(
        `${API_PREFIX}/players/characters/search?char_name=test&page=1&page_size=10`,
        TEST_USERS.superAdmin
      );

      expect([200, 404]).toContain(response.status);
      if (response.status === 200) {
        expect(response.body.data.page).toBe(1);
        expect(response.body.data.page_size).toBe(10);
      }
    });

    it('应该支持多条件查询', async () => {
      const response = await authGet(
        `${API_PREFIX}/players/characters/search?char_name=test&level_min=1&level_max=100&job=0`,
        TEST_USERS.superAdmin
      );

      expect([200, 404]).toContain(response.status);
    });

    it('应该拒绝无权限的请求', async () => {
      const response = await unauthenticatedRequest('get', `${API_PREFIX}/players/characters/search?char_name=test`);

      expectUnauthorized(response);
    });

    it('应该验证分页参数范围', async () => {
      const response = await authGet(
        `${API_PREFIX}/players/characters/search?char_name=test&page_size=101`,
        TEST_USERS.superAdmin
      );

      // 应该自动限制为最大值或返回错误
      expect([200, 400, 404]).toContain(response.status);
    });
  });

  describe('GET /players/characters/:char_no', () => {
    it('应该成功查询存在的角色详情', async () => {
      // 注意：这里需要一个真实存在的char_no，或者mock数据库
      const testCharNo = 123456; // 替换为测试环境中存在的char_no

      const response = await authGet(
        `${API_PREFIX}/players/characters/${testCharNo}`,
        TEST_USERS.superAdmin
      );

      expect([200, 404]).toContain(response.status);
      if (response.status === 200) {
        expectSuccess(response);
        expect(response.body.data).toHaveProperty('char_no', testCharNo);
        expect(response.body.data).toHaveProperty('char_name');
        expect(response.body.data).toHaveProperty('job');
        expect(response.body.data).toHaveProperty('lev');
      }
    });

    it('应该返回404对于不存在的角色', async () => {
      const response = await authGet(
        `${API_PREFIX}/players/characters/999999999`,
        TEST_USERS.superAdmin
      );

      expectNotFound(response);
    });

    it('应该拒绝无效的char_no参数', async () => {
      const response = await authGet(
        `${API_PREFIX}/players/characters/invalid`,
        TEST_USERS.superAdmin
      );

      expectBadRequest(response);
    });
  });

  describe('GET /players/characters/:char_no/equipment', () => {
    it('应该成功查询角色装备', async () => {
      const testCharNo = 123456;

      const response = await authGet(
        `${API_PREFIX}/players/characters/${testCharNo}/equipment`,
        TEST_USERS.superAdmin
      );

      expect([200, 404]).toContain(response.status);
      if (response.status === 200) {
        expectSuccess(response);
        expect(Array.isArray(response.body.data)).toBe(true);
      }
    });
  });

  describe('GET /players/characters/:char_no/inventory', () => {
    it('应该成功查询角色背包', async () => {
      const testCharNo = 123456;

      const response = await authGet(
        `${API_PREFIX}/players/characters/${testCharNo}/inventory`,
        TEST_USERS.superAdmin
      );

      expect([200, 404]).toContain(response.status);
      if (response.status === 200) {
        expectSuccess(response);
        expect(response.body.data).toHaveProperty('items');
        expect(response.body.data).toHaveProperty('total');
        expect(response.body.data).toHaveProperty('page');
        expect(response.body.data).toHaveProperty('page_size');
      }
    });

    it('应该支持分页查询', async () => {
      const testCharNo = 123456;

      const response = await authGet(
        `${API_PREFIX}/players/characters/${testCharNo}/inventory?page=1&page_size=20`,
        TEST_USERS.superAdmin
      );

      expect([200, 404]).toContain(response.status);
      if (response.status === 200) {
        expect(response.body.data.page).toBe(1);
        expect(response.body.data.page_size).toBe(20);
      }
    });
  });

  describe('GET /players/characters/:char_no/warehouse', () => {
    it('应该成功查询角色仓库', async () => {
      const testCharNo = 123456;

      const response = await authGet(
        `${API_PREFIX}/players/characters/${testCharNo}/warehouse`,
        TEST_USERS.superAdmin
      );

      expect([200, 404]).toContain(response.status);
      if (response.status === 200) {
        expectSuccess(response);
        expect(response.body.data).toHaveProperty('items');
      }
    });
  });

  describe('GET /players/characters/:char_no/money', () => {
    it('应该成功查询角色金币', async () => {
      const testCharNo = 123456;

      const response = await authGet(
        `${API_PREFIX}/players/characters/${testCharNo}/money`,
        TEST_USERS.superAdmin
      );

      expect([200, 404]).toContain(response.status);
      if (response.status === 200) {
        expectSuccess(response);
        expect(response.body.data).toHaveProperty('gold');
        expect(response.body.data).toHaveProperty('silver');
      }
    });
  });

  describe('GET /players/characters/:char_no/online', () => {
    it('应该成功查询角色在线状态', async () => {
      const testCharNo = 123456;

      const response = await authGet(
        `${API_PREFIX}/players/characters/${testCharNo}/online`,
        TEST_USERS.superAdmin
      );

      expect([200, 404]).toContain(response.status);
      if (response.status === 200) {
        expectSuccess(response);
        expect(response.body.data).toHaveProperty('is_online');
        expect(typeof response.body.data.is_online).toBe('boolean');
      }
    });
  });

  describe('GET /players/accounts/search', () => {
    it('应该成功查询账号（有player:read:sensitive权限）', async () => {
      const response = await authGet(
        `${API_PREFIX}/players/accounts/search?m_id=test`,
        TEST_USERS.seniorGM
      );

      expect([200, 404]).toContain(response.status);
      if (response.status === 200) {
        expectSuccess(response);
        expect(response.body.data).toHaveProperty('m_id');
      }
    });

    it('应该拒绝无player:read:sensitive权限的请求', async () => {
      const response = await authGet(
        `${API_PREFIX}/players/accounts/search?m_id=test`,
        TEST_USERS.normalGM
      );

      expectPermissionDenied(response);
    });

    it('应该验证必需参数', async () => {
      const response = await authGet(
        `${API_PREFIX}/players/accounts/search`,
        TEST_USERS.superAdmin
      );

      expectBadRequest(response, 'MISSING_PARAMETERS');
    });
  });

  describe('GET /players/accounts/:m_id', () => {
    it('应该成功查询存在的账号详情', async () => {
      const testMId = 'test_account';

      const response = await authGet(
        `${API_PREFIX}/players/accounts/${testMId}`,
        TEST_USERS.seniorGM
      );

      expect([200, 404]).toContain(response.status);
      if (response.status === 200) {
        expectSuccess(response);
        expect(response.body.data).toHaveProperty('account_info');
        expect(response.body.data).toHaveProperty('character_list');
        expect(response.body.data).toHaveProperty('stats');
      }
    });

    it('应该拒绝无权限的请求', async () => {
      const response = await authGet(
        `${API_PREFIX}/players/accounts/test`,
        TEST_USERS.normalGM
      );

      expectPermissionDenied(response);
    });

    it('应该返回404对于不存在的账号', async () => {
      const response = await authGet(
        `${API_PREFIX}/players/accounts/nonexistent_account_${Date.now()}`,
        TEST_USERS.superAdmin
      );

      expectNotFound(response);
    });
  });

  describe('权限测试', () => {
    it('普通GM应该能查询角色基本信息', async () => {
      const response = await authGet(
        `${API_PREFIX}/players/characters/search?char_name=test`,
        TEST_USERS.normalGM
      );

      expect([200, 404]).toContain(response.status);
    });

    it('普通GM不应该能查询账号信息', async () => {
      const response = await authGet(
        `${API_PREFIX}/players/accounts/search?m_id=test`,
        TEST_USERS.normalGM
      );

      expectPermissionDenied(response);
    });

    it('高级GM应该能查询账号信息', async () => {
      const response = await authGet(
        `${API_PREFIX}/players/accounts/search?m_id=test`,
        TEST_USERS.seniorGM
      );

      expect([200, 404]).toContain(response.status);
    });

    it('超级管理员应该能访问所有查询接口', async () => {
      const endpoints = [
        `/players/characters/search?char_name=test`,
        `/players/accounts/search?m_id=test`
      ];

      for (const endpoint of endpoints) {
        const response = await authGet(
          `${API_PREFIX}${endpoint}`,
          TEST_USERS.superAdmin
        );

        expect([200, 404]).toContain(response.status);
      }
    });
  });
});
