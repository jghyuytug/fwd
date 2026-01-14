#include "systems/combat_system.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db/db_game.h"
#include "systems/buff_system.h"
#include "systems/time_utils.h"

/* Calculate combat stats based on level, job, and equipment */
void calculate_combat_stats(ClientSession* s) {
    if (!s) return;

    CombatStats* stats = &s->combat_stats;
    memset(stats, 0, sizeof(CombatStats));

    /* Find current character info */
    DBCharacterInfo* charac = NULL;
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            charac = &s->characters[i];
            break;
        }
    }
    if (!charac) return;

    uint8_t level = charac->level;
    uint8_t job = charac->job;

    /* Base stats by job type (simplified formula) */
    /* DNF formula: base + (level - 1) * growth_rate */
    switch (job) {
        case JOB_SLAYER:  /* STR based melee */
            stats->str = 30 + level * 3;
            stats->intel = 15 + level * 1;
            stats->vit = 25 + level * 2;
            stats->spr = 15 + level * 1;
            break;
        case JOB_FIGHTER:  /* STR based melee */
            stats->str = 28 + level * 3;
            stats->intel = 12 + level * 1;
            stats->vit = 28 + level * 2;
            stats->spr = 12 + level * 1;
            break;
        case JOB_GUNNER:  /* STR/INT hybrid */
            stats->str = 22 + level * 2;
            stats->intel = 22 + level * 2;
            stats->vit = 20 + level * 2;
            stats->spr = 18 + level * 1;
            break;
        case JOB_MAGE:  /* INT based caster */
            stats->str = 10 + level * 1;
            stats->intel = 35 + level * 4;
            stats->vit = 15 + level * 1;
            stats->spr = 25 + level * 2;
            break;
        case JOB_PRIEST:  /* INT/SPR hybrid */
            stats->str = 18 + level * 1;
            stats->intel = 25 + level * 3;
            stats->vit = 22 + level * 2;
            stats->spr = 28 + level * 3;
            break;
        default:  /* THIEF or unknown */
            stats->str = 25 + level * 2;
            stats->intel = 18 + level * 1;
            stats->vit = 22 + level * 2;
            stats->spr = 15 + level * 1;
            break;
    }

    /* Equipment bonuses - will be applied after template table is defined */
    /* Using simplified formula for now, enhanced via apply_equipment_bonuses() */
    int equip_count = 0;
    for (int i = 0; i < MAX_EQUIP_SLOTS; i++) {
        uint32_t item_id = s->equipment.equipment[i].it_id;
        if (item_id == 0) continue;
        equip_count++;

        /* Simplified formula based on item_id range and grade */
        uint8_t enhance = s->equipment.equipment[i].stat;
        float enhance_mult = (enhance == 0) ? 1.0f : (1.0f + enhance * 0.03f);
        if (enhance > 10) enhance_mult = 1.30f + (enhance - 10) * 0.05f;

        /* Determine item tier from item_id sub-range (every 20 items = higher tier) */
        uint32_t tier = ((item_id % 100) / 20);  /* 0-4 tiers */
        uint32_t base_power = 30 + tier * 80;    /* 30/110/190/270/350 */

        switch (i) {
            case EQUIP_SLOT_WEAPON:
                /* Weapon provides attack based on job compatibility */
                if (item_id >= 50000 && item_id < 50100) {  /* Slayer sword */
                    stats->phys_attack += (uint32_t)(base_power * 2.5f * enhance_mult);
                    stats->str += tier * 5 + 3;
                } else if (item_id >= 50100 && item_id < 50200) {  /* Mage staff */
                    stats->mag_attack += (uint32_t)(base_power * 2.8f * enhance_mult);
                    stats->intel += tier * 5 + 4;
                } else if (item_id >= 50200 && item_id < 50300) {  /* Gunner gun */
                    stats->phys_attack += (uint32_t)(base_power * 1.5f * enhance_mult);
                    stats->mag_attack += (uint32_t)(base_power * 1.5f * enhance_mult);
                    stats->str += tier * 3 + 2;
                    stats->intel += tier * 3 + 2;
                } else if (item_id >= 50300 && item_id < 50400) {  /* Fighter gauntlet */
                    stats->phys_attack += (uint32_t)(base_power * 2.3f * enhance_mult);
                    stats->str += tier * 5 + 4;
                    stats->max_hp += tier * 50;
                } else if (item_id >= 50400 && item_id < 50500) {  /* Priest cross */
                    stats->phys_attack += (uint32_t)(base_power * 1.2f * enhance_mult);
                    stats->mag_attack += (uint32_t)(base_power * 1.8f * enhance_mult);
                    stats->intel += tier * 4 + 3;
                    stats->spr += tier * 3 + 2;
                } else if (item_id >= 50500 && item_id < 50600) {  /* Thief dagger */
                    stats->phys_attack += (uint32_t)(base_power * 2.2f * enhance_mult);
                    stats->str += tier * 4 + 2;
                    stats->crit_rate += tier * 50 + 30;
                } else {  /* Generic weapon */
                    stats->phys_attack += (uint32_t)(base_power * 2.0f * enhance_mult);
                }
                break;

            case EQUIP_SLOT_CHEST:
                stats->phys_def += (uint32_t)((25 + tier * 50) * enhance_mult);
                stats->mag_def += (uint32_t)((20 + tier * 40) * enhance_mult);
                stats->max_hp += tier * 100 + 80;
                stats->vit += tier * 4 + 3;
                break;

            case EQUIP_SLOT_PANTS:
                stats->phys_def += (uint32_t)((20 + tier * 45) * enhance_mult);
                stats->mag_def += (uint32_t)((16 + tier * 36) * enhance_mult);
                stats->max_hp += tier * 80 + 60;
                stats->vit += tier * 3 + 2;
                break;

            case EQUIP_SLOT_HELMET:
                stats->phys_def += (uint32_t)((15 + tier * 35) * enhance_mult);
                stats->mag_def += (uint32_t)((12 + tier * 28) * enhance_mult);
                stats->str += tier + 1;
                stats->intel += tier + 1;
                stats->max_hp += tier * 50 + 30;
                break;

            case EQUIP_SLOT_SHOULDER:
                stats->phys_def += (uint32_t)((12 + tier * 30) * enhance_mult);
                stats->mag_def += (uint32_t)((10 + tier * 24) * enhance_mult);
                stats->str += tier * 2 + 2;
                stats->max_hp += tier * 40 + 20;
                break;

            case EQUIP_SLOT_SHOES:
                stats->phys_def += (uint32_t)((10 + tier * 25) * enhance_mult);
                stats->mag_def += (uint32_t)((8 + tier * 20) * enhance_mult);
                stats->move_speed += tier * 3 + 3;
                stats->vit += tier + 1;
                break;

            case EQUIP_SLOT_BELT:
                stats->phys_def += (uint32_t)((8 + tier * 18) * enhance_mult);
                stats->mag_def += (uint32_t)((6 + tier * 14) * enhance_mult);
                stats->str += tier + 1;
                stats->intel += tier + 1;
                stats->vit += tier + 1;
                stats->spr += tier + 1;
                stats->max_hp += tier * 40 + 25;
                stats->max_mp += tier * 25 + 15;
                break;

            case EQUIP_SLOT_BRACELET:
                stats->phys_attack += (uint32_t)((5 + tier * 15) * enhance_mult);
                stats->mag_attack += (uint32_t)((5 + tier * 15) * enhance_mult);
                stats->str += tier * 3 + 2;
                stats->intel += tier * 3 + 2;
                stats->crit_rate += tier * 25 + 20;
                break;

            case EQUIP_SLOT_NECKLACE:
                stats->phys_attack += (uint32_t)((3 + tier * 10) * enhance_mult);
                stats->mag_attack += (uint32_t)((8 + tier * 22) * enhance_mult);
                stats->intel += tier * 4 + 3;
                stats->spr += tier * 2 + 1;
                stats->max_mp += tier * 40 + 20;
                stats->crit_rate += tier * 20 + 15;
                break;

            case EQUIP_SLOT_RING:
                stats->phys_attack += (uint32_t)((8 + tier * 20) * enhance_mult);
                stats->str += tier * 4 + 3;
                stats->crit_rate += tier * 30 + 30;
                break;

            case EQUIP_SLOT_SUBWEAPON:
                /* Subweapons: shields, totems, orbs */
                stats->phys_def += (uint32_t)((15 + tier * 30) * enhance_mult);
                stats->mag_def += (uint32_t)((15 + tier * 30) * enhance_mult);
                stats->max_hp += tier * 60 + 40;
                break;
        }
    }
    printf("[EQUIP] Applied %d equipment bonuses\n", equip_count);

    /* Calculate derived stats */
    stats->phys_attack += stats->str * 2;
    stats->mag_attack += stats->intel * 2;
    stats->phys_def += stats->vit;
    stats->mag_def += stats->spr;
    stats->max_hp = 100 + stats->vit * 30 + level * 50;
    stats->max_mp = 50 + stats->spr * 20 + level * 30;
    stats->attack_speed = 100;  /* Base 100% */
    stats->move_speed = 100;    /* Base 100% */
    stats->cast_speed = 100;    /* Base 100% */
    stats->crit_rate = 300 + level * 10;  /* 3% base + 0.1% per level */
    stats->hit_rate = 9000 + level * 50;  /* 90% base */
    stats->evasion = 500 + level * 20;    /* 5% base */

    printf("[STATS] Calculated for charac_no=%u (job=%d, lv=%d): STR=%u INT=%u PHY_ATK=%u MAG_ATK=%u HP=%u\n",
           s->character_id, job, level, stats->str, stats->intel,
           stats->phys_attack, stats->mag_attack, stats->max_hp);
}

