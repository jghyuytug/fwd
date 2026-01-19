# CLAUDE.md

## 基本要求

- **中文**交流
- **Docker**构建环境
- 详细指南使用 `/skills` 查看可用命令

---

## !! 警告: 本地编辑，远程编译 !!

**历史教训 (2025-11-27)**:

曾经犯过的错误:
- 在服务器上用 `sed`/`awk` 编辑代码 → 换行符转义问题导致编译失败
- 反复尝试用 shell 命令修复 → 越改越乱，浪费大量时间
- 忘记本地有完整的编辑工具 → 舍近求远

**必须遵守**:

1. **本地编辑**: 使用 Edit/Write 工具修改本地文件
2. **SCP 上传**: `scp local_file vm131:/remote/path/`
3. **远程编译**: `ssh vm131 "cd /path && make"`
4. **远程测试**: `ssh vm131 "./binary"` 或本地 `python test.py`

**禁止操作**:
- 禁止在服务器上用 `sed -i` 修改源代码
- 禁止在服务器上用 `awk` 插入代码
- 禁止用 `echo >>` 追加代码到源文件
- 禁止用 `perl -i` 就地编辑

**正确工作流**:
```bash
# 1. 本地编辑 (用 Edit 工具)
# 2. 上传
scp "C:/Users/waw/Desktop/game/src/xxx.cpp" vm131:/tmp/game_cpp_new/src/
# 3. 编译
ssh vm131 "cd /tmp/game_cpp_new && make clean && make db"
# 4. 运行
ssh vm131 "./df_game_db 20011"
```

---

## !! 警告: 避免进度幻觉 !!

**历史教训 (2025-11-26)**:

曾经犯过的错误:
- 说"50个网络处理器已实现" → 实际只是空壳mock
- 说"功能实现30-35%" → 实际 <1%
- 说"XX系统100%完成" → 只是测试代码
- 写大量"Phase X 完成报告" → 自我欺骗

**必须遵守**:

1. **编译通过 ≠ 功能完成**
2. **测试通过 ≠ 生产可用**
3. **代码存在 ≠ 逻辑实现**
4. **不要美化进度数字**
5. **不要写虚假的完成报告**

**验证标准**:
- 能否与真实客户端通信？
- 能否处理真实游戏场景？
- 与生产版本对比差距多少？

**正确做法**:
- 基于 dnf1 现有架构修改，不要从零造轮子
- 小步验证，确认真正可用再说"完成"
- 诚实评估，宁可低估不要高估

---

## 项目状态 (2025-11-29 深度核实)

### 代码量化指标 (实测)

| 模块 | 代码行数 | 说明 |
|------|----------|------|
| **Game服务** | 23,792 | main_db.cpp |
| **ORM层** | 16,288 | 973表全覆盖 |
| **Guild服务** | 1,676 | 基础CRUD |
| **Monitor服务** | 953 | 服务注册/心跳 |
| **Manager服务** | 856 | 进程管理 |
| **总计** | **43,565** | - |

### Game服务详细指标

| 指标 | 数值 | 说明 |
|------|------|------|
| **Handler函数** | 237 | `static int handle_*` |
| **DB函数** | 129 | `static int db_*` |
| **SQL语句** | 235 | INSERT/UPDATE/DELETE/SELECT |
| **协议号** | 211 | 唯一case分支 |
| **有DB调用** | 138 | 58% 真实持久化 |

### ORM层完成状态 ✅

| 区域 | 数据库数 | 表数 | 状态 |
|------|---------|------|------|
| **core_game** | 5 | 288 | ✅ 100% |
| **web_community** | 2 | 476 | ✅ 100% |
| **audit_logging** | 1 | 101 | ✅ 100% |
| **extended_features** | 5 | 85 | ✅ 100% |
| **commerce_trade** | 3 | 69 | ✅ 100% |
| **event_operations** | 2 | 64 | ✅ 100% |
| **security_monitoring** | 2 | 50 | ✅ 100% |
| **channel_system** | 2 | 8 | ✅ 100% |
| **总计** | **22** | **973** | ✅ **100%** |

