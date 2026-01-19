# DNF GM工具系统 - 测试文档

## 概述

本项目使用 **Jest** 和 **ts-jest** 进行单元测试，测试覆盖了认证系统的所有核心组件。

## 测试结构

```
backend/src/
├── utils/
│   └── __tests__/
│       ├── crypto.util.test.ts      # 加密工具测试（密码哈希、Token生成）
│       ├── jwt.util.test.ts         # JWT工具测试（Token生成与验证）
│       └── twoFactor.util.test.ts   # 2FA工具测试（TOTP、QR码、备份码）
├── models/
│   └── __tests__/
│       └── gmAccount.model.test.ts  # GM账号模型测试（数据库操作）
├── services/
│   └── __tests__/
│       └── auth.service.test.ts     # 认证服务测试（业务逻辑）
└── controllers/
    └── __tests__/
        └── auth.controller.test.ts  # 认证控制器测试（HTTP处理）
```

## 测试配置文件

### jest.config.js
Jest主配置文件，定义：
- 测试环境：Node.js
- 测试文件匹配模式：`**/__tests__/**/*.test.ts`
- 代码覆盖率配置
- TypeScript支持（ts-jest）

### .env.test
测试专用环境变量配置，包含：
- JWT密钥和过期时间
- 登录安全配置（最大失败次数、锁定时长）
- 2FA配置
- 测试数据库连接信息

### src/__tests__/setup.ts
全局测试设置文件，在所有测试前运行：
- 加载测试环境变量
- Mock全局console（抑制测试期间的日志输出）
- 设置测试超时时间

## 测试命令

### 基本命令

```bash
# 运行所有测试
npm test

# 运行测试并生成覆盖率报告
npm run test

# 监听模式（开发时使用）
npm run test:watch

# 只运行单元测试
npm run test:unit

# 详细输出模式
npm run test:verbose
```

### 运行特定测试文件

```bash
# 运行crypto工具测试
npx jest crypto.util.test.ts

# 运行认证服务测试
npx jest auth.service.test.ts

# 运行所有utils测试
npx jest utils/
```

## 测试覆盖范围

### 1. crypto.util.test.ts（加密工具）

**测试内容：**
- ✅ 密码哈希（bcrypt）
  - 成功哈希密码
  - 相同密码生成不同哈希（盐值）
  - 空密码错误处理
- ✅ 密码验证
  - 验证正确密码
  - 拒绝错误密码
  - 空密码处理
- ✅ 随机Token生成
  - 指定长度生成
  - Token唯一性
  - 十六进制格式验证
- ✅ 设备Token生成
  - 固定长度（64字符）
  - 唯一性保证
- ✅ SHA256哈希
  - 正确性验证
  - 相同输入产生相同输出
  - 空字符串和中文字符处理
- ✅ 设备指纹验证
  - 有效格式验证
  - 无效格式拒绝
  - 边界条件测试

**测试用例数：** 28个

### 2. jwt.util.test.ts（JWT工具）

**测试内容：**
- ✅ Access Token生成
  - 有效Token格式
  - Payload完整性
  - 过期时间设置
- ✅ Refresh Token生成
  - 有效Token格式
  - 不包含permissions字段
  - 长过期时间
- ✅ Token验证
  - 验证有效Token
  - 拒绝无效Token
  - 拒绝过期Token
  - 拒绝签名错误Token
  - 格式错误处理
- ✅ Token类型区分
  - Access vs Refresh类型标识
  - 类型互斥性验证

**测试用例数：** 17个

### 3. twoFactor.util.test.ts（2FA工具）

**测试内容：**
- ✅ 2FA密钥生成
  - 有效密钥格式（Base32）
  - otpauthUrl正确性
  - 密钥唯一性
- ✅ QR码生成
  - Base64编码验证
  - 不同URL生成不同QR码
- ✅ TOTP代码验证
  - 验证有效代码
  - 拒绝无效代码
  - 时间窗口容错（±1步）
  - 格式错误处理
- ✅ 备份代码生成
  - 数量正确
  - 8位字符格式
  - 只包含大写字母和数字
  - 唯一性保证
  - 排除易混淆字符（0, O, I, 1）
- ✅ 备份代码格式验证
  - 有效格式验证
  - 长度检查
  - 字符类型检查
  - 空值处理
- ✅ 集成测试
  - 完整2FA设置流程

**测试用例数：** 24个

### 4. gmAccount.model.test.ts（GM账号模型）

**测试内容：**
- ✅ 查询GM账号
  - 根据用户名查找
  - 根据ID查找
  - 不存在时返回null
  - 数据库错误处理
- ✅ 登录失败次数管理
  - 增加失败次数
  - 达到最大次数自动锁定
  - 重置失败次数
- ✅ 登录信息更新
  - 更新最后登录时间和IP
- ✅ 登录日志记录
  - 成功登录日志
  - 失败登录日志
  - 2FA验证结果记录
- ✅ 信任设备管理
  - 根据token查找设备
  - 创建信任设备
  - 更新设备最后使用时间
  - 查找用户所有设备
  - 撤销设备
  - 只返回激活且未过期设备
- ✅ 2FA管理
  - 启用2FA
  - 禁用2FA

**测试用例数：** 19个
**Mock策略：** 完整Mock数据库连接池

### 5. auth.service.test.ts（认证服务）

