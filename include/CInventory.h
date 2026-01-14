/*
 * CInventory.h - 背包/物品栏类定义
 *
 * 基于 df_game_r_v2.c 的偏移量分析恢复
 * 主要参考函数: CInventory::gain_money, CInventory::use_money
 */

#ifndef CINVENTORY_H
#define CINVENTORY_H

#include "../include/defs.h"

// 前向声明
class CUser;
class CUserCharacInfo;

/*
 * CInventory - 角色背包/物品栏类
 *
 * 关键证据:
 * - Line 1427173: if ( *(_DWORD *)a1 )  → 检查 owner 指针
 * - Line 1427175: CUser::get_acc_name(*(CUser **)a1)  → offset 0 是 CUser*
 * - Line 1427180: v19 = *(_DWORD *)(a1 + 4)  → offset 4 是金币
 * - Line 1427183: *(_DWORD *)(a1 + 4) += a2  → 修改金币
 * - Line 1427201: v12 = *(CUserCharacInfo **)a1  → offset 0 也可能是 CUserCharacInfo*
 */
class CInventory {
public:
    //=========================================================================
    // 基础字段 (已确认)
    //=========================================================================

    // Offset 0: 所有者指针
    // 从 gain_money() 函数分析:
    // - Line 1427175: CUser::get_acc_name(*(CUser **)a1)
    // - Line 1427189: CUser::get_charac_no(*(CUser **)a1, -1)
    // - Line 1427201: v12 = *(CUserCharacInfo **)a1
    //
    // 结论: 可能是 union，或者 CUser 包含 CUserCharacInfo
    union {
        CUser *owner_user;            // 作为 CUser* 访问
        CUserCharacInfo *owner_charac; // 作为 CUserCharacInfo* 访问
        void *owner;                   // 通用访问
    };                                // offset 0 (0x00)

    // Offset 4: 金币数量
    // 从 gain_money() 函数分析:
    // - Line 1427180: v19 = *(_DWORD *)(a1 + 4)  → 读取金币
    // - Line 1427183: *(_DWORD *)(a1 + 4) += a2  → 增加金币
    // - Line 1427187: *(_DWORD *)(a1 + 4) = MoneyLimitPerLevel  → 设置上限
    // - Line 1427221: return *(_DWORD *)(a1 + 4) - v19  → 返回变化量
    int money;                        // offset 4 (0x04)

    //=========================================================================
    // 未知字段 (待分析)
    //=========================================================================

    // 偏移量分析: 从 analyze_struct.py 输出看到 offset 3533 (0x0dcd)
    // 估算结构体大小: 至少 3537+ 字节
    // 待逆向分析: item slots, equipment, avatar, creature storage

    uint8_t _unknown_data[3529];     // offset 8 onwards (临时占位)

    //=========================================================================
    // 成员函数声明 (从代码中提取)
    //=========================================================================

    // 金币操作
    int gain_money(unsigned int amount, int reason, bool write_log, int bonus);
    int use_money(int amount, int reason, bool write_log);

    // 待实现成员函数 (Phase 7):
    // - add_item(int item_id, int count, int slot)
    // - delete_item(int slot, int count)
    // - use_item(int slot)
    // - move_item(int from_slot, int to_slot)
    // - get_item_count(int item_id)
};

#endif /* CINVENTORY_H */