### Phase 15 完成: 运营系统 (2025-11-29 核实)

| 任务 | 状态 | 实现 |
|------|------|------|
| P1: 商城系统 | ✅ | `db_get_cash_balance`, `db_deduct_cash`, `db_get_vip_info`, `cash_shop_buy_item` (msg_no=510-514) |
| P2: 活动奖励 | ✅ | 金币/经验/物品发放完整 (msg_no=390-391) |
| P3: 周常任务 | ✅ | `db_load_weekly_quests` 使用 YEARWEEK 自动周重置 (msg_no=382-383) |
| P4: 签到系统 | ✅ | 连续签到streak, 补签makeup_used(最多3次), 月重置 (msg_no=534-536) |
| P5: 跨服匹配 | ✅ | 内存队列 `g_match_queue`, 排队/取消/状态/接受/拒绝/奖励 (msg_no=550-555) |

**测试验证**: 2025-11-29 6/6 测试全部通过

### Phase 14 状态 (已完成)

| 任务 | 状态 | DB支持 |
|------|------|--------|
| 深渊副本 (350-355) | ✅ | `db_consume_abyss_tickets`, `db_get_abyss_daily_count` |
| 日常任务 (380-381) | ✅ | `db_load_daily_quests`, `db_save_daily_quest` |
| 活动系统 (390-391) | ✅ | `db_load_event_progress`, 完整奖励发放 |
| 商城系统 (510-514) | ✅ | `db_get_cash_balance`, `db_deduct_cash`, VIP验证 |
| 跨服匹配 (550-555) | ✅ | 内存队列匹配，结果奖励持久化 |

### Phase 13 完成: 高级游戏系统 (5个任务)

| 任务 | 状态 | 实现 |
|------|------|------|
| P1: 死亡之塔系统 | ✅ | 100层挑战，每日3次，每10层Boss，排行榜 (msg_no=410-412) |
| P2: 套装系统 | ✅ | 套装定义，3/5/7件阶梯属性加成 (msg_no=420) |
| P3: 赛季系统 | ✅ | 7级段位(青铜~宗师)，ELO积分 (msg_no=430) |
| P4: 装备成长系统 | ✅ | 10级成长，2%~40%属性加成 (msg_no=440-441) |
| P5: 神秘商店系统 | ✅ | 6格商品，4小时刷新，限购机制 (msg_no=450-452) |

### 历史完成: Phase 12 游戏体验提升 (5个任务)

| 任务 | 状态 | 实现 |
|------|------|------|
| P1: Boss战机制深化 | ✅ | 3阶段转换，4种技能槽，狂暴机制，弱点系统 |
| P2: 成就系统 | ✅ | 20种成就类型，进度追踪，奖励发放 (msg_no=290-296) |
| P3: 称号系统 | ✅ | 称号属性加成，装备/卸下 (msg_no=160-165) |
| P4: 深渊副本模式 | ✅ | **已在350-355实现** - 入场券/Boss/特殊掉落 |
| P5: 日志审计系统 | ✅ | 25种行为日志，金币/物品流转追踪 |

**累计完成**: Phase 4.5→6→7→8→9→10→11→12→13→14→15 共 60+ 个任务

### 历史完成: Phase 6 战斗系统基础

| 任务 | 状态 | 实现 |
|------|------|------|
| 技能伤害公式 | ✅ | `calculate_skill_damage()` - 物理/魔法伤害，暴击，防御计算 |
| 技能冷却系统 | ✅ | `skill_is_on_cooldown()`, `skill_start_cooldown()` - CDR支持 |
| Buff系统框架 | ✅ | 27种Buff类型，堆叠/持续/DOT效果，`buff_add/remove/update()` |
| 怪物基础AI | ✅ | 10种AI类型 (Idle/Patrol/Chase/Flee/Guard/Boss等) |
| 怪物攻击模式 | ✅ | 11种攻击类型，25+怪物技能表，施法/恢复计时器 |

