/**
 * GM操作API集成测试
 * 测试物品赠送、邮件发送、账号封禁等功能
 */

import {
  authPost,
  TEST_USERS,
  expectSuccess,
  expectPermissionDenied,
  expectBadRequest,
  expectNotFound,
  TestDataGenerator,
  TestDataCleaner
} from './helpers';

const API_PREFIX = process.env.API_PREFIX || '/api/v1';

describe('GM操作API集成测试', () => {
  const cleaner = new TestDataCleaner();

  afterAll(async () => {
    await cleaner.cleanup();
  });

  describe('POST /operations/grant-item', () => {
    it('应该拒绝无item:grant权限的请求', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/grant-item`,
        TEST_USERS.normalGM
      ).send({
        char_no: 123456,
        item_id: 100001,
        amount: 10,
        reason: '测试赠送'
      });

      expectPermissionDenied(response);
    });

    it('应该验证必需参数', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/grant-item`,
        TEST_USERS.superAdmin
      ).send({
        char_no: 123456,
        item_id: 100001
        // 缺少amount和reason
      });

      expectBadRequest(response, 'MISSING_PARAMETERS');
    });

    it('应该验证char_no为正整数', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/grant-item`,
        TEST_USERS.superAdmin
      ).send({
        char_no: -1,
        item_id: 100001,
        amount: 10,
        reason: '测试'
      });

      expectBadRequest(response, 'INVALID_CHAR_NO');
    });

    it('应该验证amount范围', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/grant-item`,
        TEST_USERS.superAdmin
      ).send({
        char_no: 123456,
        item_id: 100001,
        amount: 0,
        reason: '测试'
      });

      expectBadRequest(response, 'INVALID_AMOUNT');
    });

    // 注意：成功赠送的测试需要真实的数据库环境
  });

  describe('POST /operations/send-mail', () => {
    it('应该拒绝无权限的请求', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/send-mail`,
        TEST_USERS.normalGM
      ).send({
        char_no: 123456,
        title: '测试邮件',
        content: '这是测试内容'
      });

      expectPermissionDenied(response);
    });

    it('应该验证必需参数', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/send-mail`,
        TEST_USERS.superAdmin
      ).send({
        char_no: 123456,
        title: '测试邮件'
        // 缺少content
      });

      expectBadRequest(response, 'MISSING_PARAMETERS');
    });

    it('应该验证标题不为空', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/send-mail`,
        TEST_USERS.superAdmin
      ).send({
        char_no: 123456,
        title: '',
        content: '内容'
      });

      expectBadRequest(response, 'INVALID_TITLE');
    });
  });

  describe('POST /operations/ban-account', () => {
    it('应该拒绝无account:ban权限的请求', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/ban-account`,
        TEST_USERS.normalGM
      ).send({
        m_id: 'test_user',
        ban_type: 1,
        ban_duration: 86400,
        reason: '测试封禁'
      });

      expectPermissionDenied(response);
    });

    it('应该验证必需参数', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/ban-account`,
        TEST_USERS.seniorGM
      ).send({
        m_id: 'test_user',
        ban_type: 1
        // 缺少ban_duration和reason
      });

      expectBadRequest(response, 'MISSING_PARAMETERS');
    });

    it('应该验证ban_type为整数', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/ban-account`,
        TEST_USERS.seniorGM
      ).send({
        m_id: 'test_user',
        ban_type: 'invalid',
        ban_duration: 86400,
        reason: '测试'
      });

      expectBadRequest(response, 'INVALID_BAN_TYPE');
    });

    it('应该验证ban_duration为非负整数', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/ban-account`,
        TEST_USERS.seniorGM
      ).send({
        m_id: 'test_user',
        ban_type: 1,
        ban_duration: -1,
        reason: '测试'
      });

      expectBadRequest(response, 'INVALID_BAN_DURATION');
    });
  });

  describe('POST /operations/unban-account', () => {
    it('应该拒绝无account:unban权限的请求', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/unban-account`,
        TEST_USERS.normalGM
      ).send({
        m_id: 'test_user',
        reason: '测试解封'
      });

      expectPermissionDenied(response);
    });

    it('应该验证必需参数', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/unban-account`,
        TEST_USERS.seniorGM
      ).send({
        m_id: 'test_user'
        // 缺少reason
      });

      expectBadRequest(response, 'MISSING_PARAMETERS');
    });
  });

  describe('POST /operations/modify-level', () => {
    it('应该拒绝无player:modify权限的请求', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/modify-level`,
        TEST_USERS.seniorGM
      ).send({
        char_no: 123456,
        new_level: 85,
        reason: '测试修改'
      });

      expectPermissionDenied(response);
    });

    it('应该验证必需参数', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/modify-level`,
        TEST_USERS.superAdmin
      ).send({
        char_no: 123456
        // 缺少new_level和reason
      });

      expectBadRequest(response, 'MISSING_PARAMETERS');
    });

    it('应该验证等级范围', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/modify-level`,
        TEST_USERS.superAdmin
      ).send({
        char_no: 123456,
        new_level: 0,
        reason: '测试'
      });

      expectBadRequest(response, 'INVALID_LEVEL');
    });
  });

  describe('POST /operations/modify-money', () => {
    it('应该拒绝无权限的请求', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/modify-money`,
        TEST_USERS.seniorGM
      ).send({
        char_no: 123456,
        gold: 1000000,
        reason: '测试修改'
      });

      expectPermissionDenied(response);
    });

    it('应该验证必需参数', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/modify-money`,
        TEST_USERS.superAdmin
      ).send({
        char_no: 123456
        // 缺少gold/silver和reason
      });

      expectBadRequest(response, 'MISSING_PARAMETERS');
    });

    it('应该要求至少提供gold或silver之一', async () => {
      const response = await authPost(
        `${API_PREFIX}/operations/modify-money`,
        TEST_USERS.superAdmin
      ).send({
        char_no: 123456,
        reason: '测试'
        // 既没有gold也没有silver
      });

      expectBadRequest(response, 'MISSING_PARAMETERS');
    });
  });

  describe('权限测试', () => {
    it('普通GM不应该能执行任何GM操作', async () => {
      const operations = [
        { url: '/operations/grant-item', body: { char_no: 1, item_id: 1, amount: 1, reason: 'test' } },
        { url: '/operations/send-mail', body: { char_no: 1, title: 'test', content: 'test' } },
        { url: '/operations/ban-account', body: { m_id: 'test', ban_type: 1, ban_duration: 0, reason: 'test' } },
        { url: '/operations/unban-account', body: { m_id: 'test', reason: 'test' } },
        { url: '/operations/modify-level', body: { char_no: 1, new_level: 1, reason: 'test' } },
        { url: '/operations/modify-money', body: { char_no: 1, gold: 1, reason: 'test' } }
      ];

      for (const op of operations) {
        const response = await authPost(
          `${API_PREFIX}${op.url}`,
          TEST_USERS.normalGM
        ).send(op.body);

        expectPermissionDenied(response);
      }
    });

    it('高级GM应该能执行封禁操作但不能修改玩家数据', async () => {
      // 能执行的操作
      const allowed = [
        { url: '/operations/grant-item', body: { char_no: 1, item_id: 1, amount: 1, reason: 'test' } },
        { url: '/operations/ban-account', body: { m_id: 'test', ban_type: 1, ban_duration: 0, reason: 'test' } }
      ];

      for (const op of allowed) {
        const response = await authPost(
          `${API_PREFIX}${op.url}`,
          TEST_USERS.seniorGM
        ).send(op.body);

        // 应该不是权限错误（可能是其他错误如NOT_FOUND）
        expect(response.status).not.toBe(403);
      }

      // 不能执行的操作
      const denied = [
        { url: '/operations/modify-level', body: { char_no: 1, new_level: 1, reason: 'test' } },
        { url: '/operations/modify-money', body: { char_no: 1, gold: 1, reason: 'test' } }
      ];

      for (const op of denied) {
        const response = await authPost(
          `${API_PREFIX}${op.url}`,
          TEST_USERS.seniorGM
        ).send(op.body);

        expectPermissionDenied(response);
      }
    });
  });
});
