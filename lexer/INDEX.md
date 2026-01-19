# lexer/ - 词法分析器组件

**文档类型**: 导航索引
**生成日期**: 2025-11-05
**版本**: v1.0
**状态**: 活跃

---

## 概述

本目录包含从主二进制中提取的 **词法分析器（Lexer）组件**。这是一个完整的、独立的词法分析实现，用于解析游戏数据文件。

**项目来源**: Stage 3 提取（Phase 5.4.2）
**提取方式**: 从 df_game_r 二进制中静态反向工程
**代码行数**: ~2,000 行 (含注释)
**语言**: C++

---

## 目录结构

```
lexer/
├── 📄 dnflex.cpp                # 核心词法分析器 (448 行)
├── 📄 dnflex_wrapper.cpp        # 包装接口 (892 行)
├── 📄 compiled_dnflex.cpp       # 编译表 (648 行)
│
├── include/                     # 头文件
│   ├── dnflex.h
│   ├── dnflex_wrapper.h
│   └── dnflex_types.h
│
├── 📝 Makefile.lexer            # 编译配置
└── 📋 README.md                 # 组件说明
```

---

## 核心文件说明

### 1. dnflex.cpp (448 行)
**用途**: Flex 词法分析器核心实现
**包含**:
- DFA 状态表 (Deterministic Finite Automaton)
- 令牌识别逻辑
- 字符转换规则

**主要函数**:
- `yylex()` - 主要词法分析函数
- `BEGIN()` - 改变词法状态
- `ECHO()` - 输出令牌

### 2. dnflex_wrapper.cpp (892 行)
**用途**: 高级 C++ 包装接口
**包含**:
- 类型安全的接口
- 错误处理
- 缓冲区管理

**主要类**:
- `DnfLexer` - 词法分析器类
- `Token` - 令牌结构体
- `LexerState` - 状态管理

### 3. compiled_dnflex.cpp (648 行)
**用途**: 预编译的 Flex/Bison 输出
**包含**:
- `yy_def[]` - DFA 定义表 (10,000+ 项)
- `yy_ec[]` - 等价类表
- `yy_accept[]` - 接受状态表

---

## 快速导航

### 按任务查找

**我想编译词法分析器**
→ `make -f Makefile.lexer all`

**我想了解词法分析的工作原理**
→ 见 [工作原理](#工作原理)

**我想添加新的令牌类型**
→ 修改 dnflex.cpp 中的规则

**我想集成到主程序**
→ 链接 compiled_dnflex.o + dnflex_wrapper.o

---

## 工作原理

### Flex/Bison 分析流程

```
输入文本
  ↓
[DFA 状态机]  ← yy_def[], yy_ec[] 定义
  ↓
令牌识别
  ↓
动作执行 (yylex())
  ↓
输出: 令牌流
```

### 主要数据结构

#### DFA 状态表
```c
/* yy_def[] 定义状态转移 */
__int16 yy_def[10000] = {
    0, 9992, 1, 9992, ...  // 每个状态的默认转移
};

/* yy_ec[] 字符等价类 */
__int8 yy_ec[256] = {
    0, 0, 0, ...           // 字符分类
};

/* yy_accept[] 接受状态 */
__int16 yy_accept[500] = {
    0, 0, 0, ...           // 哪些状态是接受状态
};
```

---

## 编译与构建

### 编译命令

```bash
# 编译词法分析库
make -f Makefile.lexer all

# 生成对象文件
make -f Makefile.lexer objects

# 清理
make -f Makefile.lexer clean

# 测试
make -f Makefile.lexer test
```

### 编译标志

```makefile
CXXFLAGS = -m32 -O2 -g -fPIC -std=c++11
LDFLAGS = -m32 -Wl,--as-needed
```

---

## 功能特性

### 支持的令牌类型

| 令牌 | 描述 | 示例 |
|------|------|------|
| KEYWORD | 关键字 | if, while, return |
| IDENTIFIER | 标识符 | variable_name |
| NUMBER | 数字 | 123, 0x456 |
| STRING | 字符串 | "hello world" |
| OPERATOR | 操作符 | +, -, *, / |
| DELIMITER | 分隔符 | (, ), {, } |
| COMMENT | 注释 | // ... 或 /* ... */ |

### 状态管理

支持多个词法状态，用于处理不同的上下文：

```c
BEGIN(INITIAL);   // 初始状态
BEGIN(STRING);    // 字符串解析状态
BEGIN(COMMENT);   // 注释解析状态
```

---

## 使用示例

### 基础用法

```cpp
#include "dnflex_wrapper.h"

// 创建词法分析器
DnfLexer lexer("input.txt");

// 获取令牌流
std::vector<Token> tokens;
Token token;
while (lexer.getNextToken(token)) {
    tokens.push_back(token);
    std::cout << "Token: " << token.type << " = " << token.value << std::endl;
}
```

### 集成到主程序

```cpp
// 在主程序中
#include "dnflex_wrapper.h"

void parse_game_script(const char* filename) {
    DnfLexer lexer(filename);
    // 使用词法分析器处理数据
    // ...
}
```

---

## 文件统计

| 文件 | 行数 | 大小 | 类型 |
|------|------|------|------|
| dnflex.cpp | 448 | ~15K | 核心实现 |
| dnflex_wrapper.cpp | 892 | ~28K | 包装 |
| compiled_dnflex.cpp | 648 | ~20K | 表数据 |
| 头文件 | ~150 | ~8K | 接口 |
| **总计** | **~2,100** | **~70K** | — |

---

## 相关文档

**其他组件**:
- [parser/INDEX.md](../parser/INDEX.md) - 解析器（词法分析后的阶段）
- [tools/INDEX.md](../tools/INDEX.md) - 提取工具

**源代码**:
- [src/INDEX.md](../src/INDEX.md) - 主程序源代码

---

**上次更新**: 2025-11-05
**维护者**: AI Assistant
**状态**: 完整