### 历史完成: Phase 4.5 (10个优先任务)

| 任务 | 状态 | 实现 |
|------|------|------|
| P1: 疲劳值系统 | ✅ | `db_update_fatigue()`, `db_get_fatigue()` - 副本进入扣疲劳并持久化 |
| P2: 升级系统 | ✅ | `get_exp_for_level()`, `check_and_level_up()` - exp = base * level^2.3 |
| P3: 装备耐久度 | ✅ | `db_update_item_durability()`, `decrease_equipment_durability()` |
| P4: NPC对话逻辑 | ✅ | 9种NPC类型(商店/铁匠/仓库/任务/传送/公会/拍卖/技能/分解) |
| P5: 任务进度持久化 | ✅ | `db_save_quest_progress()`, `db_load_quest_progress()` |
| P6: 宠物系统 | ✅ | 召唤/战斗逻辑，所有权验证，session状态追踪 |
| P7: 仓库物品持久化 | ✅ | `db_storage_deposit_item()`, `db_storage_withdraw_item()` |
| P8: 交易物品转移 | ✅ | `db_transfer_item()` 已实现 |
| P9: 拍卖行DB集成 | ✅ | `db_auction_create/search/buy/cancel()` |
| P10: Guild IPC通信 | ✅ | `guild_ipc_connect/send/recv()` 基础设施 |

### 里程碑: 核心链路 + 完整数据库持久化

**已验证**: 真实客户端能够通过完整链路进入城镇
```
Client -> Bridge(7000) -> Channel(7001) -> Game(10011) -> 进入城镇 ✓
```

**数据库读取** (全部真实数据):
```
背包(user_items) ✓ | 技能(skill) ✓ | 邮件(postal) ✓ | 好友(charac_friends) ✓
公会(guild_info) ✓ | 仓库(account_cargo) ✓ | 宠物(creature_items) ✓
排行榜(charac_info+inventory JOIN) ✓
```

**数据库写入** (持久化操作):
```
金币: NPC_SHOP_BUY/SELL, DUNGEON_CLEAR, MAIL_SEND/CLAIM ✓
物品: NPC_SHOP_BUY/SELL, DUNGEON_REWARD, EQUIP/UNEQUIP, MOVE_ITEM ✓
邮件: MAIL_SEND, MAIL_DELETE, MAIL_READ, MAIL_CLAIM ✓
好友: FRIEND_ADD, FRIEND_REMOVE, FRIEND_ACCEPT ✓
公会: GUILD_CREATE, GUILD_JOIN, GUILD_LEAVE, GUILD_DISBAND, GUILD_KICK ✓
经验: DUNGEON_CLEAR (db_add_exp) ✓
```

### 服务端架构状态 (2025-11-29 深度核实)

```
开发代码总量: 43,565行
├── Game服务:     23,792行 (237 handler, 211协议)
├── ORM层:        16,288行 (973表, 22数据库) ✅ 100%完成
├── Guild服务:     1,676行 (运行中)
├── Monitor服务:     953行 (运行中)
└── Manager服务:     856行 (开发完成)

生产版二进制参考 (dnf1/home/neople/):
├── game/df_game_r        35 MB    ← 开发版 576KB (1.6%)
├── auction/df_auction_r  12 MB    ← 未开始
├── point/df_point_r      12 MB    ← 未开始
├── dbmw_*/df_dbmw_r    3.9MB x3   ← ORM层替代 ✅
├── bridge/df_bridge_r   3.3 MB    ← 使用生产版
├── channel/df_channel_r 1.3 MB    ← 使用生产版
├── guild/df_guild_r     2.2 MB    ← 开发版运行中
├── monitor/df_monitor_r 2.4 MB    ← 开发版运行中
├── manager/df_manager_r 3.1 MB    ← 开发完成
├── relay/df_relay_r     409 KB    ← 未开始
└── 其他6个服务          ~8 MB     ← 未开始
```

### 服务运行状态

