/**
 * User Module - Character Implementation
 * Purpose: Character structure functions implementation
 */

#include "user/user_character.h"
#include <string.h>
#include <strings.h>  // For strcasecmp
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* ==================== Experience Table ==================== */

// Experience required for each level (1-100)
static const uint64_t EXPERIENCE_TABLE[100] = {
    0, 1000, 2500, 4500, 7000, 10000, 13500, 17500, 22000, 27000,       // 1-10
    32500, 38500, 45000, 52000, 59500, 67500, 76000, 85000, 94500, 104500, // 11-20
    115000, 126000, 137500, 149500, 162000, 175000, 188500, 202500, 217000, 232000, // 21-30
    247500, 263500, 280000, 297000, 314500, 332500, 351000, 370000, 389500, 409500, // 31-40
    430000, 451000, 472500, 494500, 517000, 540000, 563500, 587500, 612000, 637000, // 41-50
    662500, 688500, 715000, 742000, 769500, 797500, 826000, 855000, 884500, 914500, // 51-60
    945000, 976000, 1007500, 1039500, 1072000, 1105000, 1138500, 1172500, 1207000, 1242000, // 61-70
    1277500, 1313500, 1350000, 1387000, 1424500, 1462500, 1501000, 1540000, 1579500, 1619500, // 71-80
    1660000, 1701000, 1742500, 1784500, 1827000, 1870000, 1913500, 1957500, 2002000, 2047000, // 81-90
    2092500, 2138500, 2185000, 2232000, 2279500, 2327500, 2376000, 2425000, 2474500, 2524500  // 91-100
};

/* ==================== Helper Functions ==================== */

/**
 * Calculate required experience for next level
 */
static uint64_t calculate_next_level_exp(uint16_t level)
{
    if (level < 1 || level >= 100) {
        return 0;
    }
    return EXPERIENCE_TABLE[level];
}

/**
 * Calculate HP based on attributes
 */
static int32_t calculate_max_hp(const Character* character)
{
    if (!character) return 100;

    // Base HP + (Vitality * 20) + (Level * 50)
    int32_t base_hp = 100;
    int32_t vit_bonus = character->attributes.vitality * 20;
    int32_t level_bonus = character->level * 50;

    return base_hp + vit_bonus + level_bonus;
}

/**
 * Calculate MP based on attributes
 */
static int32_t calculate_max_mp(const Character* character)
{
    if (!character) return 100;

    // Base MP + (Spirit * 15) + (Level * 30)
    int32_t base_mp = 100;
    int32_t spr_bonus = character->attributes.spirit * 15;
    int32_t level_bonus = character->level * 30;

    return base_mp + spr_bonus + level_bonus;
}

/**
 * Calculate physical attack based on attributes
 */
static int32_t calculate_physical_attack(const Character* character)
{
    if (!character) return 10;

    // Base + (Strength * 2) + (Level * 1)
    return 10 + (character->attributes.strength * 2) + character->level;
}

/**
 * Calculate magical attack based on attributes
 */
static int32_t calculate_magical_attack(const Character* character)
{
    if (!character) return 10;

    // Base + (Intelligence * 2) + (Level * 1)
    return 10 + (character->attributes.intelligence * 2) + character->level;
}

/* ==================== Character Management Functions ==================== */

/**
 * Initialize Character structure with default values
 */
void Character_Initialize(Character* character, uint32_t account_id,
                          const char* name, JobClass job_class, Gender gender)
{
    if (!character) return;

    memset(character, 0, sizeof(Character));

    // Basic info
    character->account_id = account_id;
    strncpy(character->character_name, name, MAX_CHARACTER_NAME_LENGTH - 1);
    character->character_name[MAX_CHARACTER_NAME_LENGTH - 1] = '\0';

    character->status = CHARACTER_ACTIVE;
    character->job_class = job_class;
    character->job_grow_type = JOB_GROW_TYPE_NONE;

    // Starting level
    character->level = 1;
    character->experience = 0;
    character->next_level_exp = calculate_next_level_exp(1);

    // Starting points
    character->sp_points = 0;
    character->tp_points = 0;
    character->stat_points = 0;

    // Appearance
    character->appearance.gender = gender;
    character->appearance.skin_color = 0;
    character->appearance.hair_style = 0;
    character->appearance.hair_color = 0;
    character->appearance.face_style = 0;

    // Starting position (town)
    character->position.current_zone = 0;
    character->position.current_map = 1;
    character->position.position_x = 0.0f;
    character->position.position_y = 0.0f;

    // Base attributes (starting values)
    character->attributes.strength = 10;
    character->attributes.intelligence = 10;
    character->attributes.vitality = 10;
    character->attributes.spirit = 10;

    // Calculate combat attributes
    Character_CalculateAttributes(character);

    // Set HP/MP to max
    character->attributes.current_hp = character->attributes.max_hp;
    character->attributes.current_mp = character->attributes.max_mp;

    // Starting currency
    character->currency.gold = 1000;  // Starting gold
    character->currency.silver = 0;

    // Inventory
    character->inventory_slots_total = 27;  // Default inventory size
    character->inventory_slots_used = 0;

    // Fatigue
    character->fatigue_value = 156;
    character->fatigue_max = 156;

    // Party
    character->party_index = -1;

    // Timestamps
    uint32_t current_time = (uint32_t)time(NULL);
    character->create_date = current_time;
    character->last_login = current_time;
    character->created_at = current_time;
    character->updated_at = current_time;
}

