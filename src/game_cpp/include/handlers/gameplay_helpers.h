#ifndef HANDLERS_GAMEPLAY_HELPERS_H
#define HANDLERS_GAMEPLAY_HELPERS_H

#include <stdint.h>
#include "game_server_types.h"
#include "db/db_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAX_RANDOM_ATTRS
#define MAX_RANDOM_ATTRS 4
#endif

#ifndef AVATAR_SLOT_MAX
#define AVATAR_SLOT_MAX 12
#endif

#ifndef TITLE_MAX_COUNT
#define TITLE_MAX_COUNT 100
#endif

#ifndef TITLE_SLOT_COUNT
#define TITLE_SLOT_COUNT 3
#endif

#ifndef ACHIEVEMENT_MAX
#define ACHIEVEMENT_MAX 200
#endif

#ifndef GEM_MAX_SOCKETS
#define GEM_MAX_SOCKETS 4
#endif

#ifndef GEM_TYPE_MAX
#define GEM_TYPE_MAX 10
#endif

#ifndef ABYSS_DIFFICULTY_MAX
#define ABYSS_DIFFICULTY_MAX 5
#endif

#ifndef ABYSS_TICKET_ITEM_ID
#define ABYSS_TICKET_ITEM_ID 99001
#endif

#ifndef ABYSS_DAILY_LIMIT
#define ABYSS_DAILY_LIMIT 5
#endif

#ifndef DAILY_QUEST_MAX
#define DAILY_QUEST_MAX 5
#endif

#ifndef EVENT_MAX
#define EVENT_MAX 20
#endif

#ifndef EVENT_REWARD_MAX
#define EVENT_REWARD_MAX 5
#endif

#ifndef PVP_ROOM_MAX
#define PVP_ROOM_MAX 100
#endif

#ifndef PVP_ROOM_CAPACITY
#define PVP_ROOM_CAPACITY 8
#endif

typedef enum {
    RATTR_NONE = 0,
    RATTR_PHY_ATTACK,
    RATTR_MAG_ATTACK,
    RATTR_PHY_DEFENSE,
    RATTR_MAG_DEFENSE,
    RATTR_MAX_HP,
    RATTR_MAX_MP,
    RATTR_CRIT_RATE,
    RATTR_CRIT_DAMAGE,
    RATTR_ATTACK_SPEED,
    RATTR_MOVE_SPEED,
    RATTR_HP_REGEN,
    RATTR_MP_REGEN,
    RATTR_ALL_STATS,
    RATTR_ELEMENTAL_FIRE,
    RATTR_ELEMENTAL_ICE,
    RATTR_ELEMENTAL_LIGHT,
    RATTR_ELEMENTAL_DARK,
    RATTR_LIFESTEAL,
    RATTR_DAMAGE_REDUCE,
    RATTR_COOLDOWN_REDUCE,
    RATTR_MAX
} RandomAttributeType;

typedef enum {
    RARITY_NORMAL = 0,
    RARITY_MAGIC,
    RARITY_RARE,
    RARITY_EPIC,
    RARITY_LEGENDARY
} AttributeRarity;

typedef struct {
    uint8_t attr_type;
    int16_t value;
    uint8_t rarity;
} RandomAttribute;

typedef struct {
    uint8_t  difficulty;
    const char* name;
    uint8_t  min_level;
    uint8_t  recommended_level;
    uint16_t monster_level_scale;
    uint16_t reward_scale;
    uint8_t  room_count;
    uint8_t  fatigue_cost;
    uint8_t  unlock_clear_count;
} DungeonDifficultyInfo;

typedef enum {
    TITLE_TYPE_NORMAL = 0,
    TITLE_TYPE_RARE,
    TITLE_TYPE_EPIC,
    TITLE_TYPE_LEGENDARY,
    TITLE_TYPE_EVENT
} TitleType;

typedef struct {
    uint16_t title_id;
    uint8_t  type;
    int16_t  phy_attack;
    int16_t  mag_attack;
    int16_t  phy_defense;
    int16_t  mag_defense;
    int16_t  hp_bonus;
    int16_t  mp_bonus;
    int16_t  all_stats;
    uint8_t  move_speed;
    uint8_t  attack_speed;
} TitleInfo;

typedef struct {
    uint16_t ach_id;
    uint8_t  type;
    uint32_t target;
    uint32_t reward_gold;
    uint32_t reward_exp;
    uint16_t reward_item_id;
    uint8_t  reward_item_count;
    uint16_t reward_title_id;
} AchievementDef;

typedef struct {
    uint8_t  rarity;
    uint16_t material_ids[3];
    uint8_t  material_counts[3];
    uint8_t  material_count;
} DisassembleResult;

typedef struct {
    uint16_t quest_id;
    uint8_t  type;
    uint32_t target;
    uint32_t reward_gold;
    uint32_t reward_exp;
    uint16_t reward_item_id;
    uint8_t  reward_item_count;
} DailyQuestDef;

typedef struct {
    uint16_t event_id;
    char     name[64];
    uint8_t  type;
    uint8_t  state;
    uint32_t start_time;
    uint32_t end_time;
    uint32_t target;
    uint32_t reward_gold;
    uint32_t reward_exp;
    uint16_t reward_items[EVENT_REWARD_MAX];
    uint8_t  reward_counts[EVENT_REWARD_MAX];
} EventDef;

typedef struct {
    uint32_t recipe_id;
    uint32_t result_item_id;
    uint8_t  result_count;
    uint16_t required_mats[5];
    uint8_t  required_counts[5];
    uint32_t gold_cost;
    uint8_t  required_level;
} CraftRecipe;

