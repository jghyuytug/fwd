#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PVF语义键映射表 V2
==================
基于数据模式分析扩展的语义映射
"""

# ============================================================================
# 跨格式通用键
# ============================================================================

COMMON_KEYS = {
    # 名称和文本
    0x2A12: "name",                    # 名称字段 (STR) - 5种格式共用
    0x417B: "field_name",              # 地面显示名称 (STR)
    0x41D1: "description",             # 描述文本 (STR)

    # 等级相关
    0x417D: "grade",                   # 品级 (INT, 1-69)
    0x4338: "level",                   # 等级要求 (INT, 1-70)
    0x5B8C: "required_level",          # 需求等级 (INT, 18-60)

    # 通用引用
    0x417E: "icon_ref",                # 图标引用 (REF)
    0x4182: "image_ref",               # 图片引用 (REF)
    0x4183: "model_ref",               # 模型引用 (REF)
    0x4185: "effect_ref",              # 特效引用 (REF)
    0x4187: "rarity_ref",              # 稀有度引用 (REF)
    0x418E: "tooltip_ref",             # 提示文本引用 (REF)
    0x4180: "animation_ref",           # 动画引用 (REF)
    0x4189: "attach_type_ref",         # 装备部位引用 (REF)

    # 数值属性
    0x418C: "base_value",              # 基础数值 (INT, 大范围)
    0x418D: "scale_value",             # 缩放数值 (INT)
    0x1A7C: "modifier",                # 修正值 (INT, 0-5000)

    # 标志
    0x0522: "type_flag",               # 类型标志 (INT, 0-5)
    0x5B8B: "tradeable",               # 是否可交易 (INT, 0-5)

    # 触发/条件
    0x10FC: "trigger_value",           # 触发值 (INT, 大范围)
    0x10FD: "trigger_type",            # 触发类型 (REF)
    0x160B: "target_type_ref",         # 目标类型引用 (REF)
}

# ============================================================================
# 装备文件 (.equ) 扩展
# ============================================================================

EQU_KEYS_V2 = {
    **COMMON_KEYS,

    # 物品基础
    0x5DB0: "item_code_ref",           # 物品代码引用 (REF)
    0x5DAE: "item_quality",            # 物品品质 (INT, 100-2840)

    # 属性值
    0x1A72: "sub_modifier",            # 子修正值 (INT, 3-105)
    0x11A9: "animation_frame",         # 动画帧 (INT, 3-105)
    0x3FCF: "constant_value",          # 常量值 (INT, =7)
    0x3FAF: "alternate_ref",           # 备用引用 (REF)
    0x3FD1: "linked_value",            # 关联值 (REF)

    # 副本/区域
    0x4239: "dungeon_value",           # 副本相关值 (INT, 大范围)
    0x41DF: "area_modifier",           # 区域修正 (INT, 1-171)
    0x41E1: "difficulty_value",        # 难度值 (INT)
    0x5DAD: "drop_rate",               # 掉落率 (INT, 0-106)
}

# ============================================================================
# 怪物文件 (.mob) 扩展
# ============================================================================

MOB_KEYS_V2 = {
    **COMMON_KEYS,

    # 核心属性 - 已验证
    0x0B69: "hp",                      # 生命值 (INT, 0-2000)
    0x0B68: "attack",                  # 攻击力 (INT, 0-1200)
    0x1A7A: "defense",                 # 防御力 (INT, 0-2000)
    0x1A7C: "exp",                     # 经验值 (INT, 10000-80000000)

    # AI相关
    0x020F: "detect_range",            # 探测范围 (INT, 0-300)
    0x020D: "attack_range",            # 攻击范围 (INT, 100-2000)
    0x020C: "chase_range",             # 追击范围 (INT, 10-1000)
    0x020E: "move_speed",              # 移动速度 (INT, 0-1000)
    0x01E0: "ai_type_ref",             # AI类型引用 (REF)

    # 动画/外观引用
    0x299E: "sprite_ref",              # 精灵引用 (REF)
    0x299A: "spawn_anim_ref",          # 生成动画引用 (REF)
    0x29CF: "death_anim_ref",          # 死亡动画引用 (REF)
    0x29A6: "idle_anim_ref",           # 待机动画引用 (REF)

    # 技能引用
    0x06EB: "skill1_ref",              # 技能1引用 (REF)
    0x06EF: "skill2_ref",              # 技能2引用 (REF)
    0x06E7: "skill3_ref",              # 技能3引用 (REF)

    # 行为类型
    0x29A3: "behavior_type",           # 行为类型 (INT, 0-3)
    0x29A4: "aggro_type",              # 仇恨类型 (INT, 0-15)
    0x2A43: "is_boss",                 # Boss标记 (BOOL, 0-1)

    # 属性调整
    0x12E1: "damage_value",            # 伤害值 (INT, 0-9000)
    0x2A14: "special_modifier",        # 特殊修正 (INT, 0-170)
    0x3E74: "status_resist_ref",       # 状态抗性引用 (REF)

    # 位置/大小
    0x1A77: "hit_offset_x",            # 受击偏移X (INT, -60~100)
    0x1A74: "hit_offset_y",            # 受击偏移Y (INT, -50~100)
    0x2E8F: "body_size",               # 体型大小 (INT, 0-20000)
    0x4256: "spawn_interval",          # 刷新间隔 (INT)
    0x425A: "spawn_count",             # 刷新数量 (INT, 0-1000)

    # 掉落
    0x4420: "drop_id",                 # 掉落表ID (INT, 大范围)
}

# ============================================================================
# 技能文件 (.skl) 扩展
# ============================================================================

SKL_KEYS_V2 = {
    **COMMON_KEYS,

    # SP/MP消耗
    0x0500: "sp_data",                 # SP数据 (复合类型)
    0x22A0: "mp_cost",                 # MP消耗 (INT, -250~61531)

    # 技能等级
    0x2283: "max_level",               # 最大等级 (INT, 0-30)
    0x2285: "damage_type",             # 伤害类型 (INT, 0-4)
    0x2282: "skill_type",              # 技能类型 (INT, 0-4)
    0x2280: "base_damage",             # 基础伤害 (INT, 1-169)

    # 冷却和施法
    0x427E: "cooldown",                # 冷却时间 (INT, 1-50)
    0x227F: "cast_time",               # 施法时间 (INT, 1-5)
    0x227D: "animation_speed",         # 动画速度 (INT, 0-60)

    # 范围和目标
    0x418D: "skill_range",             # 技能范围 (INT, 500-150000)
    0x4318: "combo_count",             # 连击次数 (INT, 1-15)
    0x22A9: "target_count",            # 目标数量 (INT, 0-4)
    0x228E: "is_passive",              # 是否被动 (BOOL)

    # 引用
    0x4345: "prerequisite_ref",        # 前置技能引用 (REF)
    0x2286: "skill_tree_ref",          # 技能树引用 (REF)
    0x22A1: "effect_anim_ref",         # 特效动画引用 (REF)
    0x227E: "cast_anim_ref",           # 施法动画引用 (REF)
    0x2281: "hit_anim_ref",            # 命中动画引用 (REF)
    0x228A: "projectile_ref",          # 投射物引用 (REF)
    0x2290: "buff_ref",                # Buff引用 (REF)
    0x22FC: "debuff_ref",              # Debuff引用 (REF)
    0x22AA: "sound_ref",               # 音效引用 (REF)

    # 状态
    0x0200: "enabled_data",            # 启用数据 (复合类型)
    0x0800: "state_data",              # 状态数据 (复合类型)
    0x0900: "target_logic_data",       # 目标逻辑数据 (复合类型)
    0x0700: "item_check_data",         # 物品检查数据 (复合类型)
}

# ============================================================================
# 堆叠物品文件 (.stk) 扩展
# ============================================================================

STK_KEYS_V2 = {
    **COMMON_KEYS,

    # 物品基础
    0x0700: "item_data",               # 物品数据 (复合类型)
    0x0207: "use_effect_ref",          # 使用效果引用 (REF)
    0x0206: "is_consumable",           # 是否消耗品 (BOOL)

    # 塔/副本相关
    0x4601: "tower_ref",               # 塔引用 (REF)
    0x4606: "tower_floor",             # 塔层数 (INT, 1-1000)
    0x4666: "dungeon_data",            # 副本数据 (复合类型)
    0x468D: "boss_drop_ref",           # Boss掉落引用 (REF)
    0x465E: "dungeon_value",           # 副本数值 (INT, 大范围)
    0x465F: "dungeon_ref",             # 副本引用 (REF)
    0x4665: "reward_data",             # 奖励数据 (复合类型)

    # 价格/价值
    0x418B: "sell_price",              # 售价 (INT, 0-300000)
    0x41AE: "buy_price",               # 购买价格 (INT, 0-2000)

    # 事件相关
    0x78BC: "event_data",              # 事件数据 (复合类型)
    0x7093: "event_ref",               # 事件引用 (REF)
    0x7094: "event_type",              # 事件类型 (复合类型)
}

# ============================================================================
# 任务文件 (.qst) 扩展
# ============================================================================

QST_KEYS_V2 = {
    **COMMON_KEYS,

    # 任务条件
    0x82DF: "complete_condition",      # 完成条件 (INT, 大范围)
    0x82DB: "repeat_count",            # 重复次数 (INT, -1~66)
    0x82DA: "level_condition",         # 等级条件 (INT, 1-95)
    0x53FD: "item_condition",          # 物品条件 (INT, -1~3)
    0x82D9: "time_limit",              # 时间限制 (INT, 20-600)

    # 奖励
    0x82EB: "exp_reward",              # 经验奖励 (INT, 5-7836)
    0x82EC: "item_reward_ref",         # 物品奖励引用 (REF)
    0x82E0: "gold_reward_ref",         # 金币奖励引用 (REF)
    0x836E: "faction_reward",          # 阵营奖励 (BOOL)

    # NPC引用
    0x1A65: "npc_ref",                 # NPC引用 (REF)
    0x5749: "chain_quest_ref",         # 连锁任务引用 (REF)
    0x8314: "dialog_ref",              # 对话引用 (REF)
    0x8315: "dialog_data",             # 对话数据 (复合类型)

    # 其他条件
    0x82E2: "skill_condition_ref",     # 技能条件引用 (REF)
    0x8300: "dungeon_condition_ref",   # 副本条件引用 (REF)
    0x82DD: "kill_condition_ref",      # 击杀条件引用 (REF)
    0x85C4: "collect_condition_ref",   # 收集条件引用 (REF)
    0x8DEC: "special_condition_ref",   # 特殊条件引用 (REF)
    0x164E: "is_daily",                # 是否日常 (BOOL)
    0x846E: "difficulty_level",        # 难度等级 (INT, 1-20)
    0xCF5F: "quest_category",          # 任务分类 (INT, 0-14)
}

# ============================================================================
# 动作文件 (.act) 扩展
# ============================================================================

ACT_KEYS_V2 = {
    # 触发器
    0x02B2: "trigger_ref",             # 触发器引用 (REF)
    0x02FA: "on_death_ref",            # 死亡时引用 (REF)
    0x02A6: "input_type",              # 输入类型 (INT, 0-9)
    0x02ED: "proc_ref",                # 过程引用 (REF)
    0x02A9: "frame_start_ref",         # 帧起始引用 (REF)
    0x02AB: "frame_end_ref",           # 帧结束引用 (REF)
    0x02A8: "cancel_ref",              # 取消引用 (REF)
    0x02AA: "frame_index",             # 帧索引 (INT, 0-18)
    0x02A7: "action_ref",              # 动作引用 (REF)

    # 冷却和时间
    0x02F7: "key_cooldown",            # 按键冷却 (INT, 0-103)
    0x02F5: "cooldown_ref",            # 冷却引用 (REF)
    0x1099: "timing_ref",              # 时序引用 (REF)

    # 输入相关
    0x0305: "move_offset",             # 移动偏移 (INT, -300~1515)
    0x0304: "key_combo",               # 按键连招 (INT, 0-70)
    0x0303: "delete_input_ref",        # 删除输入引用 (REF)
    0x0308: "chain_action_ref",        # 连锁动作引用 (REF)
    0x0302: "input_buffer",            # 输入缓冲 (INT, 大范围)
    0x0388: "dash_distance",           # 冲刺距离 (INT, 0-5000)
    0x037F: "input_flag",              # 输入标志 (BOOL)

    # 其他
    0x0301: "speech_ref",              # 语音引用 (REF)
    0x02BD: "damage_ref",              # 伤害引用 (REF)
    0x0780: "cast_type_ref",           # 施法类型引用 (REF)
    0x02BE: "hit_ref",                 # 命中引用 (REF)
    0x02B3: "move_ref",                # 移动引用 (REF)
    0x0353: "create_object_ref",       # 创建对象引用 (REF)
    0x02B0: "anim_event_ref",          # 动画事件引用 (REF)
    0x02AF: "sound_event_ref",         # 音效事件引用 (REF)
    0x077D: "cast_cancel_ref",         # 施法取消引用 (REF)
    0x098D: "effect_ref",              # 效果引用 (REF)
    0x0381: "state_ref",               # 状态引用 (REF)
}

# ============================================================================
# 攻击文件 (.atk) 扩展
# ============================================================================

ATK_KEYS_V2 = {
    # 攻击引用
    0x03F4: "attack_name_ref",         # 攻击名称引用 (REF)
    0x03FA: "effect_ref",              # 特效引用 (REF)
    0x03F8: "animation_ref",           # 动画引用 (REF)
    0x0403: "hit_effect_ref",          # 命中特效引用 (REF)
    0x0400: "sound_ref",               # 音效引用 (REF)
    0x03FE: "knockback_ref",           # 击退引用 (REF)
    0x040A: "crit_effect_ref",         # 暴击特效引用 (REF)
    0x1A63: "projectile_ref",          # 投射物引用 (REF)
    0x1A5C: "trail_ref",               # 轨迹引用 (REF)

    # 攻击数值
    0x03FD: "damage_percent",          # 伤害百分比 (INT, 0-1000)
    0x03FC: "range",                   # 攻击范围 (INT, 0-1000)
    0x1A42: "base_damage",             # 基础伤害 (INT, -95~300)
    0x03FB: "stun_duration",           # 眩晕时间 (INT, -1000~40000)
    0x2BCF: "knockback_force",         # 击退力度 (INT, -100~40000)
    0x2A3F: "hit_stun",                # 硬直时间 (INT, 1-100)
    0x0407: "super_armor",             # 霸体值 (INT, -1~1000)
    0x2069: "hit_height",              # 命中高度 (INT, -1000~200)

    # 布尔标志
    0x03F7: "is_multi_hit",            # 是否多段 (BOOL)
    0x03F6: "is_piercing",             # 是否穿透 (BOOL)
    0x2067: "is_aoe",                  # 是否范围 (BOOL)
    0x1A46: "is_projectile",           # 是否投射 (BOOL)
    0x3E66: "ignore_defense",          # 无视防御 (BOOL)
    0x6229: "ignore_super_armor",      # 无视霸体 (BOOL)
    0x46CB: "is_grab",                 # 是否抓取 (BOOL)
    0x5459: "is_unblockable",          # 是否无法格挡 (BOOL)

    # 其他数值
    0x1A44: "hit_count",               # 命中次数 (INT, 1-100)
    0x1A4F: "projectile_speed",        # 投射物速度 (INT, 负值)

    # 浮点值
    0x4260: "damage_scale",            # 伤害缩放 (FLOAT, 100.0)
    0x2F9F: "crit_multiplier",         # 暴击倍率 (FLOAT, 30-150)
    0x1D20: "range_scale",             # 范围缩放 (FLOAT, 100.0)
}

# ============================================================================
# AI文件 (.ai) 扩展
# ============================================================================

AI_KEYS_V2 = {
    # AI模式
    0x01E4: "patrol_data",             # 巡逻数据 (复合类型)
    0x01EA: "combat_data",             # 战斗数据 (复合类型)
    0x01E8: "idle_data",               # 待机数据 (复合类型)
    0x01E0: "ai_type_ref",             # AI类型引用 (REF)
    0x01E1: "ai_subtype_ref",          # AI子类型引用 (REF)
    0x01E2: "behavior_ref",            # 行为引用 (REF)

    # 行为参数
    0x01F2: "aggro_range",             # 仇恨范围 (INT, -1~10)
    0x0444: "patrol_distance",         # 巡逻距离 (INT, 2-190)
    0x02C1: "attack_pattern",          # 攻击模式 (INT, 0-11)
    0x01FB: "skill_chance",            # 技能概率 (INT, 0-68)
    0x0309: "flee_threshold",          # 逃跑阈值 (INT, 0-11)
    0x030C: "target_priority",         # 目标优先级 (INT, 1-2)
    0x0329: "skill_cooldown",          # 技能冷却 (INT, 3-175)
    0x046E: "special_behavior",        # 特殊行为 (INT, 7-74)
    0x059D: "memory_value",            # 记忆值 (INT, 0-10603)

    # 引用
    0x02C2: "skill_ref",               # 技能引用 (REF)
    0x032B: "target_ref",              # 目标引用 (REF)
    0x01E6: "special_ref",             # 特殊引用 (REF)
    0x01FC: "chain_ai_ref",            # 连锁AI引用 (REF)
    0x02BF: "event_ref",               # 事件引用 (REF)
    0x01F7: "state_ref",               # 状态引用 (REF)
    0x04CA: "spawn_ref",               # 生成引用 (REF)
    0x0595: "death_ref",               # 死亡引用 (REF)
    0x046B: "idle_ref",                # 待机引用 (REF)
    0x08F7: "boss_phase_ref",          # Boss阶段引用 (REF)
    0x046C: "combat_ref",              # 战斗引用 (REF)
    0x0430: "alert_ref",               # 警戒引用 (REF)

    # 布尔标志
    0x01FA: "is_aggressive",           # 是否主动 (BOOL)
    0x05C6: "can_flee",                # 是否逃跑 (BOOL)
    0x07BB: "is_summoned",             # 是否召唤 (BOOL)
}

# ============================================================================
# 统计
# ============================================================================

def count_keys():
    """统计已映射的键数量"""
    all_mappings = [
        COMMON_KEYS, EQU_KEYS_V2, MOB_KEYS_V2, SKL_KEYS_V2,
        STK_KEYS_V2, QST_KEYS_V2, ACT_KEYS_V2, ATK_KEYS_V2, AI_KEYS_V2
    ]

    all_keys = set()
    for m in all_mappings:
        all_keys.update(m.keys())

    return len(all_keys)


if __name__ == "__main__":
    total = count_keys()
    print(f"Total unique keys mapped: {total}")
    print(f"Estimated coverage: {total}/2300 = {total/2300*100:.1f}%")
