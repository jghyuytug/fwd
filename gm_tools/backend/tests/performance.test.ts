/**
 * 性能测试
 * 测试API的响应时间和并发处理能力
 */

import {
  authGet,
  authPost,
  TEST_USERS,
  PerformanceMonitor,
  concurrentRequests
} from './helpers';

const API_PREFIX = process.env.API_PREFIX || '/api/v1';

describe('性能测试', () => {
  describe('响应时间测试', () => {
    it('登录应该在500ms内完成', async () => {
      const monitor = new PerformanceMonitor();
      monitor.start();

      await authPost(`${API_PREFIX}/auth/login`, TEST_USERS.superAdmin)
        .send({
          username: 'admin',
          password: 'Admin123!@#'
        });

      monitor.expectUnder(500);
    });

    it('角色查询应该在1000ms内完成', async () => {
      const monitor = new PerformanceMonitor();
      monitor.start();

      await authGet(
        `${API_PREFIX}/players/characters/search?char_name=test`,
        TEST_USERS.superAdmin
      );

      monitor.expectUnder(1000);
    });

    it('操作日志查询应该在1000ms内完成', async () => {
      const monitor = new PerformanceMonitor();
      monitor.start();

      await authGet(
        `${API_PREFIX}/audit/operations?page=1&page_size=50`,
        TEST_USERS.superAdmin
      );

      monitor.expectUnder(1000);
    });

    it('统计查询应该在2000ms内完成', async () => {
      const monitor = new PerformanceMonitor();
      monitor.start();

      await authGet(
        `${API_PREFIX}/audit/stats/operations`,
        TEST_USERS.superAdmin
      );

      monitor.expectUnder(2000);
    });
  });

  describe('并发处理测试', () => {
    it('应该能处理10个并发的角色查询请求', async () => {
      const monitor = new PerformanceMonitor();
      monitor.start();

      const responses = await concurrentRequests(
        () => authGet(
          `${API_PREFIX}/players/characters/search?char_name=test`,
          TEST_USERS.superAdmin
        ),
        10
      );

      const duration = monitor.end();

      // 所有请求都应该成功或返回404
      responses.forEach(response => {
        expect([200, 404]).toContain(response.status);
      });

      // 10个请求应该在3秒内完成
      expect(duration).toBeLessThan(3000);
    });

    it('应该能处理5个并发的统计请求', async () => {
      const monitor = new PerformanceMonitor();
      monitor.start();

      const responses = await concurrentRequests(
        () => authGet(
          `${API_PREFIX}/audit/stats/operations`,
          TEST_USERS.superAdmin
        ),
        5
      );

      const duration = monitor.end();

      responses.forEach(response => {
        expect([200, 404]).toContain(response.status);
      });

      expect(duration).toBeLessThan(5000);
    });
  });

  describe('分页性能测试', () => {
    it('小分页大小应该更快', async () => {
      const smallPageMonitor = new PerformanceMonitor();
      smallPageMonitor.start();

      await authGet(
        `${API_PREFIX}/audit/operations?page=1&page_size=10`,
        TEST_USERS.superAdmin
      );

      const smallPageDuration = smallPageMonitor.end();

      const largePageMonitor = new PerformanceMonitor();
      largePageMonitor.start();

      await authGet(
        `${API_PREFIX}/audit/operations?page=1&page_size=100`,
        TEST_USERS.superAdmin
      );

      const largePageDuration = largePageMonitor.end();

      // 小分页应该更快或相近（允许有一定波动）
      expect(smallPageDuration).toBeLessThanOrEqual(largePageDuration * 1.5);
    });
  });

  describe('数据库查询优化测试', () => {
    it('带索引字段查询应该快速', async () => {
      const monitor = new PerformanceMonitor();
      monitor.start();

      // gm_id应该有索引
      await authGet(
        `${API_PREFIX}/audit/operations?gm_id=1`,
        TEST_USERS.superAdmin
      );

      monitor.expectUnder(1000);
    });

    it('时间范围查询应该快速', async () => {
      const monitor = new PerformanceMonitor();
      monitor.start();

      const endDate = new Date();
      const startDate = new Date();
      startDate.setDate(startDate.getDate() - 7);

      await authGet(
        `${API_PREFIX}/audit/operations?start_date=${startDate.toISOString()}&end_date=${endDate.toISOString()}`,
        TEST_USERS.superAdmin
      );

      monitor.expectUnder(1500);
    });
  });

  describe('内存使用测试', () => {
    it('大量请求不应该导致内存泄漏', async () => {
      const initialMemory = process.memoryUsage().heapUsed;

      // 执行100个请求
      for (let i = 0; i < 100; i++) {
        await authGet(
          `${API_PREFIX}/audit/operations?page=1&page_size=10`,
          TEST_USERS.superAdmin
        );
      }

      // 手动触发垃圾回收（如果可用）
      if (global.gc) {
        global.gc();
      }

      const finalMemory = process.memoryUsage().heapUsed;
      const memoryIncrease = finalMemory - initialMemory;

      // 内存增长不应超过50MB
      expect(memoryIncrease).toBeLessThan(50 * 1024 * 1024);
    }, 60000); // 超时时间60秒
  });
});