| 服务 | 端口 | 版本 | 状态 |
|------|------|------|------|
| Bridge | 7000 | 生产版 | ✅ 运行中 |
| Channel | 7001 | 生产版 | ✅ 运行中 |
| Game | 20011 | 开发版 | ✅ 运行中 |
| Guild | 30403 | 开发版 | ✅ 运行中 |
| Monitor | 30303 | 开发版 | ✅ 运行中 |
| ORM/DBMW | - | 开发版 | ✅ **100%完成** |

### Phase 8 完成: 支撑服务 (2025-11-28)

| 服务 | 端口 | 代码行数 | 功能 |
|------|------|----------|------|
| Guild Server | 30403 | 1676行 | 22消息类型，公会CRUD/成员/技能/仓库/等级系统 |
| Monitor | 30303 | 660行 | 服务注册/心跳/健康检测/状态查询 |
| Manager | 40403 | 750行 | 进程启动/停止/重启/自动恢复 |

### Game 服务协议状态 (205个已实现, 2025-11-29核对)

**协议号分布统计**:
```
0-20   (核心登录/角色):    18个
21-50  (游戏基础操作):     26个
51-70  (PVP/好友):         8个
71-101 (公会/邮件/仓库):   9个
102-199(系统扩展):         30个
200-299(Custom扩展):       66个
300-399(Phase7+深渊):      13个
400-499(Phase13):          20个
500-555(Phase14):          15个
━━━━━━━━━━━━━━━━━━━━━━━━━━
总计:                      205个
```

| 类别 | msg_no | 协议 | 状态 |
|------|--------|------|------|
| **核心** | 0 | CHECK_CONNECTION | ✓ |
| | 1 | LOGIN (DB集成) | ✓ |
| | 2 | SET_UDP_IP_PORT | ✓ |
| | 3 | EXIT | ✓ |
| | 4 | SELECT_CHARACTER | ✓ |
| | 5 | CREATE_CHARACTER | ✓ |
| | 6 | DELETE_CHARACTER | ✓ |
| | 7 | RETURN_SELECT | ✓ |
| | 8 | GET_USERINFO | ✓ |
| **组队** | 12 | PARTY_CREATE | ✓ |
| | 13 | PARTY_LEAVE | ✓ |
| | 14 | PARTY_KICK | ✓ |
| **游戏流程** | 15 | START_GAME | ✓ |
| | 16 | DUNGEON_ENTER | ✓ |
| | 17 | CHAT | ✓ |
| | 18 | DELETE_ITEM | ✓ |
| | 19 | MOVE_ITEM | ✓ |
| | 20 | SORT_ITEM | ✓ |
| **NPC交易** | 21 | NPC_SHOP_BUY | ✓ |
| | 24 | NPC_SHOP_SELL | ✓ |
| | 25 | REPAIR_EQUIPMENT | ✓ |
| **交易/合成** | 26 | TRADE_REQUEST | ✓ |
| | 27 | COMPOUND_ITEM | ✓ |
| | 28 | DISJOINT_ITEM | ✓ |
| **技能** | 30 | CHANGE_SKILLSLOT | ✓ |
| | 31 | BUY_SKILL | ✓ |
| | 32 | INCREASE_STATUS | ✓ |
| **任务** | 33 | ACCEPT_QUEST | ✓ |
| | 34 | GIVEUP_QUEST | ✓ |
| | 35 | SET_QUEST_TRIGGER | ✓ |
| | 36 | FINISH_QUEST | ✓ |
| **移动** | 37 | SET_USER_POSITION | ✓ |
| | 38 | SET_USER_AREA | ✓ |
| **战斗** | 40 | FINISH_LOADING | ✓ |
| | 41 | USE_SKILL | ✓ |
| | 42 | DIE_MONSTER | ✓ |
| | 43 | DIE_CHARACTER | ✓ |
| | 44 | USE_COIN (复活) | ✓ |
| | 45 | GIVEUP_GAME | ✓ |
| | 46 | GET_ITEM (拾取) | ✓ |
| | 47 | USE_STACKABLE | ✓ |
| | 48 | MOVE_MAP (换房) | ✓ |
| | 49 | SET_PLAY_RESULT | ✓ |
| | 50 | DROP_ITEM | ✓ |
| **PVP** | 53-56 | PVP房间系统 | ✓ |
| **好友** | 63 | FRIEND_ADD | ✓ |
| | 64 | FRIEND_REMOVE | ✓ |
| **公会** | 70 | GUILD_MEMBER_LIST | ✓ |
| | 77 | GUILD_INFO | ✓ |
| | 78 | GUILD_JOIN | ✓ |
| | 81 | GUILD_LEAVE | ✓ |
| **强化** | 83 | UPGRADE_ITEM | ✓ |
| **邮件** | 97-99 | 邮件系统 | ✓ |
| **仓库金币** | 100-101 | 存取金币 | ✓ |
| **称号** | 160-165 | 称号系统 (6个) | ✓ |
| **拍卖** | 170-178 | 拍卖系统 (9个) | ✓ |
| **仓库物品** | 180-185 | 仓库系统 (6个) | ✓ |
| **宠物** | 190-198 | 宠物系统 (9个) | ✓ |
| **Custom扩展** | 200-205 | 背包/装备/属性 | ✓ |
| | 210-215 | 副本扩展 | ✓ |
| | 220-225 | 组队扩展 | ✓ |
| | 230-235 | 好友扩展 | ✓ |
| | 240-248 | 公会扩展 | ✓ |
| | 250-253 | 邮件扩展 | ✓ |
| | 260-267 | 交易扩展 | ✓ |
| | 270-277 | PVP扩展 | ✓ |
| | 280-285 | 排行榜 | ✓ |
| | 290-296 | 成就系统 | ✓ |
| **Phase7** | 300-306 | 强化/技能/物品/难度/公会/验装 | ✓ |
| **Phase13** | 410-412 | 死亡之塔 | ✓ |
| | 420 | 套装系统 | ✓ |
| | 430 | 赛季系统 | ✓ |
| | 440-441 | 装备成长 | ✓ |
| | 450-452 | 神秘商店 | ✓ |
| | 460-462 | 连招/技能取消/消耗品 | ✓ |
| | 470-473 | 传送/换图/任务触发/追踪 | ✓ |
| | 480-482 | 快速组队匹配 | ✓ |

