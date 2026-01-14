#ifndef GAME_SERVER_TYPES_H
#define GAME_SERVER_TYPES_H

#include <stdint.h>
#include "db/db_types.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 65536
#endif

#ifndef MAX_SESSIONS
#define MAX_SESSIONS 256
#endif

#ifndef MAX_ACCOUNT_NAME
#define MAX_ACCOUNT_NAME 256
#endif

#ifndef MAX_CHARACTERS
#define MAX_CHARACTERS 16
#endif

#ifndef GAME_SERVER_TYPES_DEFINED
#define GAME_SERVER_TYPES_DEFINED
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    STATE_CONNECTED = 0,
    STATE_AUTHENTICATED,
    STATE_CHAR_SELECT,
    STATE_IN_GAME,
    STATE_IN_DUNGEON,
    STATE_DISCONNECTING
} SessionState;

typedef enum {
    DUNGEON_NORMAL = 0,
    DUNGEON_HARD,
    DUNGEON_EXPERT,
    DUNGEON_MASTER,
    DUNGEON_KING
} DungeonDifficulty;

typedef enum {
    MONSTER_STATE_IDLE = 0,
    MONSTER_STATE_PATROL,
    MONSTER_STATE_CHASE,
    MONSTER_STATE_ATTACK,
    MONSTER_STATE_HIT_STUN,
    MONSTER_STATE_DEAD,
    MONSTER_STATE_SKILL_CAST,
    MONSTER_STATE_SKILL_RECOVER,
    MONSTER_STATE_FLEE,
    MONSTER_STATE_SUMMON,
    MONSTER_STATE_ENRAGE
} MonsterState;

typedef enum {
    MONSTER_AI_PASSIVE = 0,
    MONSTER_AI_NORMAL,
    MONSTER_AI_AGGRESSIVE,
    MONSTER_AI_BOSS,
    MONSTER_AI_RANGED,
    MONSTER_AI_SUPPORT,
    MONSTER_AI_ASSASSIN,
    MONSTER_AI_TANK,
    MONSTER_AI_SWARM,
    MONSTER_AI_STATIONARY
} MonsterAIType;

typedef enum {
    MONSTER_ATTACK_MELEE = 0,
    MONSTER_ATTACK_RANGED,
    MONSTER_ATTACK_AOE_CIRCLE,
    MONSTER_ATTACK_AOE_CONE,
    MONSTER_ATTACK_AOE_LINE,
    MONSTER_ATTACK_CHARGE,
    MONSTER_ATTACK_GRAB,
    MONSTER_ATTACK_BUFF_SELF,
    MONSTER_ATTACK_DEBUFF_PLAYER,
    MONSTER_ATTACK_SUMMON,
    MONSTER_ATTACK_HEAL
} MonsterAttackType;

typedef struct {
    uint16_t skill_id;
    uint8_t  attack_type;
    uint16_t damage_base;
    uint16_t damage_scaling;
    uint16_t range;
    uint16_t aoe_radius;
    uint16_t cooldown_ticks;
    uint16_t cast_time_ticks;
    uint16_t recovery_ticks;
    uint8_t  debuff_type;
    uint16_t debuff_duration_ms;
    int16_t  debuff_value;
    uint8_t  element;
} MonsterSkillInfo;

typedef struct {
    uint32_t monster_id;
    uint16_t monster_type;
    uint16_t x;
    uint16_t y;
    uint32_t hp;
    uint32_t max_hp;
    uint8_t  alive;
    uint8_t  state;
    uint8_t  ai_type;
    uint16_t target_x;
    uint16_t target_y;
    uint16_t spawn_x;
    uint16_t spawn_y;
    uint32_t target_player;
    uint32_t last_action_time;
    uint32_t attack_cooldown;
    uint16_t move_speed;
    uint16_t aggro_range;
    uint16_t attack_range;
    uint32_t attack_damage;
    uint8_t  hit_stun_frames;
    uint8_t  level;
    uint8_t  attack_pattern;
    uint16_t skill_cooldowns[4];
    uint8_t  current_skill;
    uint16_t cast_timer;
    uint16_t recovery_timer;
    uint8_t  enraged;
    uint16_t preferred_distance;
    uint32_t phys_def;
    uint32_t mag_def;
    uint8_t  is_boss;
    uint8_t  boss_phase;
    uint8_t  weak_element;
    uint8_t  armor_broken;
    uint16_t armor_break_timer;
    uint8_t  skill_cycle_index;
    uint16_t phase_skill_cd[3];
    uint32_t boss_id;
} MonsterInfo;