/**
 * Calculate character's combat attributes based on base attributes
 */
void Character_CalculateAttributes(Character* character)
{
    if (!character) return;

    // Calculate max HP and MP
    character->attributes.max_hp = calculate_max_hp(character);
    character->attributes.max_mp = calculate_max_mp(character);

    // Calculate attack
    character->attributes.physical_attack = calculate_physical_attack(character);
    character->attributes.magical_attack = calculate_magical_attack(character);

    // Calculate defense (Vitality and Spirit)
    character->attributes.physical_defense = 5 + (character->attributes.vitality / 2);
    character->attributes.magical_defense = 5 + (character->attributes.spirit / 2);

    // Default speed values
    character->attributes.attack_speed = 100;
    character->attributes.move_speed = 100;
    character->attributes.cast_speed = 100;

    // Default critical values
    character->attributes.critical_rate = 0.05f;  // 5%
    character->attributes.critical_damage = 1.5f;  // 150%

    // HP/MP regen
    character->attributes.hp_regen = 1 + (character->attributes.vitality / 10);
    character->attributes.mp_regen = 1 + (character->attributes.spirit / 10);

    character->updated_at = (uint32_t)time(NULL);
}

/**
 * Add experience to character (with level up check)
 */
int Character_AddExperience(Character* character, uint64_t exp_amount)
{
    if (!character) return 0;

    int levels_gained = 0;
    character->experience += exp_amount;

    // Check for level up
    while (character->level < 100 &&
           character->experience >= character->next_level_exp) {
        if (Character_LevelUp(character)) {
            levels_gained++;
        } else {
            break;
        }
    }

    character->updated_at = (uint32_t)time(NULL);
    return levels_gained;
}

/**
 * Level up character (increase level by 1)
 */
int Character_LevelUp(Character* character)
{
    if (!character) return 0;

    if (character->level >= 100) {
        return 0;  // Already max level
    }

    character->level++;

    // Grant stat points (5 per level)
    character->stat_points += 5;

    // Grant SP points (varies by level)
    if (character->level <= 15) {
        character->sp_points += 1;
    } else if (character->level <= 30) {
        character->sp_points += 2;
    } else {
        character->sp_points += 3;
    }

    // Grant TP points (1 per level after 20)
    if (character->level > 20) {
        character->tp_points += 1;
    }

    // Update next level exp requirement
    character->next_level_exp = calculate_next_level_exp(character->level);

    // Recalculate attributes
    Character_CalculateAttributes(character);

    // Full heal on level up
    character->attributes.current_hp = character->attributes.max_hp;
    character->attributes.current_mp = character->attributes.max_mp;

    character->updated_at = (uint32_t)time(NULL);

    printf("[Character] %s leveled up to %d!\n",
           character->character_name, character->level);

    return 1;
}

/**
 * Add currency to character
 */
int Character_AddCurrency(Character* character, CurrencyType currency_type, uint64_t amount)
{
    if (!character) return 0;

    switch (currency_type) {
        case CURRENCY_GOLD:
            if (character->currency.gold + amount < character->currency.gold) {
                return 0;  // Overflow
            }
            character->currency.gold += amount;
            break;

        case CURRENCY_SILVER:
            if (character->currency.silver + amount < character->currency.silver) {
                return 0;
            }
            character->currency.silver += amount;
            break;

        case CURRENCY_CERA_POINT:
            if (character->currency.cera_point + amount < character->currency.cera_point) {
                return 0;
            }
            character->currency.cera_point += amount;
            break;

        case CURRENCY_MILEAGE:
            character->currency.mileage += amount;
            break;

        case CURRENCY_WIN_POINT:
            character->currency.win_point += amount;
            break;

        default:
            return 0;
    }

    character->updated_at = (uint32_t)time(NULL);
    return 1;
}

