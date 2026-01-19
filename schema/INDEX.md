# schema/ - 数据库模式目录

**文档类型**: 导航索引
**生成日期**: 2025-11-05
**版本**: v1.0
**状态**: 活跃

---

## 概述

本目录包含游戏数据库的模式定义、表结构和关系图。包括 SQL 脚本、模式文档和数据库初始化脚本。

**大小**: ~384KB
**主要内容**: SQL 脚本、模式定义、初始化数据

---

## 目录结构

```
schema/
├── definitions/                 # 表定义
│   ├── tables.sql
│   ├── views.sql
│   ├── procedures.sql
│   └── ...
│
├── 📋 README.md                 # 说明
└── 初始化脚本
```

---

## 内容说明

### definitions/ 子目录

**包含内容**:
- 表定义 (CREATE TABLE)
- 视图定义 (CREATE VIEW)
- 存储过程 (PROCEDURE)
- 索引定义 (INDEX)
- 约束定义 (CONSTRAINT)

### 主要表

游戏服务器涉及的主要表：

| 表名 | 用途 |
|------|------|
| users | 用户账户 |
| characters | 游戏角色 |
| inventory | 物品库存 |
| skills | 技能数据 |
| quests | 任务数据 |
| dungeons | 地下城配置 |
| ... | ... |

---

## 使用场景

**数据库管理员**:
- 初始化数据库
- 维护表结构
- 备份和恢复

**开发者**:
- 理解数据结构
- 编写数据访问代码
- 设计新功能

---

## 初始化数据库

```bash
# 导入模式
mysql -u root -p game_db < schema/definitions/tables.sql

# 导入初始数据
mysql -u root -p game_db < schema/init_data.sql
```

---

**上次更新**: 2025-11-05
**维护者**: AI Assistant
**状态**: 活跃

