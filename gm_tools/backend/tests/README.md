# 集成测试文档

## 📋 概述

本目录包含DNF GM Tools后端API的完整集成测试套件，包括功能测试、性能测试和安全测试。

## 🗂️ 测试文件结构

```
tests/
├── setup.ts              # 测试环境设置
├── helpers.ts            # 测试辅助工具
├── auth.test.ts          # 认证API测试
├── player.test.ts        # 玩家查询API测试
├── operations.test.ts    # GM操作API测试
├── audit.test.ts         # 审计日志API测试
├── performance.test.ts   # 性能测试
├── security.test.ts      # 安全测试
└── README.md            # 本文档
```

## 🚀 运行测试

### 前置条件

1. 安装依赖:
```bash
npm install
```

2. 配置测试环境变量:
创建 `.env.test` 文件（参考 `.env.example`）

3. 确保测试数据库可访问

### 运行所有测试

```bash
npm test
```

### 运行特定测试文件

```bash
# 认证API测试
npm test -- tests/auth.test.ts

# 玩家查询API测试
npm test -- tests/player.test.ts

# GM操作API测试
npm test -- tests/operations.test.ts

# 审计日志API测试
npm test -- tests/audit.test.ts

# 性能测试
npm test -- tests/performance.test.ts

# 安全测试
npm test -- tests/security.test.ts
```

### 生成测试覆盖率报告

```bash
npm test -- --coverage
```

覆盖率报告将生成在 `coverage/` 目录。

### 监视模式

```bash
npm test -- --watch
```

## 📊 测试覆盖范围

### 1. 认证API测试 (auth.test.ts)

- ✅ 登录功能
- ✅ Token刷新
- ✅ 登出功能
- ✅ 密码修改
- ✅ 认证中间件

**测试用例数**: 15+

### 2. 玩家查询API测试 (player.test.ts)

- ✅ 角色搜索
- ✅ 角色详情查询
- ✅ 装备查询
- ✅ 背包查询
- ✅ 仓库查询
- ✅ 金币查询
- ✅ 在线状态查询
- ✅ 账号搜索
- ✅ 账号详情查询
- ✅ 权限控制

**测试用例数**: 20+

### 3. GM操作API测试 (operations.test.ts)

- ✅ 物品赠送
- ✅ 邮件发送
- ✅ 账号封禁
- ✅ 账号解封
- ✅ 等级修改
- ✅ 金币修改
- ✅ 参数验证
- ✅ 权限检查

**测试用例数**: 25+

### 4. 审计日志API测试 (audit.test.ts)

- ✅ 操作日志查询
- ✅ 登录日志查询
- ✅ 操作统计
- ✅ 登录统计
- ✅ 日志导出（JSON/CSV）
- ✅ 权限控制

**测试用例数**: 15+

### 5. 性能测试 (performance.test.ts)

- ✅ 响应时间测试
- ✅ 并发处理测试
- ✅ 分页性能测试
- ✅ 数据库查询优化测试
- ✅ 内存使用测试

**测试用例数**: 10+

### 6. 安全测试 (security.test.ts)

- ✅ SQL注入防护
- ✅ XSS防护
- ✅ 认证安全
- ✅ 权限绕过测试
- ✅ 输入验证
- ✅ 信息泄露测试
- ✅ CSRF防护
- ✅ 密码安全

**测试用例数**: 25+

**总测试用例数**: 110+

## 🛠️ 测试工具

### 辅助函数 (helpers.ts)

#### 认证请求
```typescript
import { authGet, authPost, TEST_USERS } from './helpers';

// GET请求（自动添加认证）
const response = await authGet('/api/v1/players/characters/search', TEST_USERS.superAdmin);

// POST请求（自动添加认证）
const response = await authPost('/api/v1/operations/grant-item', TEST_USERS.seniorGM)
  .send({ char_no: 123456, item_id: 100001, amount: 10, reason: '测试' });
```