/**
 * Remove currency from character
 */
int Character_RemoveCurrency(Character* character, CurrencyType currency_type, uint64_t amount)
{
    if (!character) return 0;

    switch (currency_type) {
        case CURRENCY_GOLD:
            if (character->currency.gold < amount) {
                return 0;  // Insufficient
            }
            character->currency.gold -= amount;
            break;

        case CURRENCY_SILVER:
            if (character->currency.silver < amount) {
                return 0;
            }
            character->currency.silver -= amount;
            break;

        case CURRENCY_CERA_POINT:
            if (character->currency.cera_point < amount) {
                return 0;
            }
            character->currency.cera_point -= amount;
            character->currency.used_cera += amount;
            break;

        case CURRENCY_MILEAGE:
            if (character->currency.mileage < (int32_t)amount) {
                return 0;
            }
            character->currency.mileage -= amount;
            break;

        case CURRENCY_WIN_POINT:
            if (character->currency.win_point < (int32_t)amount) {
                return 0;
            }
            character->currency.win_point -= amount;
            break;

        default:
            return 0;
    }

    character->updated_at = (uint32_t)time(NULL);
    return 1;
}

/**
 * Check if character has sufficient currency
 */
int Character_HasCurrency(const Character* character, CurrencyType currency_type, uint64_t amount)
{
    if (!character) return 0;

    switch (currency_type) {
        case CURRENCY_GOLD:
            return (character->currency.gold >= amount) ? 1 : 0;
        case CURRENCY_SILVER:
            return (character->currency.silver >= amount) ? 1 : 0;
        case CURRENCY_CERA_POINT:
            return (character->currency.cera_point >= amount) ? 1 : 0;
        case CURRENCY_MILEAGE:
            return (character->currency.mileage >= (int32_t)amount) ? 1 : 0;
        case CURRENCY_WIN_POINT:
            return (character->currency.win_point >= (int32_t)amount) ? 1 : 0;
        default:
            return 0;
    }
}

/**
 * Check if character can enter dungeon (fatigue check)
 */
int Character_CanEnterDungeon(const Character* character, uint16_t fatigue_cost)
{
    if (!character) return 0;
    return (character->fatigue_value >= fatigue_cost) ? 1 : 0;
}

/**
 * Consume fatigue for dungeon entry
 */
int Character_ConsumeFatigue(Character* character, uint16_t fatigue_cost)
{
    if (!character) return 0;

    if (character->fatigue_value < fatigue_cost) {
        return 0;  // Insufficient fatigue
    }

    character->fatigue_value -= fatigue_cost;
    character->updated_at = (uint32_t)time(NULL);
    return 1;
}

/**
 * Reset daily fatigue
 */
void Character_ResetFatigue(Character* character)
{
    if (!character) return;

    character->fatigue_value = character->fatigue_max;
    character->last_fatigue_reset = (uint32_t)time(NULL);
    character->updated_at = (uint32_t)time(NULL);
}

/**
 * Join guild
 */
void Character_JoinGuild(Character* character, uint32_t guild_id, uint8_t rank)
{
    if (!character) return;

    character->guild_id = guild_id;
    character->guild_rank = rank;
    character->guild_contribution = 0;
    character->updated_at = (uint32_t)time(NULL);
}

/**
 * Leave guild
 */
void Character_LeaveGuild(Character* character)
{
    if (!character) return;

    character->guild_id = 0;
    character->guild_rank = 0;
    character->guild_contribution = 0;
    character->updated_at = (uint32_t)time(NULL);
}

/**
 * Join party
 */
void Character_JoinParty(Character* character, int16_t party_index, uint8_t position)
{
    if (!character) return;

    character->party_index = party_index;
    character->party_position = position;
    character->updated_at = (uint32_t)time(NULL);
}

/**
 * Leave party
 */
void Character_LeaveParty(Character* character)
{
    if (!character) return;

    character->party_index = -1;
    character->party_position = 0;
    character->updated_at = (uint32_t)time(NULL);
}

/**
 * Update PVP record (after match)
 */
