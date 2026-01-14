/*
 * Packet Types - Production Protocol Mapping
 * Extracted from df_game_r binary via strings analysis
 *
 * ENUM_CMDPACKET_* = Client -> Server (608 types)
 * ENUM_NOTIPACKET_* = Server -> Client (563 types)
 *
 * Generated: 2025-11-28
 */

#ifndef __COMMON_PACKET_TYPES_H__
#define __COMMON_PACKET_TYPES_H__

/* ========================================
 * Client -> Server Packet Types (CMDPACKET)
 * msg_no values extracted from production binary
 * ======================================== */

// Core Connection (0-9)
#define CMDPACKET_CHECK_CONNECTION          0   // 连接检查
#define CMDPACKET_LOGIN                     1   // 登录
#define CMDPACKET_SET_UDP_IP_PORT           2   // 设置UDP IP端口
#define CMDPACKET_EXIT                      3   // 退出
#define CMDPACKET_SELECT_CHARACTER          4   // 选择角色
#define CMDPACKET_CREATE_CHARACTER          5   // 创建角色
#define CMDPACKET_DELETE_CHARACTER          6   // 删除角色
#define CMDPACKET_RETURN_SELECT_CHARACTER   7   // 返回角色选择
#define CMDPACKET_GET_USERINFO              8   // 获取用户信息
#define CMDPACKET_RECOVER_STAMINA           9   // 恢复体力

// P2P & Party (10-15)
#define CMDPACKET_REQUEST_PEER              10  // 请求P2P
#define CMDPACKET_RESPONSE_PEER             11  // P2P响应
#define CMDPACKET_SET_PARTY_INFO            12  // 设置组队信息
#define CMDPACKET_LEAVE_PARTY               13  // 离开组队
#define CMDPACKET_WALKOUT_PARTY_MEMBER      14  // 踢出队员
#define CMDPACKET_START_GAME                15  // 开始游戏

// Dungeon & Chat (16-17)
#define CMDPACKET_SELECT_DUNGEON            16  // 选择副本
#define CMDPACKET_SEND_MESSAGE              17  // 发送消息/聊天

// Item Operations (18-29)
#define CMDPACKET_DELETE_ITEM               18  // 删除物品
#define CMDPACKET_MOVE_ITEMSPACE            19  // 移动物品空间
#define CMDPACKET_SORT_ITEM                 20  // 整理物品
#define CMDPACKET_BUY_ITEM                  21  // 购买物品(NPC)
#define CMDPACKET_RENT_AVATAR               22  // 租借时装
#define CMDPACKET_EXTEND_AVATAR             23  // 延长时装
#define CMDPACKET_SELL_ITEM                 24  // 出售物品
#define CMDPACKET_REPAIR_EQUIPMENT          25  // 修理装备
#define CMDPACKET_SET_ITEMTRADE_STATE       26  // 设置交易状态
#define CMDPACKET_COMPOUND_ITEM             27  // 合成物品
#define CMDPACKET_DISJOINT_ITEM             28  // 分解物品
#define CMDPACKET_USE_LOTTERY_ITEM          29  // 使用抽奖物品

// Skills & Stats (30-36)
#define CMDPACKET_CHANGE_SKILLSLOT          30  // 更换技能槽
#define CMDPACKET_BUY_SKILL                 31  // 购买技能
#define CMDPACKET_INCREASE_STATUS           32  // 增加属性
#define CMDPACKET_ACCEPT_QUEST              33  // 接受任务
#define CMDPACKET_GIVEUP_QUEST              34  // 放弃任务
#define CMDPACKET_SET_QUEST_TRIGGER         35  // 设置任务触发
#define CMDPACKET_FINISH_QUEST              36  // 完成任务

// Movement & Area (37-40)
#define CMDPACKET_SET_USER_POSITION         37  // 设置用户位置
#define CMDPACKET_SET_USER_AREA             38  // 设置用户区域
#define CMDPACKET_FISHING                   39  // 钓鱼
#define CMDPACKET_FINISH_LOADING            40  // 完成加载

// Combat (41-51)
#define CMDPACKET_USE_SKILL                 41  // 使用技能
#define CMDPACKET_DIE_MONSTER               42  // 怪物死亡
#define CMDPACKET_DIE_CHARACTER             43  // 角色死亡
#define CMDPACKET_USE_COIN                  44  // 使用复活币
#define CMDPACKET_GIVEUP_GAME               45  // 放弃游戏
#define CMDPACKET_GET_ITEM                  46  // 拾取物品
#define CMDPACKET_USE_STACKABLE             47  // 使用消耗品
#define CMDPACKET_MOVE_MAP                  48  // 移动地图/换房间
#define CMDPACKET_SET_PLAY_RESULT           49  // 设置游戏结果
#define CMDPACKET_DROP_ITEM                 50  // 丢弃物品
#define CMDPACKET_DECREASE_DURABILITY       51  // 降低耐久度