### 新增子系统特性

**疲劳值系统** (P1):
- 副本进入扣除疲劳 (难度1=8点, 难度2=10点, 难度3=12点, 难度4+=15点)
- 疲劳值DB持久化到 charac_stat 表

**升级系统** (P2):
- 经验曲线: exp = 100 * level^2.3
- 自动检测升级并更新属性
- 最高85级

**装备耐久度** (P3):
- 副本通关后扣除耐久 (难度1=2点, 难度4=8点)
- 耐久度0时装备无效
- 铁匠NPC修理功能

**NPC对话系统** (P4):
- 9种NPC类型识别
- 动态对话选项返回

**任务持久化** (P5):
- quest_log 表存储进度
- 状态: 0=未接, 1=进行中, 2=完成

**宠物系统** (P6):
- 召唤/解散/喂养
- 所有权验证
- 战斗状态追踪

**Guild IPC** (P10):
- Socket连接到 30403 端口
- 异步消息发送/接收
- 在线状态同步

### Phase 7 子系统 (2025-11-28)

**装备强化** (P1):
- +1~+15强化等级
- 成功率: 100%→95%→90%→...→25%→18%→12%
- 失败惩罚: 等级-0/-1/-2 (低/中/高级)
- 保护石: 防爆/防降级

**随机属性** (P2):
- 12种属性类型 (物攻/魔攻/暴击/生命/防御/回避/速度/吸血等)
- 5种稀有度 (普通/魔法/稀有/史诗/传说)
- 属性数量: 0/1-2/2-3/3-4/4-5 (按稀有度)

**技能系统** (P4):
- 前置技能检查
- SP消耗表: 1/1/1/2/2/3/3/4/5/6 (10级)
- 技能重置和SP返还

