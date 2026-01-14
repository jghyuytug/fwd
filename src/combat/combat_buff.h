/**
 * Combat Buff System - Header
 * Purpose: Manage buffs and debuffs
 *
 * Phase: 6.1 - Combat System Implementation
 * Created: 2025-11-23
 */

#ifndef SRC_COMBAT_COMBAT_BUFF_H_
#define SRC_COMBAT_COMBAT_BUFF_H_

#include <common/defs.h>
#include <time.h>

#define MAX_BUFFS_PER_CHARACTER 32

/**
 * Buff type
 */
typedef enum {
    BUFF_TYPE_BENEFICIAL = 0,  // 增益
    BUFF_TYPE_HARMFUL = 1       // 减益
} BuffType;

/**
 * Buff effect type
 */
typedef enum {
    BUFF_EFFECT_ATTRIBUTE_BONUS = 1,  // 属性加成
    BUFF_EFFECT_DOT = 2,               // 持续伤害
    BUFF_EFFECT_STUN = 3,              // 眩晕
    BUFF_EFFECT_FREEZE = 4,            // 冰冻
    BUFF_EFFECT_INVINCIBLE = 5         // 无敌
} BuffEffectType;

/**
 * Active buff instance
 */
typedef struct {
    int buff_id;
    BuffType type;
    BuffEffectType effect_type;
    int duration_ms;        // 持续时间(毫秒)
    int stack_count;        // 叠加层数
    time_t start_time;      // 开始时间

    // Effect parameters
    int param1;             // 效果参数1 (如属性加成值)
    int param2;             // 效果参数2
} ActiveBuff;

/**
 * Initialize buff system
 * @return 0 on success
 */
int CombatBuff_Initialize(void);

/**
 * Add buff to character
 * @param character_id Character ID
 * @param buff_id Buff ID
 * @param duration_ms Duration in milliseconds
 * @return 0 on success
 */
int CombatBuff_Add(int character_id, int buff_id, int duration_ms);

/**
 * Remove buff from character
 * @param character_id Character ID
 * @param buff_id Buff ID
 * @return 0 on success
 */
int CombatBuff_Remove(int character_id, int buff_id);

/**
 * Update buffs (call every frame)
 * @param character_id Character ID
 * @return Number of buffs removed (expired)
 */
int CombatBuff_Update(int character_id);

/**
 * Get active buffs for character
 * @param character_id Character ID
 * @param out_buffs Output buffer
 * @param max_count Max buffer size
 * @return Actual buff count
 */
int CombatBuff_GetActive(int character_id, ActiveBuff* out_buffs, int max_count);

/**
 * Cleanup buff system
 */
void CombatBuff_Cleanup(void);

#endif /* SRC_COMBAT_COMBAT_BUFF_H_ */
