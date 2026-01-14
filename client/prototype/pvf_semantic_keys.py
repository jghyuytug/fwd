#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PVF语义键映射表
===============
基于数据分析推断的真实语义映射
"""

# ============================================================================
# 装备文件 (.equ) 键值映射
# ============================================================================

EQU_KEYS = {
    # 基础属性
    0x2A12: "name",              # 装备名称 (字符串)
    0x417E: "icon_ref",          # 图标引用
    0x4182: "image_ref",         # 装备图片引用
    0x4183: "model_ref",         # 模型引用
    0x5DB0: "item_code",         # 物品代码
    0x5DAE: "item_level",        # 物品等级

    # 位置/图形
    0x417B: "field_name",        # 地面显示名称
    0x417D: "grade",             # 品级
    0x4185: "effect_ref",        # 特效引用
    0x4187: "rarity",            # 稀有度
    0x4189: "attach_type",       # 装备部位
    0x418C: "physical_attack",   # 物理攻击
    0x418D: "physical_defense",  # 物理防御
    0x418E: "tooltip_ref",       # 提示文本引用

    # 扩展
    0x1A7C: "move_speed",        # 移动速度加成
    0x0522: "durability",        # 耐久度
    0x5B8B: "tradeable",         # 是否可交易
    0x5B8C: "required_level",    # 需求等级
    0x4239: "class_restriction", # 职业限制
}

# ============================================================================
# 怪物文件 (.mob) 键值映射
# ============================================================================

MOB_KEYS = {
    # 基础属性
    0x0B69: "hp",                # 生命值
    0x0B68: "attack",            # 攻击力
    0x299E: "image_ref",         # 怪物图片引用
    0x12E1: "damage_action",     # 伤害动作
    0x1A7C: "exp",               # 经验值
    0x1A7A: "defense",           # 防御力

    # AI相关
    0x020F: "detect_range",      # 探测范围
    0x020D: "attack_range",      # 攻击范围
    0x020C: "chase_range",       # 追击范围
    0x020E: "move_speed",        # 移动速度
    0x01E0: "ai_type",           # AI类型

    # 状态
    0x299A: "spawn_ref",         # 生成点引用
    0x06EB: "skill1_ref",        # 技能1引用
    0x06EF: "skill2_ref",        # 技能2引用
    0x06E7: "skill3_ref",        # 技能3引用
    0x3E74: "status_resist",     # 状态抗性
    0x29CF: "death_anim_ref",    # 死亡动画引用
    0x2A12: "name",              # 怪物名称 (字符串)
    0x29A4: "drop_table",        # 掉落表
    0x2A14: "boss_flag",         # Boss标记
    0x29A3: "spawn_count",       # 生成数量
    0x1A77: "hit_reaction",      # 受击反应
    0x4256: "respawn_time",      # 重生时间
    0x2A43: "element_type",      # 元素属性
    0x4338: "level",             # 怪物等级
}

# ============================================================================
# 技能文件 (.skl) 键值映射
# ============================================================================

SKL_KEYS = {
    0x0500: "sp_cost",           # SP消耗 (通常是元组)
    0x0200: "enabled",           # 是否启用
    0x4345: "fatigue_cost",      # 疲劳消耗
    0x2283: "max_level",         # 最大等级
    0x2285: "damage_type",       # 伤害类型
    0x2282: "skill_type",        # 技能类型
    0x427E: "cooldown",          # 冷却时间
    0x2286: "skill_ref",         # 技能引用
    0x2A12: "name",              # 技能名称
    0x0800: "state_apply",       # 状态应用
    0x22A1: "effect_ref",        # 特效引用
    0x227F: "cast_time",         # 施法时间
    0x227E: "animation_ref",     # 动画引用
    0x22A0: "mp_cost",           # MP消耗
    0x2281: "required_level",    # 需求等级
    0x2280: "base_damage",       # 基础伤害
    0x418D: "range",             # 技能范围
    0x160B: "target_type",       # 目标类型
    0x4318: "combo_count",       # 连击次数
    0x228A: "hit_count",         # 命中次数
    0x2290: "crit_rate",         # 暴击率
    0x1A5C: "projectile_ref",    # 投射物引用
    0x22FC: "buff_ref",          # Buff引用
    0x4185: "aoe_radius",        # AOE半径
    0x0900: "target_logic",      # 目标逻辑
}

# ============================================================================
# 堆叠物品文件 (.stk) 键值映射
# ============================================================================

STK_KEYS = {
    0x0700: "stack_limit",       # 堆叠上限
    0x0207: "use_effect",        # 使用效果
    0x0500: "item_type",         # 物品类型
    0x0200: "tradeable",         # 可交易
    0x0206: "consume_type",      # 消耗类型
    0x2A12: "name",              # 物品名称
    0x4182: "icon_ref",          # 图标引用
    0x4183: "tooltip_ref",       # 提示文本
    0x417E: "model_ref",         # 模型引用
    0x0522: "cooldown",          # 使用冷却
    0x4185: "effect_ref",        # 特效引用
    0x4601: "tower_reward",      # 塔奖励
    0x418E: "quality",           # 品质
    0x1A7C: "sell_price",        # 售价
    0x10FD: "use_condition",     # 使用条件
    0x4187: "category",          # 分类
    0x4606: "tower_floor",       # 塔层数
    0x4666: "dungeon_ref",       # 副本引用
}

# ============================================================================
# 任务文件 (.qst) 键值映射
# ============================================================================

QST_KEYS = {
    0x417D: "target_id",         # 任务目标ID
    0x10FD: "trigger_type",      # 触发类型
    0x10FC: "trigger_count",     # 触发次数
    0x82DF: "complete_flag",     # 完成标记
    0x82DB: "repeat_type",       # 重复类型
    0x4338: "level_range",       # 等级范围
    0x82E0: "reward_ref",        # 奖励引用
    0x53FD: "item_require",      # 物品需求
    0x1A65: "npc_ref",           # NPC引用
    0x82DA: "exp_reward",        # 经验奖励
    0x82EC: "gold_reward",       # 金币奖励
    0x82EB: "item_reward",       # 物品奖励
    0x160B: "quest_type",        # 任务类型
    0x0500: "dialog",            # 对话
    0x5749: "chain_quest",       # 连锁任务
    0x82D9: "time_limit",        # 时间限制
    0x836E: "faction_reward",    # 阵营奖励
    0x2A12: "name",              # 任务名称
}

# ============================================================================
# 动作文件 (.act) 键值映射
# ============================================================================

ACT_KEYS = {
    0x02B2: "trigger_action",    # 触发动作
    0x02FA: "on_die",            # 死亡时
    0x02A6: "input_trigger",     # 输入触发
    0x02ED: "proc_action",       # 过程动作
    0x02F7: "key_cooldown",      # 按键冷却
    0x0305: "import_key",        # 导入按键
    0x0303: "delete_input",      # 删除输入
    0x02A9: "frame_start",       # 帧起始
    0x0304: "key_combo",         # 按键连招
    0x02AB: "frame_end",         # 帧结束
    0x0301: "speech_trigger",    # 语音触发
    0x0780: "cast_type",         # 施法类型
    0x02BE: "hit_trigger",       # 命中触发
    0x02BD: "damage_trigger",    # 伤害触发
    0x02B3: "move_trigger",      # 移动触发
    0x02F5: "cooldown_start",    # 冷却开始
    0x0353: "create_object",     # 创建对象
    0x02B0: "anim_event",        # 动画事件
    0x02AF: "sound_event",       # 音效事件
    0x0308: "chain_action",      # 连锁动作
    0x02A8: "cancel_trigger",    # 取消触发
    0x1099: "timing",            # 时序
    0x077D: "cast_cancel",       # 施法取消
}

# ============================================================================
# 攻击文件 (.atk) 键值映射
# ============================================================================

ATK_KEYS = {
    0x03F4: "attack_name_ref",   # 攻击名称引用
    0x03FA: "effect_ref",        # 特效引用
    0x03F8: "animation_ref",     # 动画引用
    0x03F7: "hit_count",         # 命中次数
    0x03FD: "damage_percent",    # 伤害百分比
    0x0403: "cooldown",          # 冷却时间
    0x03FC: "range",             # 攻击范围
    0x03F6: "multi_hit",         # 多段攻击
    0x0400: "sound_ref",         # 音效引用
    0x03FE: "knockback_ref",     # 击退引用
    0x1A42: "base_damage",       # 基础伤害
    0x040A: "crit_ref",          # 暴击引用
    0x03FB: "stun_duration",     # 眩晕时间
    0x0407: "super_armor",       # 霸体
    0x2BCF: "animation_speed",   # 动画速度
    0x1A4F: "projectile_speed",  # 投射物速度
    0x1A44: "pierce_count",      # 穿透次数
    0x2A3F: "hit_stun",          # 硬直时间
    0x2067: "hit_height",        # 命中高度
    0x4260: "damage_scale",      # 伤害缩放
}

# ============================================================================
# 通用函数
# ============================================================================

def get_semantic_name(file_type: str, key: int) -> str:
    """获取键的语义名称"""
    mappings = {
        '.equ': EQU_KEYS,
        '.mob': MOB_KEYS,
        '.skl': SKL_KEYS,
        '.stk': STK_KEYS,
        '.qst': QST_KEYS,
        '.act': ACT_KEYS,
        '.atk': ATK_KEYS,
    }

    if file_type in mappings:
        return mappings[file_type].get(key, f"key_0x{key:04X}")

    return f"key_0x{key:04X}"


def get_all_mappings():
    """获取所有映射"""
    return {
        '.equ': EQU_KEYS,
        '.mob': MOB_KEYS,
        '.skl': SKL_KEYS,
        '.stk': STK_KEYS,
        '.qst': QST_KEYS,
        '.act': ACT_KEYS,
        '.atk': ATK_KEYS,
    }


if __name__ == "__main__":
    print("PVF Semantic Key Mappings")
    print("=" * 60)

    for ftype, keys in get_all_mappings().items():
        print(f"\n{ftype} ({len(keys)} keys):")
        for key, name in sorted(keys.items()):
            print(f"  0x{key:04X} = {name}")