**物品堆叠** (P6):
- 消耗品/材料/货币可堆叠
- 最大堆叠: 99/999/9999 (按类型)
- 自动合并和拆分

**副本难度** (P7):
- 5级难度: 普通/冒险/勇士/王者/地狱
- 最低等级: 1/15/25/40/55
- 怪物属性缩放: HP/攻击/防御
- 奖励倍率: 1.0x~3.0x

**公会等级** (P9):
- 20级上限
- 经验表: 1000→3000→6000→...→360000
- 成员上限: 20→22→...→60

**聊天频道** (P10):
- 7种频道: 普通/组队/公会/世界/私聊/系统/喇叭
- 冷却限制: 2s普通, 60s世界
- 敏感词过滤 (10个占位词)

### 副本系统特性

- 多房间支持 (难度决定房间数: 3-7)
- 怪物生成 (每房间3-8只，根据难度)
- 伤害计算和击杀判定
- 房间清空检测和推进
- 通关评级 (S/A/B/C/D)
- 奖励计算 (经验/金币/物品)

### 数据库集成

| 指标 | 状态 |
|------|------|
| 表总数 | 973表 (22个数据库) |
| 数据库连接 | 4个: d_taiwan, taiwan_cain, taiwan_cain_2nd, d_guild |
| 读取函数 | 25+ (login, inventory, skill, mail, friend, guild, pet, ranking, fatigue, quest...) |
| 写入函数 | 25+ (gold, item, mail, friend, guild, exp, fatigue, durability, quest, auction...) |
| 真实数据返回 | ✓ 角色、背包、技能、邮件、好友、公会、宠物、排行榜 |
| 持久化写入 | ✓ 金币、物品、邮件、好友、公会、经验、疲劳、耐久、任务、拍卖 |
| ORM层 | ✅ **100%完成** - 973表, 16,288行代码 |

### 测试服务器 (VM-131)

| 端口 | 服务 | 说明 |
|------|------|------|
| 7000 | Bridge | 生产版本 |
| 7001 | Channel | 生产版本 |
| 10011 | Game cain01 | 生产版本 |
| 20011 | df_game_db | 开发测试版 (MySQL集成) |

## 开发优先级

### Phase 1: 核心链路 ✓ 已完成
1. ~~**Bridge** - 客户端入口~~ (使用生产版本)
2. ~~**Channel** - 频道选择~~ (使用生产版本)
3. ~~**Game** - 基础协议~~ (登录、选角、进城镇)

### Phase 2: Game 协议扩展 ✓ 已完成
1. ~~背包系统 (INVENTORY_*)~~ - msg_no=25,26,27
2. ~~技能系统 (SKILL_*)~~ - msg_no=30,31
3. ~~NPC交互 (NPC_*)~~ - msg_no=35,36,37
4. ~~移动系统 (MOVE_*)~~ - msg_no=16,20,21

### Phase 3: 副本系统 ✓ 已完成
1. ~~副本进入/退出~~ - msg_no=60,61,62
2. ~~怪物生成~~ - 动态生成，难度缩放
3. ~~战斗逻辑~~ - msg_no=65 伤害计算
4. ~~奖励结算~~ - msg_no=70,71 评级和奖励

### Phase 4: 社交与经济系统 ✓ 已完成
1. ~~组队系统~~ - msg_no=120-126 (创建/邀请/加入/离开/踢人/改队长)
2. ~~拍卖系统~~ - msg_no=170-174 (搜索/上架/取消/购买/我的列表)
3. ~~交易系统~~ - msg_no=160-164 (含物品转移持久化)
4. ~~好友在线状态~~ - is_character_online() 检测
5. ~~公会在线状态~~ - 成员在线显示
6. ~~角色软删除~~ - delete_flag + delete_time