/* ========================================
 * Equipment Stats System
 * Based on item_id ranges from production data
 * ======================================== */

/* Item grade/rarity */
typedef enum {
    ITEM_GRADE_COMMON = 0,      /* White */
    ITEM_GRADE_UNCOMMON,        /* Green */
    ITEM_GRADE_RARE,            /* Blue */
    ITEM_GRADE_UNIQUE,          /* Purple */
    ITEM_GRADE_LEGENDARY,       /* Orange */
    ITEM_GRADE_EPIC             /* Pink */
} ItemGrade;

/* Equipment stats by item template */
typedef struct {
    uint32_t item_id_min;   /* Starting item_id range */
    uint32_t item_id_max;   /* Ending item_id range */
    uint8_t  slot_type;     /* EQUIP_SLOT_* */
    uint8_t  grade;         /* ItemGrade */
    uint8_t  level_req;     /* Required level */
    int16_t  phys_attack;   /* Physical attack bonus */
    int16_t  mag_attack;    /* Magic attack bonus */
    int16_t  phys_def;      /* Physical defense */
    int16_t  mag_def;       /* Magic defense */
    int16_t  str_bonus;     /* STR bonus */
    int16_t  int_bonus;     /* INT bonus */
    int16_t  vit_bonus;     /* VIT bonus */
    int16_t  spr_bonus;     /* SPR bonus */
    int16_t  hp_bonus;      /* Max HP bonus */
    int16_t  mp_bonus;      /* Max MP bonus */
    int16_t  crit_bonus;    /* Crit rate bonus (0.01%) */
    int16_t  speed_bonus;   /* Attack/move speed bonus */
} EquipmentTemplate;