#ifndef MAX_MONSTERS
#define MAX_MONSTERS 32
#endif

typedef struct {
    uint32_t dungeon_id;
    uint8_t  difficulty;
    uint8_t  current_room;
    uint8_t  total_rooms;
    uint8_t  active;
    uint32_t start_time;
    uint32_t time_limit;
    MonsterInfo monsters[MAX_MONSTERS];
    int monster_count;
    int monsters_killed;
    uint32_t party_id;
    uint32_t shared_dungeon_id;
    uint8_t  is_party_host;
    uint8_t  free_resurrect_used;
    uint8_t  death_count;
} DungeonInstance;

#ifndef MAX_SHARED_DUNGEONS
#define MAX_SHARED_DUNGEONS 64
#endif

#ifndef MAX_PARTY_MEMBERS_DUNGEON
#define MAX_PARTY_MEMBERS_DUNGEON 4
#endif

typedef struct {
    uint32_t shared_id;
    uint32_t party_id;
    uint32_t dungeon_id;
    uint8_t  difficulty;
    uint8_t  current_room;
    uint8_t  total_rooms;
    uint8_t  active;
    uint32_t start_time;
    uint32_t time_limit;
    MonsterInfo monsters[MAX_MONSTERS];
    int monster_count;
    int monsters_killed;
    uint32_t member_fds[MAX_PARTY_MEMBERS_DUNGEON];
    uint8_t  member_count;
} SharedDungeonInstance;

#ifndef MAX_PARTY_MEMBERS
#define MAX_PARTY_MEMBERS 4
#endif

#ifndef MAX_PARTIES
#define MAX_PARTIES 256
#endif

typedef struct {
    uint32_t party_id;
    uint32_t leader_id;
    uint32_t members[MAX_PARTY_MEMBERS];
    uint8_t  member_count;
    uint8_t  loot_mode;
    uint8_t  active;
} PartyInfo;

typedef enum {
    PVP_MODE_1V1 = 0,
    PVP_MODE_2V2,
    PVP_MODE_3V3,
    PVP_MODE_FREE_FOR_ALL
} PVPMode;

typedef enum {
    PVP_STATE_NONE = 0,
    PVP_STATE_QUEUED,
    PVP_STATE_MATCH_FOUND,
    PVP_STATE_IN_MATCH,
    PVP_STATE_MATCH_END
} PVPPlayerState;

typedef struct {
    uint32_t player_id;
    uint32_t player_fd;
    uint8_t  mode;
    uint32_t pvp_points;
    uint32_t queue_time;
    uint8_t  active;
} PVPQueueEntry;

typedef struct {
    uint32_t match_id;
    uint8_t  mode;
    uint8_t  state;
    uint32_t team1[4];
    uint32_t team2[4];
    uint8_t  team1_count;
    uint8_t  team2_count;
    uint32_t team1_hp[4];
    uint32_t team2_hp[4];
    uint8_t  team1_ready[4];
    uint8_t  team2_ready[4];
    uint32_t start_time;
    uint32_t winner;
} PVPMatch;

typedef struct {
    uint32_t charac_no;
    uint32_t pvp_points;
    uint32_t wins;
    uint32_t losses;
    uint8_t  rank;
    uint8_t  current_state;
    uint32_t current_match_id;
    uint8_t  team;
} PVPPlayerStats;

#ifndef MAX_PVP_QUEUE
#define MAX_PVP_QUEUE 64
#endif

#ifndef MAX_PVP_MATCHES
#define MAX_PVP_MATCHES 32
#endif

typedef struct {
    uint32_t str;
    uint32_t intel;
    uint32_t vit;
    uint32_t spr;
    uint32_t phys_attack;
    uint32_t mag_attack;
    uint32_t phys_def;
    uint32_t mag_def;
    uint32_t max_hp;
    uint32_t max_mp;
    uint32_t attack_speed;
    uint32_t move_speed;
    uint32_t cast_speed;
    uint32_t crit_rate;
    uint32_t hit_rate;
    uint32_t evasion;
    int32_t buff_attack_mod;
    int32_t buff_magic_mod;
    int32_t buff_def_mod;
    int32_t buff_mdef_mod;
    int32_t buff_speed_mod;
    int32_t buff_aspeed_mod;
    int32_t buff_crit_mod;
} CombatStats;

#ifndef MAX_ACHIEVEMENTS
#define MAX_ACHIEVEMENTS 128
#endif

