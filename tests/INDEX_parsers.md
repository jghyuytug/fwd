# test/ - 测试套件目录

**文档类型**: 导航索引
**生成日期**: 2025-11-05
**版本**: v1.0
**状态**: 活跃

---

## 概述

本目录包含项目的单元测试和集成测试代码。提供对解析器、词法分析器和核心功能的全面测试覆盖。

**测试框架**: Google Test (gtest) / Catch2
**测试数量**: 70+ 个测试用例
**代码行数**: ~1,400 行

---

## 目录结构

```
test/
├── 📂 lexer_tests/              # 词法分析器测试 (~200 行)
│   └── test_lexer.cpp
│
├── 📂 parser_tests/             # 解析器测试 (~600 行)
│   ├── test_parser.cpp
│   ├── test_creature_parser.cpp
│   ├── test_dungeon_parser.cpp
│   └── test_data/
│
├── 📂 network_tests/            # 网络层测试 (~400 行)
│   └── test_network.cpp
│
├── Makefile.test                # 测试构建配置
└── README.md                    # 测试说明
```

---

## 测试内容

### Lexer 测试 (200 行)
- 令牌识别测试
- 状态转换测试
- 边界条件测试

### Parser 测试 (600 行)
- 各解析器的单元测试
- 数据完整性验证
- 错误恢复测试

### Network 测试 (400 行)
- 网络层功能测试
- 协议验证
- 并发测试

---

## 快速命令

```bash
# 编译所有测试
make -f Makefile.test all

# 运行所有测试
make -f Makefile.test test

# 运行特定测试
make -f Makefile.test test_lexer
make -f Makefile.test test_parser

# 生成覆盖报告
make -f Makefile.test coverage

# 清理测试产物
make -f Makefile.test clean
```

---

## 测试报告

运行测试后检查输出，确保所有测试通过 ✅

**预期结果**: 70+ tests passed

---

**上次更新**: 2025-11-05
**维护者**: AI Assistant
**状态**: 活跃

