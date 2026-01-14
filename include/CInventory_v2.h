/*
 * CInventory.h - CInventory 结构体定义 (改进版 v2)
 *
 * 基于 df_game_r_v3.c 的完整偏移量分析
 * 总大小: ~3533 bytes
 *
 * 生成日期: 2025年
 * 数据来源: Phase 4 深度分析
 */

#ifndef CINVENTORY_V2_H
#define CINVENTORY_V2_H

#include <stdint.h>

// 前向声明
class CUser;
class CUserCharacInfo;
namespace user_creature {
    class CCreatureMgr;
}
namespace WongWork {
    class CAvatarItemMgr;
}

/*
 * Inven_Item - 物品结构体
 * 大小: 61 bytes
 */
struct Inven_Item
{
    uint8_t data[61];  // 详细字段待分析
};

/*
 * AvatarCoin - 头像货币
 * 大小: 144 bytes
 */
struct AvatarCoin
{
    uint8_t data[144];
};

/*
 * CInventory - 玩家背包系统
 * 大小: ~3533 bytes
 *
 * 管理4种货币和5类物品栏：
 * - 金币、点券、付费点券、活动点券
 * - 装备栏 (26个槽位)
 * - 快捷栏 (12个槽位)
 * - 背包 (312个槽位，外部分配)
 * - 头像 (105个槽位，外部分配)
 * - 宠物 (242个槽位，外部分配)
 */
class CInventory
{
public:
    //=========================================================================
    // 核心字段区域 (0-27 bytes)
    //=========================================================================

    union {
        CUser* owner_user;              // 偏移 0: 所有者(CUser指针)
        CUserCharacInfo* owner_charac;  // 偏移 0: 所有者(CUserCharacInfo指针)
    };

    uint32_t money;       // 偏移 4: 金币
    uint32_t coin;        // 偏移 8: 点券
    uint32_t pay_coin;    // 偏移 12: 付费点券
    uint32_t event_coin;  // 偏移 16: 活动点券

    uint32_t unknown_20;  // 偏移 20: 未知字段

    uint32_t inventory_capacity;  // 偏移 24: 背包容量

    //=========================================================================
    // 装备栏区域 (28-1613 bytes)
    //=========================================================================

    Inven_Item equipment[26];  // 偏移 28: 26个装备槽位 × 61字节

    //=========================================================================
    // 快捷栏区域 (638-1369 bytes)
    //=========================================================================

    // 注: 快捷栏的实际起始位置是 638，这与上面的 equipment[26] 有布局冲突
    // 可能的原因：
    // 1. equipment 数组实际更小
    // 2. quick_slots 和 equipment 有重叠区域
    // 3. 需要重新计算偏移量

    // 为了简化，这里用 padding 填充到 offset 638
    uint8_t _padding_to_638[638 - 28 - 26*61];

    Inven_Item quick_slots[12];  // 偏移 638: 12个快捷槽位

    //=========================================================================
    // 填充到指针区域 (1370-1615 bytes)
    //=========================================================================

    uint8_t _padding_1370[1616 - (638 + 12*61)];

    //=========================================================================
    // 外部背包指针 (1616-1763 bytes)
    //=========================================================================

    Inven_Item* inventory_ptr;  // 偏移 1616: 主背包指针(312槽)
    Inven_Item* avatar_ptr;     // 偏移 1620: 头像背包指针(105槽)

    //=========================================================================
    // 头像货币对象 (1624-1767 bytes)
    //=========================================================================

    AvatarCoin avatar_coin;  // 偏移 1624: 头像货币(144字节)

    //=========================================================================
    // 宠物背包指针 (1764 bytes)
    //=========================================================================

    Inven_Item* creature_ptr;  // 偏移 1764: 宠物背包指针(242槽)

    //=========================================================================
    // 管理器对象 (1768+ bytes)
    //=========================================================================

    user_creature::CCreatureMgr creature_mgr;  // 偏移 1768: 宠物管理器(76字节)

    WongWork::CAvatarItemMgr avatar_item_mgr;  // 偏移 1844: 头像物品管理器(~688字节)

    //=========================================================================
    // 剩余未知区域
    //=========================================================================

    uint8_t _unknown_remaining[3533 - 1844 - 688];  // 填充到总大小

public:
    // 货币操作
    int gain_money(int bonus, unsigned int amount, char* reason, char write_log, int bonus2);
    int use_money(int bonus, int amount, char* reason, char write_log);
    uint32_t get_money() const { return money; }
    void set_money(uint32_t value) { money = value; }

    uint32_t GetCoin() const { return coin; }
    void SetCoin(uint32_t value) { coin = value; }
    uint32_t GetPayCoin() const { return pay_coin; }
    void SetPayCoin(uint32_t value) { pay_coin = value; }
    uint32_t GetEventCoin() const { return event_coin; }
    void SetEventCoin(uint32_t value) { event_coin = value; }
    uint32_t getTotalCoin() const { return coin + pay_coin + event_coin; }

    // 背包容量
    uint32_t get_inventory_capacity() const { return inventory_capacity; }
    void set_inventory_capacity(uint32_t cap) { inventory_capacity = cap; }

    // 背包内存分配
    void SetInventoryMemory(void* buffer_base);

    // 常量
    static const int EQUIPMENT_SLOT_COUNT = 26;
    static const int QUICK_SLOT_COUNT = 12;
    static const int INVENTORY_SLOT_COUNT = 312;
    static const int AVATAR_SLOT_COUNT = 105;
    static const int CREATURE_SLOT_COUNT = 242;
    static const int INVEN_ITEM_SIZE = 61;
};

#endif /* CINVENTORY_V2_H */