typedef struct {
    uint32_t ach_id;
    uint8_t  completed;
    uint8_t  progress;
    uint16_t points;
    uint32_t complete_time;
} DBAchievementEntry;

typedef struct {
    uint32_t charac_no;
    uint32_t total_points;
    DBAchievementEntry achievements[MAX_ACHIEVEMENTS];
    uint8_t  achievement_count;
    uint8_t  achievement_loaded;
} DBAchievementInfo;

#ifndef MAX_TITLES
#define MAX_TITLES 64
#endif

typedef struct {
    uint32_t title_id;
    uint8_t  unlocked;
    uint32_t unlock_time;
} DBTitleEntry;

typedef struct {
    uint32_t charac_no;
    uint32_t equipped_title;
    uint16_t equipped_titles[3];
    DBTitleEntry titles[MAX_TITLES];
    uint8_t  title_count;
    uint8_t  title_loaded;
} DBTitleInfo;

// Moved to db_types.h: typedef struct {
// Moved to db_types.h:     uint32_t uid;
// Moved to db_types.h:     char     accountname[MAX_ACCOUNT_NAME];
// Moved to db_types.h:     char     password[256];
// Moved to db_types.h:     char     vip[256];
// Moved to db_types.h: } DBAccountInfo;

#ifndef MAX_SKILL_COOLDOWNS
#define MAX_SKILL_COOLDOWNS 64
#endif

#ifndef MAX_ACTIVE_BUFFS
#define MAX_ACTIVE_BUFFS 32
#endif

typedef struct _ClientSession {
    int fd;
    uint32_t ip_addr;
    uint16_t port;
    SessionState state;
    uint8_t recv_buffer[BUFFER_SIZE];
    int recv_len;
    uint32_t account_id;
    uint32_t character_id;
    char account_name[MAX_ACCOUNT_NAME];
    char character_name[MAX_CHARAC_NAME];
    DBCharacterInfo characters[MAX_CHARACTERS];
    int character_count;
    uint32_t connect_time;
    uint32_t last_activity;
    DungeonInstance dungeon;
    DBInventoryInfo inventory;
    DBSkillInfo skill;
    DBEquipmentInfo equipment;
    DBAchievementInfo achievement;
    DBTitleInfo title;
    CombatStats combat_stats;
    uint32_t current_hp;
    uint32_t current_mp;
    uint32_t active_quests[10];
    uint8_t  quest_progress[10];
    uint8_t  active_quest_count;
    uint32_t completed_quests[64];
    uint8_t  completed_quest_count;
    uint32_t guild_id;
    uint32_t party_id;
    uint32_t current_village;
    uint8_t  pvp_state;
    uint32_t pvp_points;
    uint32_t pvp_wins;
    uint32_t pvp_losses;
    uint32_t pvp_match_id;
    uint8_t  pvp_team;
    uint8_t  pvp_team_index;
    uint32_t active_pet_id;
    uint32_t pet_hp;
    uint32_t pet_max_hp;
    uint8_t  pet_state;
    struct {
        uint16_t skill_id;
        uint32_t ready_time;
    } skill_cooldowns[MAX_SKILL_COOLDOWNS];
    uint8_t cooldown_count;
    struct {
        uint16_t buff_id;
        uint32_t source_id;
        uint32_t expire_time;
        int32_t  value;
        uint8_t  stacks;
    } active_buffs[MAX_ACTIVE_BUFFS];
    uint8_t buff_count;
    uint32_t last_attack_time;
    uint32_t combat_start_time;
    uint8_t  in_combat;
    uint32_t stat_monsters_killed;
    uint32_t stat_bosses_killed;
    uint32_t stat_dungeons_cleared;
    uint32_t stat_no_death_clears;
    uint32_t stat_party_clears;
    uint32_t stat_total_damage;
    uint32_t stat_crit_damage;
    uint8_t  level;
    uint8_t  job;
    struct {
        uint16_t skills[5];
        uint8_t  count;
        uint32_t last_skill_time;
        uint8_t  combo_level;
    } combo_state;
    uint8_t  pending_teleport;
    uint16_t teleport_map_id;
    uint8_t  quest_status[10];
    uint8_t  tracked_quests[3];
    uint8_t  tracked_quest_count;
    uint16_t current_map;
    uint16_t pos_x;
    uint16_t pos_y;
    int active;
} ClientSession;

#ifdef __cplusplus
}
#endif

#endif /* GAME_SERVER_TYPES_H */