// PVP (52-62)
#define CMDPACKET_REPORT_BAD_P2P_USER       52  // 举报P2P用户
#define CMDPACKET_MAKE_PVP_ROOM             53  // 创建PVP房间
#define CMDPACKET_ENTER_PVP_ROOM            54  // 进入PVP房间
#define CMDPACKET_SET_PVP_SEAT_STATE        55  // 设置PVP座位状态
#define CMDPACKET_SET_PVP_READY_STATE       56  // 设置PVP准备状态
#define CMDPACKET_SET_PVP_TEAM_MODE         57  // 设置PVP团队模式
#define CMDPACKET_DIE_PVP_CHARACTER         58  // PVP角色死亡
#define CMDPACKET_PVP_TIME_OUT              59  // PVP超时
#define CMDPACKET_END_PVP_RESULT            60  // PVP结果
#define CMDPACKET_RES_PVP_RANK              61  // PVP排名响应
#define CMDPACKET_SET_PVP_MAP_INDEX         62  // 设置PVP地图索引

// Social (63-64)
#define CMDPACKET_ADD_FRIEND                63  // 添加好友
#define CMDPACKET_REMOVE_FRIEND             64  // 删除好友

// Debug & CERA (65-69)
#define CMDPACKET_DEBUG_COMMAND             65  // 调试命令
#define CMDPACKET_CERA                      66  // CERA操作
#define CMDPACKET_BUY_CERASHOP_ITEM         67  // 购买商城物品
#define CMDPACKET_GEN_CERATICKET            68  // 生成CERA票据
#define CMDPACKET_REQUEST_PVPEXP_OF_WEEK    69  // 请求周PVP经验

// Guild (70-82)
#define CMDPACKET_GUILD_MEMER_LIST          70  // 公会成员列表
#define CMDPACKET_CALL_GUILD_CREATE_RIGHT   71  // 调用创建公会权限
#define CMDPACKET_SCORE_SCROLL_STATE        72  // 分数滚动状态
#define CMDPACKET_CARD_SELECT_RIGHT_STATE   73  // 卡片选择权限状态
#define CMDPACKET_SELECT_CARD               74  // 选择卡片
#define CMDPACKET_EPLP_COMMAND              75  // EPLP命令
#define CMDPACKET_CALL_GUILD_LEVEL_UP       76  // 调用公会升级
#define CMDPACKET_GUILD_INFO                77  // 公会信息
#define CMDPACKET_REQUEST_GUILD_ENTER       78  // 请求进入公会
#define CMDPACKET_REQUEST_MEMBER_ENTER      79  // 请求成员进入
#define CMDPACKET_MEMBER_ENTER_REPLY        80  // 成员进入回复
#define CMDPACKET_MEMBER_SECEDE             81  // 成员退出
#define CMDPACKET_CALL_MEMER_LIST           82  // 调用成员列表

// Item Enhancement (83-91)
#define CMDPACKET_UPGRADE_ITEM              83  // 强化物品
#define CMDPACKET_RESET_ITEM_ATTR           84  // 重置物品属性
#define CMDPACKET_BUY_PRIVATE_STORE_ITEM    85  // 购买摆摊物品
#define CMDPACKET_ENTER_PRIVATE_STORE       86  // 进入摆摊
#define CMDPACKET_EXIT_PRIVATE_STORE        87  // 退出摆摊
#define CMDPACKET_CREATE_PRIVATE_STORE      88  // 创建摆摊
#define CMDPACKET_REMOVE_PRIVATE_STORE      89  // 移除摆摊
#define CMDPACKET_COMPLETE_DISPLAY          90  // 完成显示
#define CMDPACKET_MOVE_TO_GATE              91  // 移动到传送门

// War Room (92-96)
#define CMDPACKET_MAKE_WARROOM_TEMP         92  // 创建战斗房间(临时)
#define CMDPACKET_ENTER_WARROOM             93  // 进入战斗房间
#define CMDPACKET_SET_WARROOM_SEAT_STATE    94  // 设置战斗房间座位状态
#define CMDPACKET_DIE_WARROOM_CHARACTER     95  // 战斗房间角色死亡
#define CMDPACKET_START_WARROOM_TEMP        96  // 开始战斗房间(临时)

// Mail (97-99)
#define CMDPACKET_MAILBOX_SEND              97  // 发送邮件
#define CMDPACKET_MAILBOX_EXTRACT_ITEM      98  // 提取邮件物品
#define CMDPACKET_MAILBOX_OPEN              99  // 打开邮箱

// Storage/Cargo (100+)
#define CMDPACKET_DEPOSIT_MONEY             100 // 存入金币
#define CMDPACKET_WITHDRAW_MONEY            101 // 取出金币

/* ========================================
 * Server -> Client Packet Types (NOTIPACKET)
 * Response/Notification msg_no values
 * Extracted via: strings df_game_r | grep ENUM_NOTIPACKET_ | nl -v 0
 * ======================================== */

