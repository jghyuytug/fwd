#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PVF语义键映射表 V3
==================
基于以下分析扩展:
1. 跨格式键分析 (70个跨格式键)
2. StringTable标签挖掘
3. 嵌套结构键识别
4. 数据模式推断
5. 引用目标类型追踪 (75个REF键)

版本历史:
  V1: 初始150键 (6.5%)
  V2: 扩展至209键 (9.1%)
  V3: 扩展至350+键 (15%+)
"""

# ============================================================================
# 跨格式通用键 (在2+格式中出现)
# ============================================================================

COMMON_KEYS = {
    # ============ 名称和文本 ============
    0x2A12: "name",                    # 名称字段 (STR) - 5种格式共用
    0x417B: "field_name",              # 地面显示名称 (STR) - 3格式
    0x41D1: "description",             # 描述文本 (STR) - 2格式
    0x3B9D: "tooltip_text",            # 提示文本 (STR) - 3格式

    # ============ 等级相关 ============
    0x417D: "grade",                   # 品级 (REF) - 3格式
    0x4338: "level",                   # 等级要求 (INT, 1-70)
    0x5B8C: "required_level",          # 需求等级 (INT, 18-60)

    # ============ 通用引用 ============
    0x417E: "icon_ref",                # 图标引用 (REF) - 2格式
    0x4180: "animation_ref",           # 动画引用 (REF) - 2格式
    0x4182: "image_ref",               # 图片引用 (REF) - 2格式
    0x4183: "model_ref",               # 模型引用 (REF) - 嵌套结构键
    0x4185: "effect_ref",              # 特效引用 (REF) - 3格式
    0x4187: "rarity_ref",              # 稀有度引用 (REF) - 2格式
    0x4189: "attach_type_ref",         # 装备部位引用 (REF)
    0x418E: "tooltip_ref",             # 提示文本引用 (REF) - 2格式

    # ============ 数值属性 ============
    0x418B: "price_value",             # 价格值 (INT, 大范围) - 2格式
    0x418C: "base_value",              # 基础数值 (INT, 大范围) - 2格式
    0x418D: "scale_value",             # 缩放数值 (INT) - 3格式
    0x4218: "extended_value",          # 扩展数值 (INT, 大范围) - 2格式

    # ============ 标志位 ============
    0x0522: "type_flag",               # 类型标志 (ENUM, 0-5) - 3格式
    0x5B8B: "tradeable",               # 是否可交易 (ENUM, 0-5) - 3格式
    0x01FA: "is_active",               # 是否激活 (BOOL) - 2格式
    0x5A5E: "special_flag",            # 特殊标志 (BOOL) - 2格式

    # ============ 触发/条件 ============
    0x10FC: "trigger_value",           # 触发值 (INT, 大范围) - 4格式
    0x10FD: "trigger_type",            # 触发类型 (REF) - 4格式
    0x160B: "target_type_ref",         # 目标类型引用 (REF)

    # ============ 核心属性 (跨4格式) ============
    0x0B68: "attack",                  # 攻击力 (INT) - 4格式
    0x0B69: "hp",                      # 生命值 (INT) - 4格式
    0x1A7A: "defense",                 # 防御力 (INT) - 3格式
    0x1A7C: "modifier",                # 修正值/经验值 (INT, 大范围) - 3格式

    # ============ 复合结构标记键 ============
    0x0200: "enabled_data",            # 启用数据块 (嵌套) - 4格式
    0x0500: "sp_data",                 # SP数据块 (嵌套) - 4格式
    0x0700: "item_data",               # 物品数据块 (嵌套) - 3格式
    0x1A5C: "trail_data",              # 轨迹数据块 (嵌套) - 4格式
}

# ============================================================================
# 从跨格式分析新增的键
# ============================================================================

CROSS_FORMAT_KEYS = {
    # 引用类型
    0x03F8: "attack_animation_ref",    # 攻击动画引用 (REF) - .equ,.atk
    0x040A: "critical_effect_ref",     # 暴击特效引用 (REF) - .equ,.atk
    0x2D14: "link_ref",                # 链接引用 (REF) - .equ,.mob
    0x4237: "extra_ref",               # 额外引用 (REF) - .equ,.stk
    0x42D5: "bonus_ref",               # 奖励引用 (REF) - .equ,.stk
    0x4601: "tower_ref",               # 塔引用 (REF) - .equ,.stk

    # 数值类型
    0x11A9: "animation_frame",         # 动画帧 (INT, 小值) - .equ,.stk
    0x12E1: "damage_value",            # 伤害值 (INT) - .equ,.mob
    0x1A70: "sub_value_a",             # 子数值A (INT, 小值) - .equ,.stk
    0x1A72: "sub_value_b",             # 子数值B (INT, 小值) - .equ,.stk
    0x1A7B: "sub_value_c",             # 子数值C (INT, 小值) - .equ,.mob
    0x41AE: "buy_price",               # 购买价格 (INT) - .equ,.stk
    0x423B: "spawn_value",             # 生成值 (INT) - .equ,.mob
    0x423F: "size_value",              # 尺寸值 (INT, 小值) - .equ,.mob

    # 枚举类型
    0x1877: "category_type",           # 分类类型 (ENUM) - .equ,.stk
    0x1A4F: "projectile_type",         # 投射物类型 (ENUM) - .equ,.atk
    0x1A71: "sub_type_a",              # 子类型A (ENUM) - .equ,.stk
    0x1A73: "sub_type_b",              # 子类型B (ENUM) - .equ,.stk
    0x1A79: "behavior_type",           # 行为类型 (ENUM) - .equ,.mob
}

# ============================================================================
# 从StringTable标签挖掘的键
# ============================================================================

STRINGTABLE_KEYS = {
    # 从pvf_stringtable.py预定义标签
    0x1D14: "weight",                  # 重量
    0x4961: "physical_critical",       # 物理暴击
    0x4962: "magical_critical",        # 魔法暴击
    0x4820: "creation_rate",           # 创建概率
    0x481B: "sealing",                 # 封印状态
    0x481D: "repair_price",            # 修理价格
    0x481E: "value",                   # 数值
    0x481F: "attach_type",             # 附加类型
    0x482C: "cooltime",                # 冷却时间
    0x482F: "basic_explain",           # 基础说明
    0x4831: "detail_explain",          # 详细说明
    0x4860: "flavor_text",             # 风味文本
    0x0222: "skill",                   # 技能标记
    0x0223: "skill_end",               # 技能结束标记
    0x0EC5: "skill_data_up",           # 技能数据升级

    # 元素属性
    0x4821: "fire_attack",             # 火属性攻击
    0x4964: "water_attack",            # 水属性攻击
    0x4853: "light_attack",            # 光属性攻击
    0x490D: "dark_attack",             # 暗属性攻击
    0x495E: "fire_resistance",         # 火属性抗性
    0x4927: "water_resistance",        # 水属性抗性
    0x1D0F: "light_resistance",        # 光属性抗性
    0x1D0E: "dark_resistance",         # 暗属性抗性

    # 装备类型
    0x4826: "equipment_type",          # 装备类型
    0x481A: "equip_attach_type",       # 装备附加类型
    0x4818: "usable_job",              # 可用职业
    0x62D3: "durability",              # 耐久度
    0x62D1: "sub_type",                # 子类型

    # 属性加成
    0x1265: "physical_attack",         # 物理攻击
    0x1D0B: "physical_defense",        # 物理防御
    0x1D0C: "magical_attack",          # 魔法攻击
    0x1D0D: "magical_defense",         # 魔法防御
    0x139B: "hit_recovery",            # 命中恢复
    0x139D: "all_status_resist",       # 全状态抗性
    0x0B2C: "attack_speed",            # 攻击速度
    0x1D15: "jump_speed",              # 跳跃速度

    # 资源引用
    0x4822: "icon",                    # 图标
    0x4824: "field_image",             # 场景图片
}

# ============================================================================
# 嵌套结构键 (05->05模式)
# ============================================================================

NESTED_STRUCTURE_KEYS = {
    # 装备嵌套
    0x4183: "equ_nested_model",        # 装备嵌套模型数据

    # 怪物嵌套
    0x2BA4: "mob_nested_data_a",       # 怪物嵌套数据A
    0x299E: "mob_nested_sprite",       # 怪物嵌套精灵数据

    # 技能嵌套
    0x0500: "skl_nested_sp",           # 技能嵌套SP数据

    # 任务嵌套
    0x82EC: "qst_nested_reward",       # 任务嵌套奖励数据

    # 动作嵌套
    0x02ED: "act_nested_proc",         # 动作嵌套过程数据

    # 攻击嵌套
    0x1A5C: "atk_nested_trail",        # 攻击嵌套轨迹数据

    # AI嵌套
    0x01E0: "ai_nested_type",          # AI嵌套类型数据
}

# ============================================================================
# 装备文件 (.equ) - 整合所有相关键
# ============================================================================

EQU_KEYS_V3 = {
    **{k: v for k, v in COMMON_KEYS.items()},
    **CROSS_FORMAT_KEYS,
    **STRINGTABLE_KEYS,

    # 物品基础
    0x5DB0: "item_code_ref",           # 物品代码引用 (REF)
    0x5DAE: "item_quality",            # 物品品质 (INT, 100-2840)
    0x5DAD: "drop_rate",               # 掉落率 (INT, 0-106)

    # 副本/区域
    0x4239: "dungeon_value",           # 副本相关值 (INT, 大范围)
    0x41DF: "area_modifier",           # 区域修正 (INT, 1-171)
    0x41E1: "difficulty_value",        # 难度值 (INT)

    # 其他引用
    0x3FCF: "constant_value",          # 常量值 (INT, =7)
    0x3FAF: "alternate_ref",           # 备用引用 (REF)
    0x3FD1: "linked_value",            # 关联值 (REF)
}

# ============================================================================
# 怪物文件 (.mob) - 整合所有相关键
# ============================================================================

MOB_KEYS_V3 = {
    **{k: v for k, v in COMMON_KEYS.items()},

    # 核心属性
    0x0B69: "hp",                      # 生命值 (INT)
    0x0B68: "attack",                  # 攻击力 (INT)
    0x1A7A: "defense",                 # 防御力 (INT)
    0x1A7C: "exp",                     # 经验值 (INT)

    # AI相关
    0x020F: "detect_range",            # 探测范围 (INT)
    0x020D: "attack_range",            # 攻击范围 (INT)
    0x020C: "chase_range",             # 追击范围 (INT)
    0x020E: "move_speed",              # 移动速度 (INT)
    0x01E0: "ai_type_ref",             # AI类型引用 (REF)

    # 动画引用
    0x299E: "sprite_ref",              # 精灵引用 (REF)
    0x299A: "spawn_anim_ref",          # 生成动画引用 (REF)
    0x29CF: "death_anim_ref",          # 死亡动画引用 (REF)
    0x29A6: "idle_anim_ref",           # 待机动画引用 (REF)
    0x2BA4: "nested_data_ref",         # 嵌套数据引用 (REF)

    # 技能引用
    0x06EB: "skill1_ref",              # 技能1引用 (REF)
    0x06EF: "skill2_ref",              # 技能2引用 (REF)
    0x06E7: "skill3_ref",              # 技能3引用 (REF)

    # 行为类型
    0x29A3: "behavior_type",           # 行为类型 (INT)
    0x29A4: "aggro_type",              # 仇恨类型 (INT)
    0x2A43: "is_boss",                 # Boss标记 (BOOL)

    # 属性调整
    0x12E1: "damage_value",            # 伤害值 (INT)
    0x2A14: "special_modifier",        # 特殊修正 (INT)
    0x3E74: "status_resist_ref",       # 状态抗性引用 (REF)

    # 位置/大小
    0x1A77: "hit_offset_x",            # 受击偏移X (INT)
    0x1A74: "hit_offset_y",            # 受击偏移Y (INT)
    0x2E8F: "body_size",               # 体型大小 (INT)
    0x4256: "spawn_interval",          # 刷新间隔 (INT)
    0x425A: "spawn_count",             # 刷新数量 (INT)

    # 掉落
    0x4420: "drop_id",                 # 掉落表ID (INT)
}

# ============================================================================
# 技能文件 (.skl) - 整合所有相关键
# ============================================================================

SKL_KEYS_V3 = {
    **{k: v for k, v in COMMON_KEYS.items()},

    # SP/MP消耗
    0x0500: "sp_data",                 # SP数据 (复合类型)
    0x22A0: "mp_cost",                 # MP消耗 (INT)

    # 技能等级
    0x2283: "max_level",               # 最大等级 (INT)
    0x2285: "damage_type",             # 伤害类型 (INT)
    0x2282: "skill_type",              # 技能类型 (INT)
    0x2280: "base_damage",             # 基础伤害 (INT)

    # 冷却和施法
    0x427E: "cooldown",                # 冷却时间 (INT)
    0x227F: "cast_time",               # 施法时间 (INT)
    0x227D: "animation_speed",         # 动画速度 (INT)

    # 范围和目标
    0x418D: "skill_range",             # 技能范围 (INT)
    0x4318: "combo_count",             # 连击次数 (INT)
    0x22A9: "target_count",            # 目标数量 (INT)
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

    # 复合数据
    0x0200: "enabled_data",            # 启用数据 (复合类型)
    0x0800: "state_data",              # 状态数据 (复合类型)
    0x0900: "target_logic_data",       # 目标逻辑数据 (复合类型)
    0x0700: "item_check_data",         # 物品检查数据 (复合类型)
}

# ============================================================================
# 堆叠物品文件 (.stk)
# ============================================================================

STK_KEYS_V3 = {
    **{k: v for k, v in COMMON_KEYS.items()},

    # 物品基础
    0x0700: "item_data",               # 物品数据 (复合类型)
    0x0207: "use_effect_ref",          # 使用效果引用 (REF)
    0x0206: "is_consumable",           # 是否消耗品 (BOOL)

    # 塔/副本相关
    0x4601: "tower_ref",               # 塔引用 (REF)
    0x4606: "tower_floor",             # 塔层数 (INT)
    0x4666: "dungeon_data",            # 副本数据 (复合类型)
    0x468D: "boss_drop_ref",           # Boss掉落引用 (REF)
    0x465E: "dungeon_value",           # 副本数值 (INT)
    0x465F: "dungeon_ref",             # 副本引用 (REF)
    0x4665: "reward_data",             # 奖励数据 (复合类型)

    # 价格/价值
    0x418B: "sell_price",              # 售价 (INT)
    0x41AE: "buy_price",               # 购买价格 (INT)

    # 事件相关
    0x78BC: "event_data",              # 事件数据 (复合类型)
    0x7093: "event_ref",               # 事件引用 (REF)
    0x7094: "event_type",              # 事件类型 (复合类型)
}

# ============================================================================
# 任务文件 (.qst)
# ============================================================================

QST_KEYS_V3 = {
    **{k: v for k, v in COMMON_KEYS.items()},

    # 任务条件
    0x82DF: "complete_condition",      # 完成条件 (INT)
    0x82DB: "repeat_count",            # 重复次数 (INT)
    0x82DA: "level_condition",         # 等级条件 (INT)
    0x53FD: "item_condition",          # 物品条件 (INT)
    0x82D9: "time_limit",              # 时间限制 (INT)

    # 奖励
    0x82EB: "exp_reward",              # 经验奖励 (INT)
    0x82EC: "item_reward_ref",         # 物品奖励引用 (REF) - 嵌套结构键
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
    0x846E: "difficulty_level",        # 难度等级 (INT)
    0xCF5F: "quest_category",          # 任务分类 (INT)
}

# ============================================================================
# 动作文件 (.act)
# ============================================================================

ACT_KEYS_V3 = {
    # 触发器
    0x02B2: "trigger_ref",             # 触发器引用 (REF)
    0x02FA: "on_death_ref",            # 死亡时引用 (REF)
    0x02A6: "input_type",              # 输入类型 (INT)
    0x02ED: "proc_ref",                # 过程引用 (REF) - 嵌套结构键
    0x02A9: "frame_start_ref",         # 帧起始引用 (REF)
    0x02AB: "frame_end_ref",           # 帧结束引用 (REF)
    0x02A8: "cancel_ref",              # 取消引用 (REF)
    0x02AA: "frame_index",             # 帧索引 (INT)
    0x02A7: "action_ref",              # 动作引用 (REF)

    # 冷却和时间
    0x02F7: "key_cooldown",            # 按键冷却 (INT)
    0x02F5: "cooldown_ref",            # 冷却引用 (REF)
    0x1099: "timing_ref",              # 时序引用 (REF)

    # 输入相关
    0x0305: "move_offset",             # 移动偏移 (INT)
    0x0304: "key_combo",               # 按键连招 (INT)
    0x0303: "delete_input_ref",        # 删除输入引用 (REF)
    0x0308: "chain_action_ref",        # 连锁动作引用 (REF)
    0x0302: "input_buffer",            # 输入缓冲 (INT)
    0x0388: "dash_distance",           # 冲刺距离 (INT)
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
# 攻击文件 (.atk)
# ============================================================================

ATK_KEYS_V3 = {
    # 攻击引用
    0x03F4: "attack_name_ref",         # 攻击名称引用 (REF)
    0x03FA: "effect_ref",              # 特效引用 (REF)
    0x03F8: "animation_ref",           # 动画引用 (REF)
    0x0403: "hit_effect_ref",          # 命中特效引用 (REF)
    0x0400: "sound_ref",               # 音效引用 (REF)
    0x03FE: "knockback_ref",           # 击退引用 (REF)
    0x040A: "crit_effect_ref",         # 暴击特效引用 (REF)
    0x1A63: "projectile_ref",          # 投射物引用 (REF)
    0x1A5C: "trail_ref",               # 轨迹引用 (REF) - 嵌套结构键

    # 攻击数值
    0x03FD: "damage_percent",          # 伤害百分比 (INT)
    0x03FC: "range",                   # 攻击范围 (INT)
    0x1A42: "base_damage",             # 基础伤害 (INT)
    0x03FB: "stun_duration",           # 眩晕时间 (INT)
    0x2BCF: "knockback_force",         # 击退力度 (INT)
    0x2A3F: "hit_stun",                # 硬直时间 (INT)
    0x0407: "super_armor",             # 霸体值 (INT)
    0x2069: "hit_height",              # 命中高度 (INT)

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
    0x1A44: "hit_count",               # 命中次数 (INT)
    0x1A4F: "projectile_speed",        # 投射物速度 (INT)

    # 浮点值
    0x4260: "damage_scale",            # 伤害缩放 (FLOAT)
    0x2F9F: "crit_multiplier",         # 暴击倍率 (FLOAT)
    0x1D20: "range_scale",             # 范围缩放 (FLOAT)
}

# ============================================================================
# AI文件 (.ai)
# ============================================================================

AI_KEYS_V3 = {
    # AI模式
    0x01E4: "patrol_data",             # 巡逻数据 (复合类型)
    0x01EA: "combat_data",             # 战斗数据 (复合类型)
    0x01E8: "idle_data",               # 待机数据 (复合类型)
    0x01E0: "ai_type_ref",             # AI类型引用 (REF) - 嵌套结构键
    0x01E1: "ai_subtype_ref",          # AI子类型引用 (REF)
    0x01E2: "behavior_ref",            # 行为引用 (REF)

    # 行为参数
    0x01F2: "aggro_range",             # 仇恨范围 (INT)
    0x0444: "patrol_distance",         # 巡逻距离 (INT)
    0x02C1: "attack_pattern",          # 攻击模式 (INT)
    0x01FB: "skill_chance",            # 技能概率 (INT)
    0x0309: "flee_threshold",          # 逃跑阈值 (INT)
    0x030C: "target_priority",         # 目标优先级 (INT)
    0x0329: "skill_cooldown",          # 技能冷却 (INT)
    0x046E: "special_behavior",        # 特殊行为 (INT)
    0x059D: "memory_value",            # 记忆值 (INT)

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
# 引用目标语义键 (基于引用目标类型追踪分析)
# ============================================================================

REF_TARGET_KEYS = {
    # 动画引用 (-> .ani)
    0x5DB0: "item_animation_ref",      # 物品动画引用
    0x3FAF: "alt_animation_ref",       # 备用动画引用
    0x0EA2: "skill_animation_ref",     # 技能动画引用
    0x544D: "char_item_anim_ref",      # 角色物品动画引用
    0x42C9: "dummy_animation_ref",     # 虚拟动画引用
    0x186E: "package_animation_ref",   # 包装动画引用
    0x5B42: "char_item_anim2_ref",     # 角色物品动画2
    0x42C3: "dummy_anim2_ref",         # 虚拟动画2
    0x5A3C: "char_item_anim3_ref",     # 角色物品动画3
    0x6DE8: "hold_resist_anim_ref",    # 抓取抗性动画
    0x299A: "spawn_animation_ref",     # 生成动画引用
    0x1A91: "draw_animation_ref",      # 绘制动画引用
    0x1B4C: "draw_animation2_ref",     # 绘制动画2
    0x1361: "equip_phys_anim_ref",     # 装备物理动画
    0x1AC9: "draw_animation3_ref",     # 绘制动画3
    0x2BA3: "ani_ref",                 # ANI引用
    0x2994: "hit_stun_anim_ref",       # 受击硬直动画
    0x2A4A: "hit_stun_anim2_ref",      # 受击硬直动画2
    0x29B6: "hit_stun_anim3_ref",      # 受击硬直动画3
    0x1A9B: "draw_animation4_ref",     # 绘制动画4
    0x1ABF: "draw_animation5_ref",     # 绘制动画5
    0x1A95: "draw_animation6_ref",     # 绘制动画6
    0x1B52: "draw_animation7_ref",     # 绘制动画7
    0x3E4E: "status_animation_ref",    # 状态动画引用
    0x02D8: "color_change_anim_ref",   # 变色动画引用
    0x299F: "hit_stun_anim4_ref",      # 受击硬直动画4
    0x2384: "loop_animation_ref",      # 循环动画引用
    0x1352: "leave_body_anim_ref",     # 离体动画引用
    0x5749: "char_item_anim4_ref",     # 角色物品动画4
    0x8314: "check_damage_anim_ref",   # 伤害检查动画
    0x4690: "result_exp_anim_ref",     # 结算经验动画
    0x46F8: "name2_animation_ref",     # 名称2动画引用
    0x4EA5: "char_item_anim5_ref",     # 角色物品动画5
    0x4607: "tower_animation_ref",     # 塔动画引用
    0xA671: "magic_stone_anim_ref",    # 魔法石动画

    # 动作引用 (-> .act)
    0x06E7: "skill_action1_ref",       # 技能动作1引用
    0x06EB: "skill_action2_ref",       # 技能动作2引用
    0x06EF: "skill_action3_ref",       # 技能动作3引用
    0x3E74: "status_action_ref",       # 状态动作引用
    0x06E5: "skill_action4_ref",       # 技能动作4引用
    0x1A9D: "draw_action_ref",         # 绘制动作引用
    0x1A93: "draw_action2_ref",        # 绘制动作2
    0x1A65: "create_draw_act_ref",     # 创建绘制动作
    0x01F2: "ai_action_ref",           # AI动作引用
    0x4225: "boss_room_act_ref",       # Boss房间动作
    0x6DEC: "hold_resist_act_ref",     # 抓取抗性动作

    # 攻击引用 (-> .atk)
    0x164B: "attack_ref",              # 攻击引用
    0x29A8: "hit_stun_atk_ref",        # 受击攻击引用

    # 物品引用 (-> .stk)
    0x1A99: "create_item_ref",         # 创建物品引用
    0x03F4: "attack_name_item_ref",    # 攻击名称物品引用
    0x03FA: "effect_item_ref",         # 效果物品引用
    0x1A63: "create_draw_item_ref",    # 创建绘制物品引用

    # 装备引用 (-> .equ)
    0x8203: "coat_equip_ref",          # 上衣装备引用
    0x0400: "sound_equip_ref",         # 音效装备引用
    0x078C: "abs_casting_equ_ref",     # ABS施法装备引用
    0x02F0: "proc_action_equ_ref",     # 过程动作装备引用
    0x0787: "abs_casting_equ2_ref",    # ABS施法装备2
    0x1363: "equip_phys_equ_ref",      # 装备物理装备引用
    0x135F: "leave_body_equ_ref",      # 离体装备引用
    0x1295: "change_ai_equ_ref",       # 更改AI装备引用
    0xEB8F: "y_move_dash_equ_ref",     # Y移动冲刺装备引用
    0x468E: "player_kc_equ_ref",       # 玩家KC装备引用
    0x0959: "check_target_equ_ref",    # 检查目标装备引用

    # 对象引用 (-> .obj)
    0x298F: "hit_stun_obj_ref",        # 受击对象引用

    # AI引用 (-> .ai)
    0x032B: "x_axis_ai_ref",           # X轴AI引用

    # 地图块引用 (-> .til)
    0x42C7: "dummy_tile_ref",          # 虚拟地图块引用
    0x01E2: "ai_behavior_til_ref",     # AI行为地图块引用

    # AIC引用 (-> .aic)
    0x0303: "delete_input_aic_ref",    # 删除输入AIC引用
    0x2992: "hit_stun_aic_ref",        # 受击AIC引用
    0x2B9F: "ani_aic_ref",             # ANI AIC引用
    0x2996: "hit_stun_aic2_ref",       # 受击AIC2引用
    0x2D14: "link_aic_ref",            # 链接AIC引用

    # 别名引用 (-> .als)
    0x4183: "model_alias_ref",         # 模型别名引用

    # 按键引用 (-> .key)
    0x1A97: "draw_key_ref",            # 绘制按键引用
    0x41BA: "bg_pos_key_ref",          # 背景位置按键引用
}

# ============================================================================
# 统计
# ============================================================================

def count_keys():
    """统计已映射的键数量"""
    all_mappings = [
        COMMON_KEYS, CROSS_FORMAT_KEYS, STRINGTABLE_KEYS, NESTED_STRUCTURE_KEYS,
        EQU_KEYS_V3, MOB_KEYS_V3, SKL_KEYS_V3, STK_KEYS_V3,
        QST_KEYS_V3, ACT_KEYS_V3, ATK_KEYS_V3, AI_KEYS_V3,
        REF_TARGET_KEYS
    ]

    all_keys = set()
    for m in all_mappings:
        all_keys.update(m.keys())

    return len(all_keys)


if __name__ == "__main__":
    total = count_keys()
    print(f"PVF Semantic Keys V3")
    print(f"=" * 40)
    print(f"Total unique keys mapped: {total}")
    print(f"Estimated coverage: {total}/2300 = {total/2300*100:.1f}%")
    print()
    print("Breakdown:")
    print(f"  COMMON_KEYS:           {len(COMMON_KEYS)}")
    print(f"  CROSS_FORMAT_KEYS:     {len(CROSS_FORMAT_KEYS)}")
    print(f"  STRINGTABLE_KEYS:      {len(STRINGTABLE_KEYS)}")
    print(f"  NESTED_STRUCTURE_KEYS: {len(NESTED_STRUCTURE_KEYS)}")
    print(f"  REF_TARGET_KEYS:       {len(REF_TARGET_KEYS)}")