### Phase 4.5: 10个优先任务 ✓ 已完成
1. ~~疲劳值系统~~ - 副本消耗+DB持久化
2. ~~升级系统~~ - 经验曲线+自动升级
3. ~~装备耐久度~~ - 战斗消耗+修理
4. ~~NPC对话逻辑~~ - 9种NPC类型
5. ~~任务进度持久化~~ - quest_log表
6. ~~宠物系统~~ - 召唤/战斗/所有权
7. ~~仓库物品持久化~~ - account_cargo_items表
8. ~~交易物品转移~~ - db_transfer_item()
9. ~~拍卖行DB集成~~ - auction_house表
10. ~~Guild IPC通信~~ - Socket IPC基础设施

### Phase 6: 战斗系统基础 ✓ 已完成
1. ~~技能伤害公式~~ - 物理/魔法/暴击/防御
2. ~~技能冷却系统~~ - CDR支持
3. ~~Buff系统框架~~ - 27种类型
4. ~~怪物基础AI~~ - 10种AI类型
5. ~~怪物攻击模式~~ - 11种攻击类型，25+技能

### Phase 7: 下一阶段 10 个优先任务
详见下方"当前开发任务"

### Phase 8: 支撑服务 (后续)
- DBMW - 数据库中间件
- Monitor - 监控
- Manager - 进程管理
- Relay - P2P中继

---

## 当前开发任务: Phase 16 - 系统深化 (5个优先级任务)

**目标**: ORM集成、战斗系统真实化、独立服务拆分
**详细计划**: `docs/PHASE16_DEVELOPMENT_PLAN.md`
**预计周期**: 5周

### 5个优先级任务

| # | 任务 | 优先级 | 工作量 | 说明 |
|---|------|--------|--------|------|
| **P0** | ORM集成到Game服务 | **最高** | 5天 | 复用973表ORM，替代直连MySQL |
| **P1** | 技能效果真实化 | **高** | 7天 | 真实伤害公式、连招、多段攻击 |
| **P2** | 怪物AI行为完善 | **高** | 5天 | 仇恨系统、技能释放、Boss阶段 |
| **P3** | PVP对战逻辑实现 | **中** | 5天 | 同步框架、伤害判定、积分系统 |
| **P4** | Auction独立服务 | **中** | 5天 | 拆分为独立进程，使用ORM |

### P0: ORM集成到Game服务 (Week 1)

**当前**: Game直连MySQL，手写SQL
**目标**: 使用ORM层(16,288行, 973表)

```c
// 当前方式
snprintf(query, "SELECT * FROM charac_info WHERE charac_no=%u", id);
mysql_query(g_db, query);

// 目标方式
CharacInfo_GetByCharacNo(&g_db_manager, id, &info);
```

**步骤**:
1. Day 1: 基础设施 (orm_wrapper.h, Makefile)
2. Day 2: 核心表替换 (charac_info, user_items, skill)
3. Day 3: 社交表替换 (friends, postal, guild)
4. Day 4: 经济表替换 (cargo, auction, cash)
5. Day 5: 测试验证

### P1: 技能效果真实化 (Week 2-3)

**当前**: 简化伤害公式
**目标**: 真实技能效果系统

- 技能数据结构 (SkillDefinition)
- 200+技能表填充
- 元素克制、多段伤害
- 连招/取消系统
- 毫秒级冷却

### P2: 怪物AI行为完善 (Week 3)

**当前**: 10种基础AI类型
**目标**: 智能战斗AI

- 仇恨系统 (目标切换)
- 技能释放逻辑
- 走位/闪避行为
- Boss多阶段转换

### P3: PVP对战逻辑实现 (Week 4)

**当前**: 房间创建OK，对战空壳
**目标**: 真实PVP系统

- 位置/输入同步
- PVP伤害计算
- 胜负判定
- ELO积分/段位

### P4: Auction独立服务 (Week 5)

**当前**: Game内嵌简化版
**目标**: 独立服务 (参考生产版12MB)

- 独立进程架构
- Game<->Auction IPC
- 使用ORM层
- 完整搜索/上架/购买

---

## Phase 15 完成记录 (2025-11-29)