/* Equipment template table (based on DNF item ranges) */
static const EquipmentTemplate g_equipment_table[] = {
    /* === Weapons (slot 11 in DB, slot 0 here) === */
    /* Slayer Swords 50000-50099 */
    {50000, 50019, EQUIP_SLOT_WEAPON, ITEM_GRADE_COMMON,   1, 50, 0, 0, 0, 3, 0, 0, 0, 0, 0, 50, 0},
    {50020, 50039, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNCOMMON, 10, 120, 0, 0, 0, 6, 0, 0, 0, 0, 0, 80, 0},
    {50040, 50059, EQUIP_SLOT_WEAPON, ITEM_GRADE_RARE,     20, 250, 0, 0, 0, 12, 0, 1, 0, 0, 0, 120, 5},
    {50060, 50079, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNIQUE,   30, 450, 0, 0, 0, 20, 0, 2, 0, 50, 0, 180, 10},
    {50080, 50099, EQUIP_SLOT_WEAPON, ITEM_GRADE_LEGENDARY,40, 750, 0, 0, 0, 35, 0, 4, 0, 100, 0, 250, 15},

    /* Mage Staffs 50100-50199 */
    {50100, 50119, EQUIP_SLOT_WEAPON, ITEM_GRADE_COMMON,   1, 0, 55, 0, 0, 0, 4, 0, 0, 0, 20, 60, 0},
    {50120, 50139, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNCOMMON, 10, 0, 135, 0, 0, 0, 8, 0, 1, 0, 40, 100, 0},
    {50140, 50159, EQUIP_SLOT_WEAPON, ITEM_GRADE_RARE,     20, 0, 280, 0, 0, 0, 15, 0, 2, 0, 80, 150, 5},
    {50160, 50179, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNIQUE,   30, 0, 500, 0, 0, 0, 25, 0, 4, 0, 150, 220, 10},
    {50180, 50199, EQUIP_SLOT_WEAPON, ITEM_GRADE_LEGENDARY,40, 0, 850, 0, 0, 0, 42, 0, 7, 0, 280, 320, 15},

    /* Gunner Guns 50200-50299 (hybrid) */
    {50200, 50219, EQUIP_SLOT_WEAPON, ITEM_GRADE_COMMON,   1, 30, 30, 0, 0, 2, 2, 0, 0, 0, 0, 50, 0},
    {50220, 50239, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNCOMMON, 10, 70, 70, 0, 0, 4, 4, 0, 0, 0, 0, 80, 0},
    {50240, 50259, EQUIP_SLOT_WEAPON, ITEM_GRADE_RARE,     20, 150, 150, 0, 0, 8, 8, 0, 0, 0, 0, 120, 5},
    {50260, 50279, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNIQUE,   30, 280, 280, 0, 0, 15, 15, 0, 0, 0, 0, 180, 10},
    {50280, 50299, EQUIP_SLOT_WEAPON, ITEM_GRADE_LEGENDARY,40, 500, 500, 0, 0, 25, 25, 0, 0, 50, 50, 250, 15},

    /* Fighter Gauntlets 50300-50399 */
    {50300, 50319, EQUIP_SLOT_WEAPON, ITEM_GRADE_COMMON,   1, 45, 0, 0, 0, 4, 0, 1, 0, 20, 0, 40, 5},
    {50320, 50339, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNCOMMON, 10, 110, 0, 0, 0, 8, 0, 2, 0, 40, 0, 70, 8},
    {50340, 50359, EQUIP_SLOT_WEAPON, ITEM_GRADE_RARE,     20, 230, 0, 0, 0, 14, 0, 4, 0, 80, 0, 110, 12},
    {50360, 50379, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNIQUE,   30, 420, 0, 0, 0, 22, 0, 6, 0, 150, 0, 160, 18},
    {50380, 50399, EQUIP_SLOT_WEAPON, ITEM_GRADE_LEGENDARY,40, 720, 0, 0, 0, 38, 0, 10, 0, 280, 0, 230, 25},

    /* Priest Crosses/Axes 50400-50499 */
    {50400, 50419, EQUIP_SLOT_WEAPON, ITEM_GRADE_COMMON,   1, 20, 35, 0, 0, 1, 3, 0, 2, 30, 30, 50, 0},
    {50420, 50439, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNCOMMON, 10, 50, 85, 0, 0, 3, 6, 0, 4, 60, 60, 80, 0},
    {50440, 50459, EQUIP_SLOT_WEAPON, ITEM_GRADE_RARE,     20, 110, 180, 0, 0, 6, 12, 0, 7, 120, 120, 120, 5},
    {50460, 50479, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNIQUE,   30, 200, 330, 0, 0, 10, 20, 0, 12, 220, 220, 180, 10},
    {50480, 50499, EQUIP_SLOT_WEAPON, ITEM_GRADE_LEGENDARY,40, 380, 580, 0, 0, 18, 35, 0, 20, 400, 400, 260, 15},

    /* Thief Daggers 50500-50599 */
    {50500, 50519, EQUIP_SLOT_WEAPON, ITEM_GRADE_COMMON,   1, 40, 0, 0, 0, 2, 0, 0, 0, 0, 0, 60, 10},
    {50520, 50539, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNCOMMON, 10, 100, 0, 0, 0, 5, 0, 0, 0, 0, 0, 100, 15},
    {50540, 50559, EQUIP_SLOT_WEAPON, ITEM_GRADE_RARE,     20, 210, 0, 0, 0, 10, 0, 0, 0, 0, 0, 150, 25},
    {50560, 50579, EQUIP_SLOT_WEAPON, ITEM_GRADE_UNIQUE,   30, 390, 0, 0, 0, 17, 0, 0, 0, 0, 0, 220, 35},
    {50580, 50599, EQUIP_SLOT_WEAPON, ITEM_GRADE_LEGENDARY,40, 680, 0, 0, 0, 30, 0, 0, 0, 0, 0, 320, 50},

    /* === Armor Pieces === */
    /* Helmets 50600-50649 */
    {50600, 50609, EQUIP_SLOT_HELMET, ITEM_GRADE_COMMON,   1, 0, 0, 15, 12, 1, 1, 1, 0, 30, 10, 0, 0},
    {50610, 50619, EQUIP_SLOT_HELMET, ITEM_GRADE_UNCOMMON, 10, 0, 0, 35, 28, 2, 2, 2, 0, 60, 20, 0, 0},
    {50620, 50629, EQUIP_SLOT_HELMET, ITEM_GRADE_RARE,     20, 0, 0, 70, 55, 4, 4, 4, 0, 120, 40, 20, 0},
    {50630, 50639, EQUIP_SLOT_HELMET, ITEM_GRADE_UNIQUE,   30, 0, 0, 120, 95, 7, 7, 7, 0, 200, 70, 35, 0},
    {50640, 50649, EQUIP_SLOT_HELMET, ITEM_GRADE_LEGENDARY,40, 0, 0, 200, 160, 12, 12, 12, 0, 350, 120, 55, 0},

    /* Shoulders 50650-50699 */
    {50650, 50659, EQUIP_SLOT_SHOULDER, ITEM_GRADE_COMMON,   1, 0, 0, 12, 10, 2, 0, 1, 0, 20, 0, 0, 0},
    {50660, 50669, EQUIP_SLOT_SHOULDER, ITEM_GRADE_UNCOMMON, 10, 0, 0, 28, 23, 4, 0, 2, 0, 40, 0, 0, 0},
    {50670, 50679, EQUIP_SLOT_SHOULDER, ITEM_GRADE_RARE,     20, 0, 0, 55, 45, 8, 0, 4, 0, 80, 0, 0, 0},
    {50680, 50689, EQUIP_SLOT_SHOULDER, ITEM_GRADE_UNIQUE,   30, 0, 0, 95, 78, 14, 0, 7, 0, 140, 0, 0, 0},
    {50690, 50699, EQUIP_SLOT_SHOULDER, ITEM_GRADE_LEGENDARY,40, 0, 0, 160, 130, 24, 0, 12, 0, 240, 0, 0, 0},

    /* Chest/Body 50700-50749 */
    {50700, 50709, EQUIP_SLOT_CHEST, ITEM_GRADE_COMMON,   1, 0, 0, 25, 20, 0, 0, 3, 0, 80, 0, 0, 0},
    {50710, 50719, EQUIP_SLOT_CHEST, ITEM_GRADE_UNCOMMON, 10, 0, 0, 55, 45, 0, 0, 5, 0, 150, 0, 0, 0},
    {50720, 50729, EQUIP_SLOT_CHEST, ITEM_GRADE_RARE,     20, 0, 0, 110, 90, 0, 0, 9, 0, 280, 0, 0, 0},
    {50730, 50739, EQUIP_SLOT_CHEST, ITEM_GRADE_UNIQUE,   30, 0, 0, 185, 150, 0, 0, 15, 0, 450, 0, 0, 0},
    {50740, 50749, EQUIP_SLOT_CHEST, ITEM_GRADE_LEGENDARY,40, 0, 0, 300, 245, 0, 0, 25, 0, 750, 0, 0, 0},

    /* Belt 50750-50799 */
    {50750, 50759, EQUIP_SLOT_BELT, ITEM_GRADE_COMMON,   1, 0, 0, 8, 6, 1, 1, 1, 1, 25, 15, 0, 0},
    {50760, 50769, EQUIP_SLOT_BELT, ITEM_GRADE_UNCOMMON, 10, 0, 0, 18, 14, 2, 2, 2, 2, 50, 30, 0, 0},
    {50770, 50779, EQUIP_SLOT_BELT, ITEM_GRADE_RARE,     20, 0, 0, 35, 28, 4, 4, 4, 4, 100, 60, 15, 0},
    {50780, 50789, EQUIP_SLOT_BELT, ITEM_GRADE_UNIQUE,   30, 0, 0, 60, 48, 7, 7, 7, 7, 170, 100, 25, 0},
    {50790, 50799, EQUIP_SLOT_BELT, ITEM_GRADE_LEGENDARY,40, 0, 0, 100, 80, 12, 12, 12, 12, 280, 170, 40, 0},

    /* Pants/Legs 50800-50849 */
    {50800, 50809, EQUIP_SLOT_PANTS, ITEM_GRADE_COMMON,   1, 0, 0, 20, 16, 0, 0, 2, 0, 60, 0, 0, 0},
    {50810, 50819, EQUIP_SLOT_PANTS, ITEM_GRADE_UNCOMMON, 10, 0, 0, 45, 36, 0, 0, 4, 0, 110, 0, 0, 0},
    {50820, 50829, EQUIP_SLOT_PANTS, ITEM_GRADE_RARE,     20, 0, 0, 90, 72, 0, 0, 7, 0, 210, 0, 0, 0},
    {50830, 50839, EQUIP_SLOT_PANTS, ITEM_GRADE_UNIQUE,   30, 0, 0, 150, 120, 0, 0, 12, 0, 350, 0, 0, 0},
    {50840, 50849, EQUIP_SLOT_PANTS, ITEM_GRADE_LEGENDARY,40, 0, 0, 250, 200, 0, 0, 20, 0, 580, 0, 0, 0},

    /* Shoes 50850-50899 */
    {50850, 50859, EQUIP_SLOT_SHOES, ITEM_GRADE_COMMON,   1, 0, 0, 10, 8, 0, 0, 1, 0, 20, 0, 0, 3},
    {50860, 50869, EQUIP_SLOT_SHOES, ITEM_GRADE_UNCOMMON, 10, 0, 0, 24, 19, 0, 0, 2, 0, 40, 0, 0, 5},
    {50870, 50879, EQUIP_SLOT_SHOES, ITEM_GRADE_RARE,     20, 0, 0, 48, 38, 0, 0, 4, 0, 80, 0, 0, 8},
    {50880, 50889, EQUIP_SLOT_SHOES, ITEM_GRADE_UNIQUE,   30, 0, 0, 80, 64, 0, 0, 7, 0, 140, 0, 0, 12},
    {50890, 50899, EQUIP_SLOT_SHOES, ITEM_GRADE_LEGENDARY,40, 0, 0, 135, 108, 0, 0, 12, 0, 230, 0, 0, 18},

    /* === Accessories === */
    /* Bracelet 50900-50949 */
    {50900, 50909, EQUIP_SLOT_BRACELET, ITEM_GRADE_COMMON,   1, 5, 5, 0, 0, 2, 2, 0, 0, 0, 0, 20, 0},
    {50910, 50919, EQUIP_SLOT_BRACELET, ITEM_GRADE_UNCOMMON, 10, 12, 12, 0, 0, 4, 4, 0, 0, 0, 0, 35, 0},
    {50920, 50929, EQUIP_SLOT_BRACELET, ITEM_GRADE_RARE,     20, 25, 25, 0, 0, 7, 7, 0, 0, 0, 0, 55, 0},
    {50930, 50939, EQUIP_SLOT_BRACELET, ITEM_GRADE_UNIQUE,   30, 45, 45, 0, 0, 12, 12, 0, 0, 0, 0, 80, 0},
    {50940, 50949, EQUIP_SLOT_BRACELET, ITEM_GRADE_LEGENDARY,40, 75, 75, 0, 0, 20, 20, 0, 0, 0, 0, 120, 0},

    /* Necklace 50950-50999 */
    {50950, 50959, EQUIP_SLOT_NECKLACE, ITEM_GRADE_COMMON,   1, 3, 8, 0, 0, 0, 3, 0, 1, 0, 20, 15, 0},
    {50960, 50969, EQUIP_SLOT_NECKLACE, ITEM_GRADE_UNCOMMON, 10, 8, 18, 0, 0, 0, 5, 0, 2, 0, 40, 28, 0},
    {50970, 50979, EQUIP_SLOT_NECKLACE, ITEM_GRADE_RARE,     20, 18, 38, 0, 0, 0, 9, 0, 4, 0, 80, 45, 0},
    {50980, 50989, EQUIP_SLOT_NECKLACE, ITEM_GRADE_UNIQUE,   30, 32, 68, 0, 0, 0, 15, 0, 7, 0, 140, 68, 0},
    {50990, 50999, EQUIP_SLOT_NECKLACE, ITEM_GRADE_LEGENDARY,40, 55, 115, 0, 0, 0, 25, 0, 12, 0, 240, 100, 0},

    /* Ring 51000-51049 */
    {51000, 51009, EQUIP_SLOT_RING, ITEM_GRADE_COMMON,   1, 8, 0, 0, 0, 3, 0, 0, 0, 0, 0, 30, 0},
    {51010, 51019, EQUIP_SLOT_RING, ITEM_GRADE_UNCOMMON, 10, 18, 0, 0, 0, 5, 0, 0, 0, 0, 0, 50, 0},
    {51020, 51029, EQUIP_SLOT_RING, ITEM_GRADE_RARE,     20, 38, 0, 0, 0, 9, 0, 0, 0, 0, 0, 80, 0},
    {51030, 51039, EQUIP_SLOT_RING, ITEM_GRADE_UNIQUE,   30, 68, 0, 0, 0, 15, 0, 0, 0, 0, 0, 120, 0},
    {51040, 51049, EQUIP_SLOT_RING, ITEM_GRADE_LEGENDARY,40, 115, 0, 0, 0, 25, 0, 0, 0, 0, 0, 180, 0},

    /* End marker */
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

/* Find equipment template by item_id */
static const EquipmentTemplate* find_equipment_template(uint32_t item_id) {
    for (int i = 0; g_equipment_table[i].item_id_max > 0; i++) {
        if (item_id >= g_equipment_table[i].item_id_min &&
            item_id <= g_equipment_table[i].item_id_max) {
            return &g_equipment_table[i];
        }
    }
    return NULL;
}

/* Calculate enhance bonus multiplier */
static float get_enhance_multiplier(uint8_t enhance_level) {
    /* Enhancement bonuses: +0=1.0, +1=1.03, +2=1.06, ... +10=1.30, +11=1.35 ... */
    if (enhance_level == 0) return 1.0f;
    if (enhance_level <= 10) return 1.0f + enhance_level * 0.03f;
    return 1.30f + (enhance_level - 10) * 0.05f;  /* Bonus after +10 */
}

/* Apply equipment bonus to combat stats */
static void apply_equipment_stats(CombatStats* stats, const EquipmentTemplate* tmpl, uint8_t enhance) {
    float mult = get_enhance_multiplier(enhance);

    /* Apply base stats */
    stats->str += tmpl->str_bonus;
    stats->intel += tmpl->int_bonus;
    stats->vit += tmpl->vit_bonus;
    stats->spr += tmpl->spr_bonus;

    /* Apply combat stats with enhance multiplier */
    stats->phys_attack += (uint32_t)(tmpl->phys_attack * mult);
    stats->mag_attack += (uint32_t)(tmpl->mag_attack * mult);
    stats->phys_def += (uint32_t)(tmpl->phys_def * mult);
    stats->mag_def += (uint32_t)(tmpl->mag_def * mult);

    /* Apply HP/MP bonuses */
    stats->max_hp += tmpl->hp_bonus;
    stats->max_mp += tmpl->mp_bonus;

    /* Apply other bonuses */
    stats->crit_rate += tmpl->crit_bonus;
    stats->attack_speed += tmpl->speed_bonus;
    stats->move_speed += tmpl->speed_bonus / 2;
}

/* ========================================
 * Skill Damage System
 * ======================================== */

/* Skill type determines which attack stat is used */
typedef enum {
    SKILL_TYPE_PHYSICAL = 0,    /* Uses phys_attack */
    SKILL_TYPE_MAGICAL,         /* Uses mag_attack */
    SKILL_TYPE_HYBRID,          /* Uses max(phys, mag) */
    SKILL_TYPE_FIXED            /* Fixed damage, ignores stats */
} SkillDamageType;

/* Element types for elemental damage system */
#ifndef ELEMENT_NONE
typedef enum {
    ELEMENT_NONE = 0,
    ELEMENT_FIRE,
    ELEMENT_ICE,
    ELEMENT_LIGHT,
    ELEMENT_DARK
} ElementType;
#else
typedef uint8_t ElementType;
#endif

/* Monster resistance profile */
typedef struct {
    uint16_t monster_type_base;  /* Base monster type (1000, 1100, etc) */
    uint8_t  phys_resist;        /* Physical resistance % (0-100) */
    uint8_t  mag_resist;         /* Magical resistance % (0-100) */
    uint8_t  fire_resist;        /* Fire resistance % */
    uint8_t  ice_resist;         /* Ice resistance % */
    uint8_t  light_resist;       /* Light resistance % */
    uint8_t  dark_resist;        /* Dark resistance % */
} MonsterResist;

/* Monster resistance table */
static const MonsterResist g_monster_resist_table[] = {
    /* type_base, phys, mag, fire, ice, light, dark */
    {1000, 20, 10, 0, 0, 0, 0},     /* Basic monsters */
    {1100, 30, 15, 50, 0, 0, 0},    /* Fire-type monsters (resist fire) */
    {1200, 25, 20, 0, 50, 0, 0},    /* Ice-type monsters (resist ice) */
    {1300, 15, 35, 0, 0, 50, 0},    /* Light-type monsters (resist light) */
    {1400, 20, 30, 0, 0, 0, 50},    /* Dark-type monsters (resist dark) */
    {1500, 40, 40, 25, 25, 25, 25}, /* Elite monsters (balanced resist) */
    {1600, 50, 20, 0, 30, 0, 0},    /* Armored (high phys resist) */
    {1700, 10, 50, 30, 0, 0, 0},    /* Magical (high mag resist) */
    {2000, 35, 35, 20, 20, 20, 20}, /* Boss base (moderate resist) */
    {0, 0, 0, 0, 0, 0, 0}          /* End marker */
};

/* Find monster resistance by type */
static const MonsterResist* find_monster_resist(uint16_t monster_type) {
    uint16_t base_type = (monster_type / 100) * 100;  /* Round to nearest 100 */
    for (int i = 0; g_monster_resist_table[i].monster_type_base != 0; i++) {
        if (g_monster_resist_table[i].monster_type_base == base_type) {
            return &g_monster_resist_table[i];
        }
    }
    return &g_monster_resist_table[0];  /* Default to basic */
}

/* Skill info for damage calculation */
typedef struct {
    uint16_t skill_id;
    uint8_t  job;               /* Required job (0xFF = any) */
    uint8_t  type;              /* SkillDamageType */
    uint8_t  element;           /* ElementType for elemental damage */
    uint16_t base_damage;       /* Base damage at level 1 */
    uint16_t damage_per_level;  /* Additional damage per skill level */
    uint16_t attack_percent;    /* % of attack stat (100 = 100%) */
    uint16_t hit_count;         /* Number of hits */
    uint16_t cooldown_ms;       /* Cooldown in milliseconds */
    uint16_t mp_cost;           /* MP cost */
} SkillDamageInfo;

/* Skill damage table - based on common DNF skill IDs */
/* Format: {skill_id, job, type, element, base_dmg, dmg/lvl, atk%, hits, cd_ms, mp} */
static const SkillDamageInfo g_skill_damage_table[] = {
    /* Slayer skills (job=0) - Physical, Dark element theme */
    {260, 0, SKILL_TYPE_PHYSICAL, ELEMENT_NONE, 150, 30, 120, 1, 1000, 10},     /* Basic attack */
    {261, 0, SKILL_TYPE_PHYSICAL, ELEMENT_NONE, 300, 50, 150, 3, 2000, 25},     /* Triple slash */
    {262, 0, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 500, 80, 200, 1, 3000, 40},     /* Dark strike */
    {263, 0, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 800, 120, 250, 5, 5000, 60},    /* Blade storm */
    {264, 0, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 1200, 180, 300, 1, 8000, 100},  /* Ultimate slash */

    /* Fighter skills (job=1) - Physical, Fire element theme */
    {300, 1, SKILL_TYPE_PHYSICAL, ELEMENT_NONE, 180, 35, 130, 2, 1200, 12},     /* Double punch */
    {301, 1, SKILL_TYPE_PHYSICAL, ELEMENT_FIRE, 350, 55, 160, 4, 2200, 30},     /* Burning strikes */
    {302, 1, SKILL_TYPE_PHYSICAL, ELEMENT_FIRE, 550, 85, 210, 1, 3200, 45},     /* Rising uppercut */
    {303, 1, SKILL_TYPE_PHYSICAL, ELEMENT_FIRE, 900, 130, 280, 8, 5500, 70},    /* Hundred fists */
    {304, 1, SKILL_TYPE_PHYSICAL, ELEMENT_FIRE, 1400, 200, 350, 1, 9000, 120},  /* Dragon kick */

    /* Gunner skills (job=2) - hybrid damage, Light element */
    {400, 2, SKILL_TYPE_HYBRID, ELEMENT_NONE, 160, 32, 125, 1, 800, 8},         /* Quick shot */
    {401, 2, SKILL_TYPE_HYBRID, ELEMENT_LIGHT, 280, 48, 145, 5, 1800, 22},      /* Rapid fire */
    {402, 2, SKILL_TYPE_PHYSICAL, ELEMENT_FIRE, 450, 75, 190, 1, 2800, 38},     /* Grenade */
    {403, 2, SKILL_TYPE_MAGICAL, ELEMENT_LIGHT, 700, 110, 240, 1, 4500, 55},    /* Laser cannon */
    {404, 2, SKILL_TYPE_HYBRID, ELEMENT_LIGHT, 1100, 160, 290, 10, 7000, 90},   /* Satellite beam */

    /* Mage skills (job=3) - Fire/Ice elements */
    {500, 3, SKILL_TYPE_MAGICAL, ELEMENT_NONE, 200, 40, 140, 1, 1500, 15},      /* Magic missile */
    {501, 3, SKILL_TYPE_MAGICAL, ELEMENT_FIRE, 400, 65, 180, 1, 2500, 35},      /* Fireball */
    {502, 3, SKILL_TYPE_MAGICAL, ELEMENT_ICE, 650, 100, 230, 3, 4000, 55},      /* Ice storm */
    {503, 3, SKILL_TYPE_MAGICAL, ELEMENT_FIRE, 1000, 150, 300, 1, 6000, 80},    /* Meteor */
    {504, 3, SKILL_TYPE_MAGICAL, ELEMENT_DARK, 1600, 220, 400, 1, 10000, 150},  /* Apocalypse */

    /* Priest skills (job=4) - Light element */
    {600, 4, SKILL_TYPE_MAGICAL, ELEMENT_LIGHT, 120, 25, 100, 1, 1000, 10},     /* Holy strike */
    {601, 4, SKILL_TYPE_MAGICAL, ELEMENT_LIGHT, 250, 45, 140, 1, 2000, 25},     /* Divine smite */
    {602, 4, SKILL_TYPE_MAGICAL, ELEMENT_LIGHT, 450, 70, 180, 1, 3500, 45},     /* Holy explosion */
    {603, 4, SKILL_TYPE_MAGICAL, ELEMENT_LIGHT, 750, 115, 250, 1, 5500, 70},    /* Judgment */
    {604, 4, SKILL_TYPE_MAGICAL, ELEMENT_LIGHT, 1300, 190, 350, 1, 8500, 110},  /* Divine wrath */

    /* Thief skills (job=5) - Dark element */
    {700, 5, SKILL_TYPE_PHYSICAL, ELEMENT_NONE, 170, 33, 135, 2, 900, 9},       /* Backstab */
    {701, 5, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 320, 52, 165, 3, 1900, 28},     /* Shadow strike */
    {702, 5, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 500, 82, 215, 5, 3000, 42},     /* Blade dance */
    {703, 5, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 850, 125, 275, 1, 5200, 65},    /* Assassinate */
    {704, 5, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 1350, 195, 340, 1, 8000, 105},  /* Shadow execution */

    /* Common skills (any job) */
    {100, 0xFF, SKILL_TYPE_PHYSICAL, ELEMENT_NONE, 100, 20, 100, 1, 500, 0},    /* Normal attack */
    {101, 0xFF, SKILL_TYPE_PHYSICAL, ELEMENT_NONE, 50, 10, 50, 1, 0, 0},        /* Basic hit */

    /* Awakening/Ultimate skills - high damage, long cooldown */
    {1000, 0, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 3000, 400, 500, 10, 60000, 200},  /* Slayer awakening */
    {1001, 1, SKILL_TYPE_PHYSICAL, ELEMENT_FIRE, 3200, 420, 520, 15, 60000, 220},  /* Fighter awakening */
    {1002, 2, SKILL_TYPE_HYBRID, ELEMENT_LIGHT, 2800, 380, 480, 20, 60000, 180},   /* Gunner awakening */
    {1003, 3, SKILL_TYPE_MAGICAL, ELEMENT_FIRE, 3500, 450, 550, 1, 60000, 250},    /* Mage awakening */
    {1004, 4, SKILL_TYPE_MAGICAL, ELEMENT_LIGHT, 2500, 350, 450, 1, 60000, 150},   /* Priest awakening */
    {1005, 5, SKILL_TYPE_PHYSICAL, ELEMENT_DARK, 3100, 410, 510, 1, 60000, 210},   /* Thief awakening */

    /* End marker */
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

/* Find skill info by skill_id */
static const SkillDamageInfo* find_skill_info(uint16_t skill_id) {
    for (int i = 0; g_skill_damage_table[i].skill_id != 0; i++) {
        if (g_skill_damage_table[i].skill_id == skill_id) {
            return &g_skill_damage_table[i];
        }
    }
    return NULL;
}

/* Calculate level difference modifier
 * DNF-style: higher level = bonus damage, lower level = reduced damage
 */
static uint32_t calculate_level_modifier(uint8_t char_level, uint8_t monster_level) {
    int level_diff = (int)char_level - (int)monster_level;

    if (level_diff >= 10) return 130;      /* +30% damage for 10+ levels higher */
    if (level_diff >= 5)  return 115;      /* +15% damage for 5-9 levels higher */
    if (level_diff >= 0)  return 100;      /* Normal damage when same or higher */
    if (level_diff >= -5) return 90;       /* -10% for 1-5 levels lower */
    if (level_diff >= -10) return 75;      /* -25% for 6-10 levels lower */
    return 50;                              /* -50% for 10+ levels lower */
}

/* Get element resistance from monster profile */
static uint8_t get_element_resist(const MonsterResist* resist, uint8_t element) {
    if (!resist) return 0;
    switch (element) {
        case ELEMENT_FIRE:  return resist->fire_resist;
        case ELEMENT_ICE:   return resist->ice_resist;
        case ELEMENT_LIGHT: return resist->light_resist;
        case ELEMENT_DARK:  return resist->dark_resist;
        default:            return 0;
    }
}

/* Calculate skill damage (enhanced version with monster type and level)
 * @param stats        - Character combat stats
 * @param skill_id     - Skill being used
 * @param skill_level  - Level of the skill (1-30)
 * @param job          - Character job
 * @param char_level   - Character level (for level diff calculation)
 * @param monster_type - Monster type (for resistance lookup)
 * @param monster_level - Monster effective level (based on dungeon difficulty)
 * @param monster_def  - Monster's base defense
 * @param is_crit      - Output: whether this was a critical hit
 * @return             - Final damage value
 */
static uint32_t calculate_skill_damage_v2(CombatStats* stats, uint16_t skill_id,
                                          uint8_t skill_level, uint8_t job,
                                          uint8_t char_level, uint16_t monster_type,
                                          uint8_t monster_level, uint32_t monster_def,
                                          uint8_t* is_crit) {
    if (!stats) return 0;
    if (skill_level < 1) skill_level = 1;
    if (skill_level > 30) skill_level = 30;

    /* Find skill info */
    const SkillDamageInfo* skill = find_skill_info(skill_id);
    if (!skill) {
        /* Unknown skill - use basic attack formula */
        uint32_t base = stats->phys_attack / 2;
        if (is_crit) *is_crit = 0;
        return base > monster_def ? base - monster_def : 1;
    }

    /* Check job restriction */
    if (skill->job != 0xFF && skill->job != job) {
        /* Wrong job for this skill - reduced damage */
        uint32_t base = stats->phys_attack / 4;
        if (is_crit) *is_crit = 0;
        return base > monster_def ? base - monster_def : 1;
    }

    /* Get monster resistance profile */
    const MonsterResist* resist = find_monster_resist(monster_type);

    /* Get attack power based on skill type */
    uint32_t attack_power;
    uint32_t defense;
    uint8_t resist_percent = 0;

    switch (skill->type) {
        case SKILL_TYPE_MAGICAL:
            attack_power = stats->mag_attack;
            defense = monster_def;
            resist_percent = resist ? resist->mag_resist : 0;
            break;
        case SKILL_TYPE_HYBRID:
            attack_power = (stats->phys_attack > stats->mag_attack) ?
                           stats->phys_attack : stats->mag_attack;
            defense = monster_def / 2;  /* Hybrid bypasses half defense */
            resist_percent = resist ? (resist->phys_resist + resist->mag_resist) / 4 : 0;
            break;
        case SKILL_TYPE_FIXED:
            attack_power = 0;  /* Fixed damage ignores attack */
            defense = 0;       /* And defense */
            resist_percent = 0;
            break;
        case SKILL_TYPE_PHYSICAL:
        default:
            attack_power = stats->phys_attack;
            defense = monster_def;
            resist_percent = resist ? resist->phys_resist : 0;
            break;
    }

    /* Calculate base skill damage */
    uint32_t skill_base = skill->base_damage + (skill->damage_per_level * (skill_level - 1));

    /* Apply attack power percentage */
    uint32_t attack_bonus = (attack_power * skill->attack_percent) / 100;

    /* Total damage before defense */
    uint32_t total_damage = skill_base + attack_bonus;

    /* Apply level modifier */
    uint32_t level_mod = calculate_level_modifier(char_level, monster_level);
    total_damage = (total_damage * level_mod) / 100;

    /* Apply defense reduction */
    uint32_t damage_after_def;
    if (total_damage > defense) {
        damage_after_def = total_damage - defense;
    } else {
        damage_after_def = total_damage / 10;  /* Minimum 10% damage */
        if (damage_after_def < 1) damage_after_def = 1;
    }

    /* Apply type resistance (physical/magical) */
    if (resist_percent > 0) {
        damage_after_def = (damage_after_def * (100 - resist_percent)) / 100;
    }

    /* Apply elemental resistance/weakness */
    if (skill->element != ELEMENT_NONE && resist) {
        uint8_t elem_resist = get_element_resist(resist, skill->element);
        if (elem_resist > 0) {
            /* Monster resists this element */
            damage_after_def = (damage_after_def * (100 - elem_resist)) / 100;
        }
        /* Note: Could add elemental weakness here (bonus damage) */
    }

    /* Critical hit check */
    uint8_t crit = 0;
    uint32_t rand_val = (uint32_t)rand() % 10000;  /* 0-9999 */
    if (rand_val < stats->crit_rate) {
        crit = 1;
        damage_after_def = (damage_after_def * 150) / 100;  /* 150% crit damage */
    }
    if (is_crit) *is_crit = crit;

    /* Add some randomness (90%-110%) */
    uint32_t variance = (uint32_t)rand() % 21;  /* 0-20 */
    damage_after_def = (damage_after_def * (90 + variance)) / 100;

    /* Minimum damage is 1 */
    if (damage_after_def < 1) damage_after_def = 1;

    return damage_after_def;
}

/* Legacy wrapper for existing code - simple version */
static uint32_t calculate_skill_damage(CombatStats* stats, uint16_t skill_id,
                                       uint8_t skill_level, uint8_t job,
                                       uint32_t monster_def, uint8_t* is_crit) {
    /* Use v2 with default monster type and same level */
    return calculate_skill_damage_v2(stats, skill_id, skill_level, job,
                                      50, 1000, 50, monster_def, is_crit);
}

/* Get skill MP cost */
uint16_t get_skill_mp_cost(uint16_t skill_id, uint8_t skill_level) {
    const SkillDamageInfo* skill = find_skill_info(skill_id);
    if (!skill) return 0;

    /* MP cost scales with skill level */
    return skill->mp_cost + (skill->mp_cost * (skill_level - 1) / 10);
}

/* Get skill cooldown */
uint16_t get_skill_cooldown(uint16_t skill_id) {
    const SkillDamageInfo* skill = find_skill_info(skill_id);
    if (!skill) return 1000;  /* Default 1 second */
    return skill->cooldown_ms;
}

/* Get skill hit count */
uint8_t get_skill_hit_count(uint16_t skill_id) {
    const SkillDamageInfo* skill = find_skill_info(skill_id);
    if (!skill) return 1;
    return (uint8_t)skill->hit_count;
}


/*------------------------------------------------------------------------------
 * Phase 6 Enhanced: Damage Calculation v3 (完整伤害公式)
 *----------------------------------------------------------------------------*/

/* Damage modifier structure - collects all modifiers before final calculation */
typedef struct {
    int32_t attack_percent;      /* Attack power bonus % (from buffs, equipment) */
    int32_t damage_percent;      /* Final damage bonus % */
    int32_t crit_damage_bonus;   /* Critical damage multiplier bonus (150 base + this) */
    int32_t def_penetration;     /* Fixed defense penetration */
    int32_t def_penetration_pct; /* % defense penetration */
    int32_t elem_bonus[6];       /* Elemental damage bonus % */
    uint8_t guaranteed_crit;     /* Force critical hit */
    uint8_t ignore_defense;      /* Ignore all defense */
} DamageModifiers;

/* Get current character level from session */
static uint8_t get_character_level(ClientSession* s) {
    if (!s) return 1;
    for (int i = 0; i < s->character_count && i < MAX_CHARACTERS; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            return s->characters[i].level;
        }
    }
    return 1;
}

/* Collect all damage modifiers from session (buffs, equipment, etc) */
static void collect_damage_modifiers(ClientSession* s, DamageModifiers* mods) {
    memset(mods, 0, sizeof(DamageModifiers));
    if (!s) return;

    /* Buff modifiers */
    mods->attack_percent += buff_get_stat_modifier(s, BUFF_ATTACK_UP);
    mods->attack_percent += buff_get_stat_modifier(s, BUFF_MAGIC_UP);
    mods->attack_percent += buff_get_stat_modifier(s, BUFF_ALL_STATS_UP);

    /* Debuff effects on player (reduce damage) */
    int32_t curse = buff_get_stat_modifier(s, DEBUFF_CURSE);
    if (curse > 0) {
        mods->damage_percent -= curse;
    }
}

/* Enhanced damage calculation with all modifiers */
uint32_t calculate_skill_damage_v3(ClientSession* s, uint16_t skill_id,
                                   uint8_t skill_level, uint16_t monster_type,
                                   uint8_t monster_level, uint32_t monster_def,
                                   uint8_t* is_crit, uint8_t* is_miss) {
    if (!s) return 0;

    CombatStats* stats = &s->combat_stats;
    uint8_t char_level = get_character_level(s);

    if (skill_level < 1) skill_level = 1;
    if (skill_level > 30) skill_level = 30;
    if (is_crit) *is_crit = 0;
    if (is_miss) *is_miss = 0;

    /* Collect all modifiers */
    DamageModifiers mods;
    collect_damage_modifiers(s, &mods);

    /* Find skill info */
    const SkillDamageInfo* skill = find_skill_info(skill_id);
    if (!skill) {
        uint32_t base = stats->phys_attack / 2;
        return base > monster_def ? base - monster_def : 1;
    }

    /* Miss check (hit rate vs evasion) */
    uint32_t hit_chance = stats->hit_rate;
    if (monster_level > char_level) {
        int level_diff = monster_level - char_level;
        hit_chance = hit_chance * (100 - level_diff * 2) / 100;
    }
    uint32_t miss_roll = (uint32_t)rand() % 10000;
    if (miss_roll >= hit_chance) {
        if (is_miss) *is_miss = 1;
        return 0;
    }

    /* Get monster resistance */
    const MonsterResist* resist = find_monster_resist(monster_type);

    /* Get base attack power */
    uint32_t attack_power;
    uint32_t defense = monster_def;
    uint8_t resist_percent = 0;

    switch (skill->type) {
        case SKILL_TYPE_MAGICAL:
            attack_power = stats->mag_attack;
            resist_percent = resist ? resist->mag_resist : 0;
            break;
        case SKILL_TYPE_HYBRID:
            attack_power = (stats->phys_attack > stats->mag_attack) ?
                           stats->phys_attack : stats->mag_attack;
            defense = monster_def / 2;
            resist_percent = resist ? (resist->phys_resist + resist->mag_resist) / 4 : 0;
            break;
        case SKILL_TYPE_FIXED:
            attack_power = 0;
            defense = 0;
            resist_percent = 0;
            break;
        case SKILL_TYPE_PHYSICAL:
        default:
            attack_power = stats->phys_attack;
            resist_percent = resist ? resist->phys_resist : 0;
            break;
    }

    /* Apply attack power modifier from buffs */
    if (mods.attack_percent != 0) {
        attack_power = attack_power * (100 + mods.attack_percent) / 100;
    }

    /* Calculate base skill damage */
    uint32_t skill_base = skill->base_damage + (skill->damage_per_level * (skill_level - 1));
    uint32_t attack_bonus = (attack_power * skill->attack_percent) / 100;
    uint32_t total_damage = skill_base + attack_bonus;

    /* Apply level modifier */
    uint32_t level_mod = calculate_level_modifier(char_level, monster_level);
    total_damage = (total_damage * level_mod) / 100;

    /* Apply defense penetration */
    if (mods.ignore_defense) {
        defense = 0;
    } else {
        if (mods.def_penetration > 0 && defense > (uint32_t)mods.def_penetration) {
            defense -= mods.def_penetration;
        }
        if (mods.def_penetration_pct > 0) {
            defense = defense * (100 - mods.def_penetration_pct) / 100;
        }
    }

    /* Apply defense reduction */
    uint32_t damage_after_def;
    if (total_damage > defense) {
        damage_after_def = total_damage - defense;
    } else {
        damage_after_def = total_damage / 10;
        if (damage_after_def < 1) damage_after_def = 1;
    }

    /* Apply type resistance */
    if (resist_percent > 0) {
        damage_after_def = damage_after_def * (100 - resist_percent) / 100;
    }

    /* Apply elemental bonus/resistance */
    if (skill->element != ELEMENT_NONE && skill->element < 6) {
        int32_t elem_bonus = mods.elem_bonus[skill->element];
        if (elem_bonus > 0) {
            damage_after_def = damage_after_def * (100 + elem_bonus) / 100;
        }
        if (resist) {
            uint8_t elem_resist = get_element_resist(resist, skill->element);
            if (elem_resist > 0) {
                damage_after_def = damage_after_def * (100 - elem_resist) / 100;
            }
        }
    }

    /* Critical hit check */
    uint8_t crit = 0;
    uint32_t crit_rate = stats->crit_rate;
    if (mods.guaranteed_crit) {
        crit = 1;
    } else {
        uint32_t rand_val = (uint32_t)rand() % 10000;
        if (rand_val < crit_rate) {
            crit = 1;
        }
    }

    if (crit) {
        uint32_t crit_multiplier = 150 + mods.crit_damage_bonus;
        damage_after_def = damage_after_def * crit_multiplier / 100;
    }
    if (is_crit) *is_crit = crit;

    /* Apply final damage modifier */
    if (mods.damage_percent != 0) {
        int32_t modified = (int32_t)damage_after_def * (100 + mods.damage_percent) / 100;
        damage_after_def = (modified > 0) ? (uint32_t)modified : 1;
    }

    /* Add variance (95%-105%) */
    uint32_t variance = (uint32_t)rand() % 11;
    damage_after_def = damage_after_def * (95 + variance) / 100;

    if (damage_after_def < 1) damage_after_def = 1;

    return damage_after_def;
}


static uint32_t calculate_final_damage(ClientSession* s, uint32_t base_damage,
                                        uint8_t is_physical) {
    if (!s) return base_damage;

    uint32_t damage = base_damage;

    /* Apply attack buffs */
    if (is_physical) {
        int32_t attack_mod = buff_get_stat_modifier(s, BUFF_ATTACK_UP);
        if (attack_mod != 0) {
            damage = (damage * (100 + attack_mod)) / 100;
        }
    } else {
        int32_t magic_mod = buff_get_stat_modifier(s, BUFF_MAGIC_UP);
        if (magic_mod != 0) {
            damage = (damage * (100 + magic_mod)) / 100;
        }
    }

    /* Apply all stats buff */
    int32_t all_mod = buff_get_stat_modifier(s, BUFF_ALL_STATS_UP);
    if (all_mod != 0) {
        damage = (damage * (100 + all_mod)) / 100;
    }

    /* Apply curse debuff (reduces all damage dealt) */
    int32_t curse = buff_get_stat_modifier(s, DEBUFF_CURSE);
    if (curse != 0) {
        damage = (damage * (100 + curse)) / 100;  /* curse is negative */
    }

    return damage;
}

/* Check if player can be damaged (invincibility check) */
static int can_be_damaged(ClientSession* s) {
    if (!s) return 1;
    return !buff_has(s, BUFF_INVINCIBLE);
}

/* Check if player can use skills (silence check) */
static int can_use_skills(ClientSession* s) {
    if (!s) return 1;
    return !buff_has(s, DEBUFF_SILENCE);
}

/* Check if player can move (freeze/stun check) */
static int can_move(ClientSession* s) {
    if (!s) return 1;
    return !buff_has(s, DEBUFF_FREEZE) && !buff_has(s, DEBUFF_STUN);
}

/* Check if player can act (stun check) */
static int can_act(ClientSession* s) {
    if (!s) return 1;
    return !buff_has(s, DEBUFF_STUN);
}

/* Process lifesteal on damage dealt */
static void process_lifesteal(ClientSession* s, uint32_t damage_dealt) {
    if (!s) return;

    int32_t lifesteal = buff_get_stat_modifier(s, BUFF_LIFESTEAL);
    if (lifesteal > 0) {
        uint32_t heal = (damage_dealt * lifesteal) / 100;
        s->current_hp += heal;
        if (s->current_hp > s->combat_stats.max_hp) {
            s->current_hp = s->combat_stats.max_hp;
        }
        /* printf("[LIFESTEAL] Healed %u HP\n", heal); */
    }
}

/* Process shield absorption */
uint32_t process_shield(ClientSession* s, uint32_t incoming_damage) {
    if (!s) return incoming_damage;

    for (int i = 0; i < s->buff_count; i++) {
        if (s->active_buffs[i].buff_id == BUFF_SHIELD) {
            int32_t shield_hp = s->active_buffs[i].value;
            if (shield_hp >= (int32_t)incoming_damage) {
                /* Shield absorbs all damage */
                s->active_buffs[i].value -= incoming_damage;
                printf("[SHIELD] Absorbed %u damage (remaining: %d)\n",
                       incoming_damage, s->active_buffs[i].value);
                return 0;
            } else {
                /* Shield breaks */
                uint32_t remaining = incoming_damage - shield_hp;
                buff_remove(s, BUFF_SHIELD);
                printf("[SHIELD] Broke! Remaining damage: %u\n", remaining);
                return remaining;
            }
        }
    }
    return incoming_damage;
}

/*------------------------------------------------------------------------------
 * Skill Cooldown Helpers
 *----------------------------------------------------------------------------*/

static uint32_t get_cooldown_ready_time(ClientSession* s, uint16_t skill_id) {
    if (!s) return 0;
    for (int i = 0; i < s->cooldown_count; i++) {
        if (s->skill_cooldowns[i].skill_id == skill_id) {
            return s->skill_cooldowns[i].ready_time;
        }
    }
    return 0;
}

int skill_is_on_cooldown(ClientSession* s, uint16_t skill_id) {
    if (!s) return 0;

    uint32_t ready_time = get_cooldown_ready_time(s, skill_id);
    if (ready_time == 0) return 0;

    uint32_t now = get_current_time_ms();
    return (ready_time > now) ? 1 : 0;
}

uint32_t skill_get_cooldown_remaining(ClientSession* s, uint16_t skill_id) {
    if (!s) return 0;

    uint32_t ready_time = get_cooldown_ready_time(s, skill_id);
    if (ready_time == 0) return 0;

    uint32_t now = get_current_time_ms();
    return (ready_time > now) ? (ready_time - now) : 0;
}

void skill_start_cooldown(ClientSession* s, uint16_t skill_id) {
    if (!s) return;

    const SkillDamageInfo* skill = find_skill_info(skill_id);
    uint32_t cooldown_ms = skill ? skill->cooldown_ms : 1000;

    int32_t cdr = buff_get_stat_modifier(s, BUFF_ATTACK_SPEED_UP);
    if (cdr > 0) {
        cooldown_ms = (cooldown_ms * (100 - cdr)) / 100;
        if (cooldown_ms < 100) cooldown_ms = 100;
    }

    uint32_t ready_time = get_current_time_ms() + cooldown_ms;

    for (int i = 0; i < s->cooldown_count; i++) {
        if (s->skill_cooldowns[i].skill_id == skill_id) {
            s->skill_cooldowns[i].ready_time = ready_time;
            return;
        }
    }

    if (s->cooldown_count < MAX_SKILL_COOLDOWNS) {
        s->skill_cooldowns[s->cooldown_count].skill_id = skill_id;
        s->skill_cooldowns[s->cooldown_count].ready_time = ready_time;
        s->cooldown_count++;
    }
}

void skill_reset_cooldown(ClientSession* s, uint16_t skill_id) {
    if (!s) return;
    for (int i = 0; i < s->cooldown_count; i++) {
        if (s->skill_cooldowns[i].skill_id == skill_id) {
            s->skill_cooldowns[i].ready_time = 0;
            return;
        }
    }
}

void skill_reset_all_cooldowns(ClientSession* s) {
    if (!s) return;
    s->cooldown_count = 0;
}

