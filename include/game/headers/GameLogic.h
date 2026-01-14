#ifndef __GAMELOGIC_H__
#define __GAMELOGIC_H__

#include <stdint.h>
#include <stdbool.h>

// ========================================================================
// Type Definitions
// ========================================================================

typedef uint8_t   _BYTE;
typedef int8_t    __int8;
typedef uint16_t  _WORD;
typedef int16_t   __int16;
typedef uint32_t  _DWORD;
typedef int32_t   __int32;
typedef uint64_t  _QWORD;
typedef int64_t   __int64;

typedef bool      _BOOL1;
typedef int16_t   _BOOL2;
typedef int32_t   _BOOL4;

#ifndef __cdecl
  #if defined(__GNUC__) || defined(__clang__)
    #define __cdecl __attribute__((cdecl))
  #else
    #define __cdecl
  #endif
#endif

// ========================================================================
// ========== 1. COMBAT SYSTEM (20+ functions) ==========
// ========================================================================

/**
 * Combat damage calculation engine
 * Handles base damage, bonuses, modifiers, and critical strikes
 */
typedef struct {
    int32_t base_damage;
    int32_t stat_bonus;
    int32_t skill_bonus;
    float type_modifier;
    float critical_rate;
    float critical_multiplier;
    float defense_penetration;
    float damage_reduction;
} STComputeDamageVariable;

// Combat functions
extern int __cdecl Combat_ComputeBaseDamage(int attacker_str, int weapon_damage, int level);
extern int __cdecl Combat_ComputeSkillDamage(int skill_id, int skill_level, int int_stat);
extern float __cdecl Combat_ApplyCriticalDamage(float damage, float crit_rate, float crit_mult);
extern int __cdecl Combat_ApplyDefense(int damage, int defender_def, int def_penetration);
extern int __cdecl Combat_ComputeTotalDamage(STComputeDamageVariable *var);
extern int __cdecl Combat_GetTypeModifier(int attacker_type, int defender_type);
extern _BOOL4 __cdecl Combat_IsCritical(float crit_rate);
extern int __cdecl Combat_GetResistanceDamage(int element_type, int damage, int resistance);
extern int __cdecl Combat_ApplyStatusEffect(int target_uid, int effect_type, int duration);
extern int __cdecl Combat_RemoveStatusEffect(int target_uid, int effect_type);

// ========================================================================
// ========== 2. SKILL SYSTEM (25+ functions) ==========
// ========================================================================

/**
 * Skill data structure
 * Tracks skill properties, upgrades, cooldowns
 */
typedef struct {
    uint32_t skill_id;
    char skill_name[64];
    int level;
    int max_level;
    int mp_cost;
    int cooldown;
    float damage_multiplier;
    float area_of_effect;
    int cast_time;
    int target_type;  // 0=single, 1=aoe, 2=cone
} CSkill;

// Skill functions
extern int __cdecl Skill_Create(uint32_t skill_id, const char *name);
extern int __cdecl Skill_Upgrade(uint32_t skill_id, int *current_level);
extern int __cdecl Skill_GetDamage(uint32_t skill_id, int skill_level, int int_stat);
extern int __cdecl Skill_GetCooldown(uint32_t skill_id);
extern int __cdecl Skill_Execute(uint32_t caster_uid, uint32_t target_uid, uint32_t skill_id);
extern int __cdecl Skill_GetMPCost(uint32_t skill_id, int skill_level);
extern _BOOL4 __cdecl Skill_CanCast(uint32_t caster_uid, uint32_t skill_id);
extern int __cdecl Skill_UpdateCooldown(uint32_t skill_id, int elapsed_ms);
extern int __cdecl Skill_GetCastTime(uint32_t skill_id);
extern int __cdecl Skill_GetAOE(uint32_t skill_id);
extern int __cdecl Skill_GetTargetType(uint32_t skill_id);

// ========================================================================
// ========== 3. QUEST SYSTEM (15+ functions) ==========
// ========================================================================

/**
 * Quest data structure
 * Tracks active quests, progress, rewards
 */
typedef struct {
    uint32_t quest_id;
    int progress;
    int target_count;
    uint32_t start_date;
    uint32_t completion_date;
    _BOOL4 completed;
    int reward_exp;
    int reward_gold;
    int reward_items;  // Item ID
} QuestData;

// Quest functions
extern int __cdecl Quest_Start(uint32_t player_uid, uint32_t quest_id);
extern int __cdecl Quest_Progress(uint32_t player_uid, uint32_t quest_id, int amount);
extern int __cdecl Quest_Complete(uint32_t player_uid, uint32_t quest_id);
extern int __cdecl Quest_GetReward(uint32_t quest_id, int *exp, int *gold);
extern _BOOL4 __cdecl Quest_IsActive(uint32_t player_uid, uint32_t quest_id);
extern _BOOL4 __cdecl Quest_IsCompleted(uint32_t player_uid, uint32_t quest_id);
extern int __cdecl Quest_GetProgress(uint32_t player_uid, uint32_t quest_id);
extern int __cdecl Quest_Abandon(uint32_t player_uid, uint32_t quest_id);
extern int __cdecl Quest_Reset(uint32_t player_uid, uint32_t quest_id);