// Core Connection & Info (0-12)
#define NOTIPACKET_CHECK_CONNECTION         0   // 连接检查响应
#define NOTIPACKET_CHANNELINFO              1   // 频道信息
#define NOTIPACKET_USERINFO                 2   // 用户/角色列表信息
#define NOTIPACKET_USER_STATE               3   // 用户状态
#define NOTIPACKET_STAMINA                  4   // 体力值
#define NOTIPACKET_DUNGEON_PERMISSION       5   // 副本权限
#define NOTIPACKET_USER_LEAVE               6   // 用户离开
#define NOTIPACKET_REQUEST_PEER             7   // P2P请求
#define NOTIPACKET_RESPONSE_PEER            8   // P2P响应
#define NOTIPACKET_PARTY_INFO               9   // 组队信息
#define NOTIPACKET_WALKOUT                  10  // 踢出队伍
#define NOTIPACKET_USER_UDP_IP_PORT         11  // UDP端口信息
#define NOTIPACKET_MESSAGE                  12  // 聊天消息

// Item & Inventory (13-18)
#define NOTIPACKET_ITEM_LIST                13  // 物品列表
#define NOTIPACKET_UPDATE_ITEM_LIST         14  // 物品列表更新
#define NOTIPACKET_CHANGE_ITEMTRADE_ITEM    15  // 交易物品变更
#define NOTIPACKET_CANCEL_ITEMTRADE         16  // 取消交易
#define NOTIPACKET_STATE_ITEMTRADE          17  // 交易状态
#define NOTIPACKET_FINISH_ITEMTRADE         18  // 完成交易

// Skills & Quests (19-21)
#define NOTIPACKET_SKILLINFO                19  // 技能信息
#define NOTIPACKET_CHANGE_BONUSSP           20  // 奖励SP变更
#define NOTIPACKET_ACCEPTABLE_QUEST_LIST    21  // 可接任务列表

// Movement & Area (22-26)
#define NOTIPACKET_USER_POSITION            22  // 用户位置
#define NOTIPACKET_USER_AREA                23  // 用户区域
#define NOTIPACKET_AREA_USERS               24  // 区域用户列表
#define NOTIPACKET_FISHING                  25  // 钓鱼状态
#define NOTIPACKET_UDP_HOST                 26  // UDP主机

// Game & Dungeon (27-40)
#define NOTIPACKET_START_GAME               27  // 开始游戏
#define NOTIPACKET_DUNGEON_INFO             28  // 副本信息
#define NOTIPACKET_START_MAP                29  // 开始地图
#define NOTIPACKET_FINISH_LOADING           30  // 完成加载
#define NOTIPACKET_ENABLE_CLEAR_DUNGEON     31  // 启用副本通关
#define NOTIPACKET_DIE_STATE                32  // 死亡状态
#define NOTIPACKET_FAIL_CLEAR_DUNGEON       33  // 副本失败
#define NOTIPACKET_PLAY_RESULT              34  // 游戏结果
#define NOTIPACKET_CLEAR_DUNGEON_REWARD     35  // 通关奖励
#define NOTIPACKET_FATIGUE                  36  // 疲劳值
#define NOTIPACKET_EXP                      37  // 经验值
#define NOTIPACKET_DIE_MONSTER              38  // 怪物死亡
#define NOTIPACKET_GET_ITEM                 39  // 获得物品
#define NOTIPACKET_DROP_ITEM                40  // 丢弃物品

// PVP (41-49)
#define NOTIPACKET_PVP_ROOM_INFO            41  // PVP房间信息
#define NOTIPACKET_PVP_ROOM_STATE           42  // PVP房间状态
#define NOTIPACKET_PVP_SEAT_STATE           43  // PVP座位状态
#define NOTIPACKET_PVP_READY_STATE          44  // PVP准备状态
#define NOTIPACKET_START_PVP                45  // 开始PVP
#define NOTIPACKET_DIE_PVP_CHARACTER        46  // PVP角色死亡
#define NOTIPACKET_END_PVP                  47  // PVP结束
#define NOTIPACKET_PVP_RECORD               48  // PVP记录
#define NOTIPACKET_REQ_PVP_RANK             49  // PVP排名请求

/* ========================================
 * Packet Header Format (from production config)
 * Total header size: 13 bytes
 * ======================================== */
#define PKT_HEADER_CLASSIFICATION_OFFSET    0   // 1 byte: packet class
#define PKT_HEADER_LENGTH_OFFSET            1   // 4 bytes: packet length
#define PKT_HEADER_MSG_NO_OFFSET            5   // 1 byte: message number
#define PKT_HEADER_RESERVED_OFFSET          6   // 1 byte: reserved
#define PKT_HEADER_CHECKSUM_OFFSET          7   // 4 bytes: checksum
#define PKT_HEADER_SEQUENCE_OFFSET          11  // 2 bytes: sequence number
#define PKT_HEADER_SIZE                     13  // Total header size

/* Packet classification values */
#define PKT_CLASS_GAME                      0   // Game server packets
#define PKT_CLASS_INTERNAL                  1   // Internal server packets

#endif /* __COMMON_PACKET_TYPES_H__ */