#### 测试用户
```typescript
// 超级管理员（所有权限）
TEST_USERS.superAdmin

// 高级GM（大部分权限）
TEST_USERS.seniorGM

// 普通GM（基础权限）
TEST_USERS.normalGM
```

#### 期望断言
```typescript
import { expectSuccess, expectError, expectPermissionDenied } from './helpers';

// 期望成功响应
expectSuccess(response);

// 期望错误响应
expectError(response, 404, 'NOT_FOUND');

// 期望权限拒绝
expectPermissionDenied(response);
```

#### 性能监控
```typescript
import { PerformanceMonitor } from './helpers';

const monitor = new PerformanceMonitor();
monitor.start();
await someOperation();
monitor.expectUnder(1000); // 期望在1000ms以内
```

#### 并发测试
```typescript
import { concurrentRequests } from './helpers';

const responses = await concurrentRequests(
  () => authGet('/api/v1/audit/operations'),
  10 // 10个并发请求
);
```

## 📝 编写新测试

### 基本测试模板

```typescript
import { authGet, authPost, TEST_USERS, expectSuccess } from './helpers';

const API_PREFIX = process.env.API_PREFIX || '/api/v1';

describe('新功能测试', () => {
  it('应该成功执行操作', async () => {
    const response = await authPost(
      `${API_PREFIX}/your-endpoint`,
      TEST_USERS.superAdmin
    ).send({
      // 请求数据
    });

    expectSuccess(response);
    expect(response.body.data).toHaveProperty('expected_field');
  });

  it('应该拒绝无权限的请求', async () => {
    const response = await authPost(
      `${API_PREFIX}/your-endpoint`,
      TEST_USERS.normalGM
    ).send({
      // 请求数据
    });

    expect(response.status).toBe(403);
  });
});
```

## 🔍 调试测试

### 查看详细输出

```bash
npm test -- --verbose
```

### 只运行特定测试

```bash
npm test -- --testNamePattern="应该成功登录"
```

### 跳过某些测试

```typescript
it.skip('暂时跳过的测试', async () => {
  // 测试代码
});
```

### 只运行某个测试

```typescript
it.only('只运行这个测试', async () => {
  // 测试代码
});
```

## 📈 持续集成

测试可以集成到CI/CD流程中:

```yaml
# .github/workflows/test.yml
name: Tests
on: [push, pull_request]
jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - uses: actions/setup-node@v2
        with:
          node-version: '18'
      - run: npm install
      - run: npm test
      - run: npm run test:coverage
      - uses: codecov/codecov-action@v2
        with:
          files: ./coverage/lcov.info
```

## 🎯 最佳实践

### 1. 测试隔离
- 每个测试应该独立运行
- 使用 `beforeEach` 和 `afterEach` 进行清理

### 2. 描述性命名
- 测试名称应该清晰描述测试内容
- 使用"应该..."的格式

### 3. 单一职责
- 每个测试只验证一个功能点
- 避免过于复杂的测试

### 4. 使用mock
- 对外部依赖使用mock
- 避免依赖真实数据库状态

### 5. 测试数据管理
- 使用TestDataGenerator生成测试数据
- 使用TestDataCleaner清理测试数据

## 🐛 常见问题

### Q: 测试超时
A: 增加超时时间或优化测试代码
```typescript
it('测试', async () => {
  // 测试代码
}, 30000); // 30秒超时
```

### Q: 数据库连接失败
A: 检查 `.env.test` 中的数据库配置

### Q: 权限测试失败
A: 确保测试用户在数据库中存在并有正确的权限

### Q: 性能测试不稳定
A: 性能测试结果可能受环境影响，可以适当放宽阈值

## 📚 参考资料

- [Jest 官方文档](https://jestjs.io/)
- [Supertest 文档](https://github.com/visionmedia/supertest)
- [TypeScript Jest 配置](https://kulshekhar.github.io/ts-jest/)

## 🤝 贡献

添加新测试时:
1. 遵循现有的测试结构
2. 使用测试辅助工具
3. 添加充分的断言
4. 更新本README文档

## 📧 联系

如有问题，请联系开发团队或提交Issue。