**测试内容：**
- ✅ 登录流程
  - 成功登录返回完整响应
  - 用户名不存在错误
  - 账号禁用错误
  - 账号锁定错误
  - 密码错误增加失败次数
  - 需要2FA时返回标志
  - 2FA验证失败
  - 2FA验证成功
  - 信任设备跳过2FA
  - 记住设备创建Token
  - 重置失败次数
  - 更新最后登录信息
  - 权限分级（超级管理员、普通GM、高级GM）
- ✅ Token刷新
  - 成功刷新Token
  - 无效Token类型错误
  - 账号不存在错误
  - 账号禁用错误
- ✅ 登出
  - 成功登出
- ✅ 2FA设置
  - 生成密钥和QR码
- ✅ 2FA验证与启用
  - 验证成功启用
  - 验证失败错误
- ✅ 2FA禁用
  - 密码验证成功禁用
  - 账号不存在错误
  - 密码错误
- ✅ 信任设备管理
  - 获取设备列表
  - 撤销设备

**测试用例数：** 28个
**Mock策略：** Mock所有外部依赖（模型、工具类）

### 6. auth.controller.test.ts（认证控制器）

**测试内容：**
- ✅ 登录接口
  - 成功登录返回200
  - 需要2FA时返回相应消息
  - 缺少用户名/密码返回400
  - IPv6地址处理
- ✅ 刷新接口
  - 成功刷新返回新Token
  - 缺少Authorization header错误
  - 错误Token格式
  - 非Bearer类型错误
- ✅ 登出接口
  - 成功登出返回200
  - 未认证用户返回401
- ✅ 2FA设置接口
  - 成功返回密钥和QR码
  - 未认证用户返回401
- ✅ 2FA验证接口
  - 成功验证启用
  - 缺少参数返回400
- ✅ 2FA禁用接口
  - 成功禁用
  - 缺少密码返回400
- ✅ 信任设备列表接口
  - 成功获取列表
- ✅ 撤销设备接口
  - 成功撤销
  - 无效设备ID返回400
- ✅ 当前用户信息接口
  - 成功获取信息
  - 未认证返回401

**测试用例数：** 21个
**Mock策略：** Mock Express Request/Response对象和服务层

## 测试统计

```
总测试文件：6个
总测试用例：137个
测试覆盖模块：
  - 工具类（utils）：3个文件，69个测试
  - 数据模型（models）：1个文件，19个测试
  - 业务服务（services）：1个文件，28个测试
  - 控制器（controllers）：1个文件，21个测试
```

## 代码覆盖率目标

- **语句覆盖率（Statements）：** ≥ 85%
- **分支覆盖率（Branches）：** ≥ 80%
- **函数覆盖率（Functions）：** ≥ 85%
- **行覆盖率（Lines）：** ≥ 85%

## 查看覆盖率报告

运行测试后，覆盖率报告会生成在 `coverage/` 目录：

```bash
# 查看HTML报告（推荐）
open coverage/lcov-report/index.html

# 或直接在终端查看
cat coverage/lcov.info
```

## Mock策略

### 数据库Mock
```typescript
jest.mock('../../config/database', () => ({
  gmToolsPool: {
    query: jest.fn()
  }
}));
```

### 外部依赖Mock
```typescript
jest.mock('../../models/gmAccount.model');
jest.mock('../../utils/crypto.util');
jest.mock('../../utils/jwt.util');
jest.mock('../../utils/twoFactor.util');
```

### Logger Mock
```typescript
jest.mock('../../utils/logger.util', () => ({
  logger: {
    info: jest.fn(),
    error: jest.fn(),
    warn: jest.fn()
  }
}));
```

## 测试最佳实践

### 1. 测试隔离
- 每个测试用例应该独立运行
- 使用 `beforeEach` 清理Mock状态
- 不依赖测试执行顺序

### 2. Mock数据
- 使用真实的数据结构
- 覆盖正常和异常情况
- 验证Mock调用参数

### 3. 断言清晰
- 使用描述性的测试名称
- 每个测试只验证一个功能点
- 使用明确的断言（toEqual, toBe, toContain等）

### 4. 错误处理
- 测试错误情况
- 验证错误消息和状态码
- 确保资源正确清理

## 持续集成（CI）

建议在CI流程中添加以下步骤：

```yaml
# .github/workflows/test.yml 示例
- name: Install dependencies
  run: npm ci

- name: Run tests
  run: npm test

- name: Upload coverage
  uses: codecov/codecov-action@v3
  with:
    files: ./coverage/lcov.info
```

## 故障排查

### 测试失败
1. 检查环境变量（.env.test）
2. 确认Mock设置正确
3. 查看详细错误信息：`npm run test:verbose`

### 超时错误
- 增加超时时间：`jest.setTimeout(10000)`
- 检查异步操作是否正确处理

### 覆盖率不足
- 查看HTML报告找到未覆盖代码
- 添加边界条件测试
- 增加错误处理测试

## 未来改进

- [ ] 添加集成测试（API端到端测试）
- [ ] 添加性能测试
- [ ] 增加数据库集成测试（使用测试数据库）
- [ ] 添加负载测试
- [ ] 完善错误场景覆盖

## 参考资料

- [Jest官方文档](https://jestjs.io/docs/getting-started)
- [ts-jest文档](https://kulshekhar.github.io/ts-jest/)
- [Testing Best Practices](https://github.com/goldbergyoni/javascript-testing-best-practices)