| 任务 | 状态 | 实现 |
|------|------|------|
| P1: 商城系统 | ✅ | `db_get_cash_balance`, `db_deduct_cash`, `cash_shop_buy_item` |
| P2: 活动奖励 | ✅ | 金币/经验/物品发放完整 |
| P3: 周常任务 | ✅ | `db_load_weekly_quests` 使用 YEARWEEK |
| P4: 签到系统 | ✅ | 连续签到streak, 补签makeup_used |
| P5: 跨服匹配 | ✅ | 内存队列 `g_match_queue` |

### 真实完成度评估

```
模块              完成度    说明
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
ORM/数据库层      100%      973表, 22数据库, 生产就绪
Game服务协议       47%      211/~450协议
Game服务功能       15%      核心链路OK，细节待完善
Guild服务          20%      基础CRUD运行中
Monitor服务        15%      服务注册/心跳
Manager服务        10%      进程管理基础
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
服务覆盖           5/16     (ORM+Game+Guild+Monitor+Manager)
代码量             43K行    vs 生产版~85MB二进制
```

**按功能模块**:
```
核心游戏循环:     可用 (登录→副本→奖励)
经济系统:         可用 (金币/物品/NPC商店)
社交系统:         可用 (好友/邮件/公会基础)
成长系统:         框架有 (技能/装备/强化)
副本系统:         简化版 (普通+深渊)
PVP系统:          框架有 (房间OK，匹配简化)
运营系统:         部分 (日常/商城/签到)
```

### 历史完成: Phase 10-11 战斗系统深化

| Phase | 任务数 | 关键功能 |
|-------|--------|----------|
| Phase 10 | 5 | 伤害v3/怪物技能/死亡复活/反作弊/计时器 |
| Phase 11 | 5 | 伤害集成/AI集成/Buff真实生效/装备属性/多人同步 |

## 快速命令

```bash
# 本地构建
make all              # 全部构建
make db               # 构建数据库版本
make clean            # 清理

# Docker构建
docker run -it -v $(pwd):/build dnf_game_builder bash

# VM-131 测试
ssh vm131 "cd /tmp/game_cpp_new && ./df_game_db 20011"

# 测试客户端
python tools/test_game_client.py 192.168.200.131:20011 1
```

详见 `/build` 命令

## 快速查找

| 需要 | 位置 |
|------|------|
| Game服务源码 | `src/game_cpp/src/main.cpp` |
| 数据库版本 | `src/game_cpp/src/main_db.cpp` |
| 游戏常量 | `include/common/constants_improved.h` |
| 错误码 | `include/common/error_codes.h` |
| 玩家结构 | `include/user/CUser.h` |
| ORM代码 | `src/orm/` |
| 测试工具 | `tools/test_game_client.py` |
| 扩展协议测试 | `tools/test_extended_protocols.py` |
| 副本测试 | `tools/test_dungeon.py` |
| 新功能测试 | `tools/test_new_features.py` |
| 数据库配置 | `config/database.cfg` |
| 构建配置 | `src/game_cpp/Makefile` |

## 数据库配置

```
Host: 192.168.200.131:3306
User: root
Pass: 88888888
账户库: d_taiwan (accounts表)
角色库: taiwan_cain (charac_info表)
```

## 代码规范

- 不删除注释代码
- 保持函数签名
- 使用已定义常量
- 保留调试符号 (`-g`)
- 遵循模块分层
- GCC 4.1.2 兼容 (无C++11)

## 可用技能

| 命令 | 用途 |
|------|------|
| `/skills` | 技能索引 |
| `/build` | 构建、环境、测试 |
| `/arch` | 架构、目录结构 |
| `/dev` | 开发任务、ORM使用 |
| `/docs` | 文档导航 |
| `/troubleshoot` | 故障排除 |
| `/server` | 生产服务器 VM-131 |
| `/server-new` | 开发服务器 VM-128 |

## 关键文档

- `README.md` - 项目总览
- `docs/INDEX.md` - 文档入口
- `docs/generated/VERSION_COMPARISON_REPORT.md` - 现实差距
- `src/orm/README.md` - ORM指南
- `src/game_cpp/README.md` - Game服务说明