void Character_UpdatePvPRecord(Character* character, int is_win, int32_t points_change)
{
    if (!character) return;

    if (is_win == 1) {
        character->pvp_win_count++;
    } else if (is_win == 0) {
        character->pvp_lose_count++;
    } else {
        character->pvp_draw_count++;
    }

    character->pvp_points += points_change;

    // Calculate win rate
    uint16_t total_matches = character->pvp_win_count + character->pvp_lose_count;
    if (total_matches > 0) {
        character->pvp_win_rate = (float)character->pvp_win_count / (float)total_matches;
    }

    character->updated_at = (uint32_t)time(NULL);
}

/**
 * Mark character for deletion (soft delete with 7-day waiting period)
 */
void Character_MarkForDeletion(Character* character)
{
    if (!character) return;

    uint32_t current_time = (uint32_t)time(NULL);

    character->status = CHARACTER_PENDING_DELETE;
    character->delete_date = current_time;
    character->delete_expire = current_time + (7 * 24 * 3600);  // 7 days
    character->updated_at = current_time;
}

/**
 * Cancel character deletion (before expire)
 */
int Character_CancelDeletion(Character* character)
{
    if (!character) return 0;

    if (character->status != CHARACTER_PENDING_DELETE) {
        return 0;
    }

    uint32_t current_time = (uint32_t)time(NULL);

    if (current_time >= character->delete_expire) {
        return 0;  // Already expired
    }

    character->status = CHARACTER_ACTIVE;
    character->delete_date = 0;
    character->delete_expire = 0;
    character->updated_at = current_time;

    return 1;
}

/**
 * Check if character deletion has expired (ready for permanent delete)
 */
int Character_IsDeletionExpired(const Character* character)
{
    if (!character) return 0;

    if (character->status != CHARACTER_PENDING_DELETE) {
        return 0;
    }

    uint32_t current_time = (uint32_t)time(NULL);
    return (current_time >= character->delete_expire) ? 1 : 0;
}

/**
 * Update character activity timestamp
 */
void Character_UpdateActivity(Character* character)
{
    if (!character) return;

    character->last_logout = (uint32_t)time(NULL);
    character->updated_at = (uint32_t)time(NULL);
}

/**
 * Get character's total power rating (综合战力)
 */
uint32_t Character_GetPowerRating(const Character* character)
{
    if (!character) return 0;

    // Simple power calculation
    // Real DNF uses complex formula, this is simplified
    uint32_t power = 0;

    power += character->level * 100;
    power += character->attributes.physical_attack * 10;
    power += character->attributes.magical_attack * 10;
    power += character->attributes.physical_defense * 5;
    power += character->attributes.magical_defense * 5;
    power += character->attributes.max_hp / 10;
    power += character->attributes.max_mp / 10;

    return power;
}

/**
 * Validate character name
 */
int Character_ValidateName(const char* name)
{
    if (!name) return 0;

    size_t len = strlen(name);

    // Length check (2-16 characters)
    if (len < 2 || len > 16) {
        return 0;
    }

    // Character check (alphanumeric only, no special chars)
    for (size_t i = 0; i < len; i++) {
        if (!isalnum((unsigned char)name[i])) {
            return 0;
        }
    }

    // Reserved names check
    if (strcasecmp(name, "admin") == 0 ||
        strcasecmp(name, "gm") == 0 ||
        strcasecmp(name, "system") == 0) {
        return 0;
    }

    return 1;
}

/**
 * Print character summary (for debugging)
 */
void Character_PrintSummary(const Character* character)
{
    if (!character) {
        printf("Character: NULL\n");
        return;
    }

    printf("========== Character Summary ==========\n");
    printf("Character ID: %u\n", character->character_id);
    printf("Name: %s\n", character->character_name);
    printf("Level: %u (EXP: %llu/%llu)\n",
           character->level, character->experience, character->next_level_exp);
    printf("Job: %d (Grow: %d)\n", character->job_class, character->job_grow_type);
    printf("HP: %d/%d  MP: %d/%d\n",
           character->attributes.current_hp, character->attributes.max_hp,
           character->attributes.current_mp, character->attributes.max_mp);
    printf("P.ATK: %d  M.ATK: %d\n",
           character->attributes.physical_attack, character->attributes.magical_attack);
    printf("Gold: %llu  Cera: %u\n",
           character->currency.gold, character->currency.cera_point);
    printf("Fatigue: %u/%u\n", character->fatigue_value, character->fatigue_max);
    printf("Power Rating: %u\n", Character_GetPowerRating(character));
    printf("=======================================\n");
}