typedef struct {
    uint32_t room_id;
    uint8_t  mode;
    uint8_t  state;
    uint32_t host_id;
    uint32_t players[PVP_ROOM_CAPACITY];
    uint8_t  player_count;
    uint8_t  team[PVP_ROOM_CAPACITY];
    uint8_t  ready[PVP_ROOM_CAPACITY];
    int32_t  scores[PVP_ROOM_CAPACITY];
    uint32_t start_time;
    uint16_t time_limit;
} PVPRoom;

extern const CraftRecipe g_craft_recipes[];
extern const uint8_t g_abyss_entry_cost[ABYSS_DIFFICULTY_MAX];
extern EventDef g_active_events[EVENT_MAX];
extern int g_active_event_count;
extern SharedDungeonInstance g_shared_dungeons[MAX_SHARED_DUNGEONS];

int parse_achievement_blob(const unsigned char* blob, unsigned long blob_len,
                           DBAchievementInfo* ach);
int parse_title_blob(const unsigned char* blob, unsigned long blob_len,
                     DBTitleInfo* title, const char* section_name);
int calc_upgrade_success_rate(uint8_t level);
uint32_t calc_upgrade_cost(uint32_t item_id, uint8_t level);
int check_and_level_up(ClientSession* s);
void decrease_equipment_durability(ClientSession* s, int amount);
void save_equipment_durability(ClientSession* s);
int get_skill_sp_cost(uint16_t skill_id, uint8_t current_level);
uint32_t get_enhance_cost(uint8_t current_level, uint8_t item_level);
int enhance_equipment(uint8_t current_level, uint8_t protect_type,
                      uint8_t* new_level, uint8_t* was_destroyed);
int generate_item_random_attrs(uint8_t item_level, uint8_t item_rarity,
                               RandomAttribute* attrs, int max_attrs);
int check_skill_prerequisite(uint16_t skill_id, uint8_t player_level,
                             uint16_t* player_skills, uint8_t* skill_levels,
                             int skill_count);
uint32_t get_skill_reset_cost(uint8_t player_level);
int validate_equipment(uint32_t item_id, uint8_t player_level,
                       uint8_t player_job, uint8_t player_sex,
                       uint8_t target_slot);
int db_try_stack_item(uint32_t charac_no, uint32_t item_id,
                      uint16_t count, int32_t* out_slot);
int db_consume_item(uint32_t charac_no, uint32_t ui_id, int amount);
int db_split_stack(uint32_t charac_no, int32_t source_slot,
                   int32_t target_slot, uint16_t split_count);
int db_disassemble_item(uint32_t charac_no, int32_t item_slot,
                        uint8_t item_rarity);
int db_equip_avatar(uint32_t charac_no, int32_t inv_slot, uint8_t avatar_slot);
int db_unequip_avatar(uint32_t charac_no, uint8_t avatar_slot,
                      int32_t target_inv_slot);
int db_get_enhance_level(uint32_t ui_id, uint8_t* level);
int db_update_enhance_level(uint32_t ui_id, uint8_t enhance_level);
int db_set_item_upgrade_level(uint32_t ui_id, uint8_t new_level);
int is_difficulty_unlocked(uint32_t charac_no, uint32_t dungeon_id,
                           uint8_t difficulty);
const DungeonDifficultyInfo* get_difficulty_info(uint8_t difficulty);
uint8_t get_guild_level_from_exp(uint32_t exp);
const TitleInfo* find_title(uint16_t title_id);
const AchievementDef* find_achievement(uint16_t ach_id);
int check_achievement_progress(uint32_t charac_no, uint8_t type, uint32_t value);
int synthesize_gems(uint8_t type, uint8_t grade, uint8_t* new_grade);
void calculate_abyss_boss_stats(uint16_t boss_id, uint8_t difficulty,
                                MonsterInfo* boss);
int can_enter_abyss(uint8_t player_level, uint16_t boss_id, uint8_t difficulty);
int generate_abyss_drops(uint16_t boss_id, uint8_t difficulty,
                         uint32_t* item_ids, uint8_t* counts, int max_drops);
void calculate_abyss_rewards(uint16_t boss_id, uint8_t difficulty,
                             uint32_t* gold, uint32_t* exp, uint16_t* item_id);
int validate_abyss_entry(ClientSession* s, uint16_t boss_id,
                         uint8_t difficulty, int consume_ticket);
PVPRoom* find_pvp_room(uint32_t room_id);
uint32_t create_pvp_room(uint32_t host_id, uint8_t mode);
int join_pvp_room(uint32_t room_id, uint32_t player_id);
int pvp_set_ready(uint32_t room_id, uint32_t player_id);
int pvp_start_match(uint32_t room_id);
int pvp_record_result(uint32_t room_id, uint32_t winner_id);
void pvp_cleanup_room(uint32_t room_id);
int generate_daily_quests(uint32_t charac_no, uint16_t* quest_ids, int count);
const DailyQuestDef* find_daily_quest(uint16_t quest_id);
EventDef* find_event(uint16_t event_id);
int is_event_active(uint16_t event_id);
const DisassembleResult* get_disassemble_result(uint8_t item_rarity);
const CraftRecipe* find_craft_recipe(uint32_t recipe_id);
int check_craft_materials(uint32_t charac_no, const CraftRecipe* recipe);
uint32_t db_get_charac_no_by_name(const char* name);

#ifdef __cplusplus
}
#endif

#endif /* HANDLERS_GAMEPLAY_HELPERS_H */