// ========================================================================
// ========== 4. DUNGEON SYSTEM (15+ functions) ==========
// ========================================================================

/**
 * Dungeon instance data
 * Manages dungeon sessions, progression, rewards
 */
typedef struct {
    uint32_t dungeon_id;
    int difficulty;
    int current_wave;
    int max_waves;
    int enemy_count;
    int player_count;
    uint32_t start_time;
    uint32_t clear_time;
    _BOOL4 cleared;
} CDungeon;

// Dungeon functions
extern int __cdecl Dungeon_Enter(uint32_t player_uid, uint32_t dungeon_id, int difficulty);
extern int __cdecl Dungeon_Progress(uint32_t dungeon_id, int enemy_defeated);
extern int __cdecl Dungeon_Clear(uint32_t dungeon_id);
extern int __cdecl Dungeon_Fail(uint32_t dungeon_id);
extern int __cdecl Dungeon_GetReward(uint32_t dungeon_id, int *exp, int *gold, int *items);
extern int __cdecl Dungeon_GetDropRate(uint32_t dungeon_id, int difficulty);
extern int __cdecl Dungeon_GetClearTime(uint32_t dungeon_id);
extern _BOOL4 __cdecl Dungeon_IsCleared(uint32_t dungeon_id);
extern int __cdecl Dungeon_GetDifficulty(uint32_t dungeon_id);

// ========================================================================
// ========== 5. PARTY SYSTEM (15+ functions) ==========
// ========================================================================

/**
 * Party data structure
 * Manages party formation, member tracking, exp sharing
 */
typedef struct {
    uint32_t party_id;
    char party_name[32];
    int member_count;
    uint32_t leader_uid;
    uint32_t members[6];  // Max 6 members
    _BOOL4 active;
} CParty;

// Party functions
extern int __cdecl Party_Create(uint32_t leader_uid, const char *party_name);
extern int __cdecl Party_Join(uint32_t party_id, uint32_t player_uid);
extern int __cdecl Party_Leave(uint32_t party_id, uint32_t player_uid);
extern int __cdecl Party_Disband(uint32_t party_id);
extern int __cdecl Party_GetMemberCount(uint32_t party_id);
extern _BOOL4 __cdecl Party_IsMember(uint32_t party_id, uint32_t player_uid);
extern int __cdecl Party_GetLeader(uint32_t party_id);
extern int __cdecl Party_ChangeLeader(uint32_t party_id, uint32_t new_leader);
extern int __cdecl Party_ShareExp(uint32_t party_id, int total_exp);

// ========================================================================
// ========== 6. EFFECT & STATUS SYSTEM (15+ functions) ==========
// ========================================================================

/**
 * Active status effect
 * Tracks temporary buffs, debuffs, conditions
 */
typedef struct {
    uint32_t effect_id;
    int effect_type;  // 0=buff, 1=debuff, 2=crowd_control
    int duration_ms;
    float intensity;
} StatusEffect;

// Effect functions
extern int __cdecl Effect_Apply(uint32_t target_uid, StatusEffect *effect);
extern int __cdecl Effect_Remove(uint32_t target_uid, int effect_id);
extern int __cdecl Effect_Update(uint32_t target_uid, int elapsed_ms);
extern _BOOL4 __cdecl Effect_IsActive(uint32_t target_uid, int effect_id);
extern int __cdecl Effect_GetDuration(uint32_t target_uid, int effect_id);
extern int __cdecl Effect_GetIntensity(uint32_t target_uid, int effect_id);
extern int __cdecl Effect_RemoveAll(uint32_t target_uid);

// ========================================================================
// ========== 7. EXPERIENCE & LEVEL SYSTEM (10+ functions) ==========
// ========================================================================

// Level functions
extern int __cdecl Level_GainExp(uint32_t player_uid, int exp_amount);
extern int __cdecl Level_GetLevel(uint32_t player_uid);
extern int __cdecl Level_GetExpToNextLevel(uint32_t player_uid);
extern _BOOL4 __cdecl Level_CheckLevelUp(uint32_t player_uid);
extern int __cdecl Level_LevelUp(uint32_t player_uid);
extern int __cdecl Level_GetStatBonus(int level);

// ========================================================================
// ========== 8. EQUIPMENT & UPGRADE SYSTEM (10+ functions) ==========
// ========================================================================

// Equipment functions
extern int __cdecl Equipment_Upgrade(uint32_t player_uid, uint32_t item_uid, int upgrade_level);
extern int __cdecl Equipment_Amplify(uint32_t player_uid, uint32_t item_uid, int amplify_level);
extern int __cdecl Equipment_Enchant(uint32_t player_uid, uint32_t item_uid, int enchant_id);
extern int __cdecl Equipment_GetPower(uint32_t item_id, int upgrade_level);
extern _BOOL4 __cdecl Equipment_CanUpgrade(uint32_t item_uid);

// ========================================================================
// Note: Additional game systems (Achievements, Guilds, PVP, etc.)
// can be added following the same pattern
// ========================================================================

#endif // __GAMELOGIC_H__
