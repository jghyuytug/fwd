/*
 * main_db.cpp - DNF Game Server with MySQL Database Integration
 *
 * Extends main.cpp with real database queries:
 * - Account authentication from d_taiwan.accounts
 * - Character list from taiwan_cain.charac_info
 *
 * Compatible with GCC 4.1.2 / CentOS 5.8 (VM-131)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <mysql/mysql.h>
#include <zlib.h>

/*==============================================================================
 * Configuration
 *============================================================================*/

#define PORT 10011
#define MAX_EVENTS 64
#define BUFFER_SIZE 65536
#define PACKET_HEADER_SIZE 13

#define MAX_SESSIONS 256
#define MAX_CHARAC_NAME 20
#define MAX_ACCOUNT_NAME 256
#define MAX_CHARACTERS 16

/* Database configuration */
#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASS "88888888"
#define DB_ACCOUNT "d_taiwan"
#define DB_CHARAC "taiwan_cain"
#define DB_CHARAC2 "taiwan_cain_2nd"
#define DB_GUILD "d_guild"
#define DB_PORT 3306

/*==============================================================================
 * Data Structures
 *============================================================================*/

/* Session states */
typedef enum {
    STATE_CONNECTED = 0,
    STATE_AUTHENTICATED,
    STATE_CHAR_SELECT,
    STATE_IN_GAME,
    STATE_IN_DUNGEON,
    STATE_DISCONNECTING
} SessionState;

/* Dungeon difficulty */
typedef enum {
    DUNGEON_NORMAL = 0,
    DUNGEON_HARD,
    DUNGEON_EXPERT,
    DUNGEON_MASTER,
    DUNGEON_KING
} DungeonDifficulty;

/* Monster AI states */
typedef enum {
    MONSTER_STATE_IDLE = 0,
    MONSTER_STATE_PATROL,
    MONSTER_STATE_CHASE,
    MONSTER_STATE_ATTACK,
    MONSTER_STATE_HIT_STUN,
    MONSTER_STATE_DEAD
} MonsterState;

/* Monster behavior types */
typedef enum {
    MONSTER_AI_PASSIVE = 0,   /* Won't attack unless attacked */
    MONSTER_AI_NORMAL,        /* Standard aggro range */
    MONSTER_AI_AGGRESSIVE,    /* Large aggro range */
    MONSTER_AI_BOSS           /* Special boss behavior */
} MonsterAIType;

/* Monster info */
typedef struct {
    uint32_t monster_id;
    uint16_t monster_type;
    uint16_t x;
    uint16_t y;
    uint32_t hp;
    uint32_t max_hp;
    uint8_t  alive;
    /* AI fields */
    uint8_t  state;           /* MonsterState */
    uint8_t  ai_type;         /* MonsterAIType */
    uint16_t target_x;        /* Movement target */
    uint16_t target_y;
    uint16_t spawn_x;         /* Original spawn position */
    uint16_t spawn_y;
    uint32_t target_player;   /* Player charac_no being chased */
    uint32_t last_action_time;/* Timestamp of last action */
    uint32_t attack_cooldown; /* Ticks until can attack again */
    uint16_t move_speed;      /* Pixels per tick */
    uint16_t aggro_range;     /* Detection range */
    uint16_t attack_range;    /* Attack reach */
    uint32_t attack_damage;   /* Damage per hit */
    uint8_t  hit_stun_frames; /* Frames remaining in hit stun */
} MonsterInfo;

#define MAX_MONSTERS 32

/* Dungeon instance */
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
    /* Party dungeon support */
    uint32_t party_id;               /* 0 = solo, >0 = party dungeon */
    uint32_t shared_dungeon_id;      /* Index into g_shared_dungeons */
    uint8_t  is_party_host;          /* 1 = leader who created the instance */
} DungeonInstance;

/*==============================================================================
 * Shared Party Dungeon Instances
 *============================================================================*/
#define MAX_SHARED_DUNGEONS 64
#define MAX_PARTY_MEMBERS_DUNGEON 4  /* Maximum party members in dungeon */

/* Shared dungeon instance for party play */
typedef struct {
    uint32_t shared_id;              /* Unique shared instance ID */
    uint32_t party_id;               /* Owning party */
    uint32_t dungeon_id;             /* Dungeon template ID */
    uint8_t  difficulty;
    uint8_t  current_room;
    uint8_t  total_rooms;
    uint8_t  active;
    uint32_t start_time;
    uint32_t time_limit;
    MonsterInfo monsters[MAX_MONSTERS];
    int monster_count;
    int monsters_killed;
    uint32_t member_fds[MAX_PARTY_MEMBERS_DUNGEON]; /* FDs of party members in dungeon */
    uint8_t  member_count;
} SharedDungeonInstance;

static SharedDungeonInstance g_shared_dungeons[MAX_SHARED_DUNGEONS];
static uint32_t g_next_shared_dungeon_id = 1;

/* Forward declarations for party system (needed for party dungeon) */
#define MAX_PARTY_MEMBERS 4
typedef struct {
    uint32_t party_id;
    uint32_t leader_id;
    uint32_t members[MAX_PARTY_MEMBERS];
    uint8_t  member_count;
    uint8_t  loot_mode;
    uint8_t  active;
} PartyInfo;

#define MAX_PARTIES 256
static PartyInfo g_parties[MAX_PARTIES];
static uint32_t g_next_party_id = 1;

/* Forward declaration */
static PartyInfo* find_party(uint32_t party_id);

/* Bag item (from user_items, slot >= 11) */
#define MAX_BAG_ITEMS 64
typedef struct {
    uint32_t ui_id;
    uint16_t slot;
    uint32_t it_id;
    uint16_t count;       /* For stackable items */
    uint8_t  upgrade;
    uint8_t  seal;
    uint16_t flags;
} DBBagItem;

/* Inventory data from DB */
typedef struct {
    uint32_t charac_no;
    uint32_t money;
    uint32_t coin;
    uint32_t pay_coin;
    uint32_t event_coin;
    uint32_t inventory_capacity;
    DBBagItem items[MAX_BAG_ITEMS];
    uint16_t item_count;
    uint8_t  inventory_loaded;
} DBInventoryInfo;

/* Skill slot constants */
#define MAX_SKILL_SLOTS      204    /* 102 slots per skill tree * 2 */
#define MAX_PARSED_SKILLS    64     /* Max skills to track */
#define SKILL_SLOT_BLOB_SIZE 408    /* Decompressed skill_slot size */

/* Skill data from DB */
typedef struct {
    uint32_t charac_no;
    uint32_t remain_sp;
    uint32_t used_sp;
    uint32_t remain_sp_2nd;
    /* Parsed skill IDs from skill_slot blob */
    uint16_t skill_ids[MAX_PARSED_SKILLS];   /* Non-zero skill IDs */
    uint8_t  skill_count;                     /* Number of learned skills */
    uint8_t  skill_loaded;
} DBSkillInfo;

/* Equipment slot types */
#define EQUIP_SLOT_WEAPON    0
#define EQUIP_SLOT_HELMET    1
#define EQUIP_SLOT_SHOULDER  2
#define EQUIP_SLOT_CHEST     3
#define EQUIP_SLOT_BELT      4
#define EQUIP_SLOT_PANTS     5
#define EQUIP_SLOT_SHOES     6
#define EQUIP_SLOT_BRACELET  7
#define EQUIP_SLOT_NECKLACE  8
#define EQUIP_SLOT_RING      9
#define EQUIP_SLOT_SUBWEAPON 10
#define MAX_EQUIP_SLOTS      11

/* Item data from DB */
typedef struct {
    uint32_t ui_id;       /* Unique item ID */
    uint32_t slot;        /* Slot position */
    uint32_t it_id;       /* Item template ID */
    uint8_t  stat;        /* Item stat/enhance level */
    uint16_t hidden_option;
} DBItemInfo;

/* Equipment data from DB */
typedef struct {
    uint32_t charac_no;
    DBItemInfo equipment[MAX_EQUIP_SLOTS];  /* Equipped items */
    uint8_t  equip_count;
    uint8_t  equip_loaded;
} DBEquipmentInfo;

/* Combat stats (calculated from level, job, equipment) */
typedef struct {
    uint32_t str;           /* Strength - physical damage */
    uint32_t intel;         /* Intelligence - magic damage */
    uint32_t vit;           /* Vitality - HP */
    uint32_t spr;           /* Spirit - MP */
    uint32_t phys_attack;   /* Physical attack power */
    uint32_t mag_attack;    /* Magic attack power */
    uint32_t phys_def;      /* Physical defense */
    uint32_t mag_def;       /* Magic defense */
    uint32_t max_hp;        /* Maximum HP */
    uint32_t max_mp;        /* Maximum MP */
    uint32_t attack_speed;  /* Attack speed */
    uint32_t move_speed;    /* Movement speed */
    uint32_t cast_speed;    /* Cast speed */
    uint32_t crit_rate;     /* Critical rate (0.01%) */
    uint32_t hit_rate;      /* Hit rate */
    uint32_t evasion;       /* Evasion rate */
} CombatStats;

/* Job base stats multipliers */
#define JOB_SLAYER      0   /* STR based */
#define JOB_FIGHTER     1   /* STR based */
#define JOB_GUNNER      2   /* STR/INT hybrid */
#define JOB_MAGE        3   /* INT based */
#define JOB_PRIEST      4   /* INT/SPR based */
#define JOB_THIEF       5   /* STR based */

/* Character info from database - 基于生产 charac_info 表结构 */
typedef struct {
    uint32_t charac_no;
    char     charac_name[MAX_CHARAC_NAME];
    uint8_t  job;
    uint8_t  grow_type;
    uint8_t  level;
    uint32_t exp;
    uint16_t hp;
    uint16_t max_hp;
    uint16_t max_mp;
    int32_t  village;
    uint32_t gold;
    int16_t  fatigue;
    uint8_t  sex;
    uint8_t  deleted;
    uint32_t guild_id;              /* 公会ID */
    uint32_t dungeon_clear_point;   /* 副本通关点数 */
    uint32_t create_time;           /* 创建时间戳 */
    /* 扩展字段 - 来自生产数据库 */
    uint8_t  expert_job;            /* 专家职业 */
    int8_t   skill_tree_index;      /* 技能树索引 (-1=未选) */
    uint16_t phy_attack;            /* 物理攻击 */
    uint16_t phy_defense;           /* 物理防御 */
    uint16_t mag_attack;            /* 魔法攻击 */
    uint16_t mag_defense;           /* 魔法防御 */
    uint16_t move_speed;            /* 移动速度 */
    uint16_t attack_speed;          /* 攻击速度 */
    uint16_t cast_speed;            /* 施法速度 */
    uint16_t hit_recovery;          /* 硬直恢复 */
    uint16_t jump;                  /* 跳跃力 */
    int32_t  charac_weight;         /* 角色重量 */
    int16_t  max_fatigue;           /* 最大疲劳值 */
    uint8_t  guild_right;           /* 公会权限 */
    uint8_t  member_flag;           /* 会员标记 */
    /* 装备外观数据 - 用于角色选择界面 */
    uint32_t equip_item_ids[MAX_EQUIP_SLOTS];  /* 装备物品ID (slot 0-10) */
} DBCharacterInfo;

/* Account info from database */
typedef struct {
    uint32_t uid;
    char     accountname[MAX_ACCOUNT_NAME];
    char     password[256];
    char     vip[256];
} DBAccountInfo;

/* Client session */
typedef struct {
    int fd;
    uint32_t ip_addr;
    uint16_t port;
    SessionState state;

    /* Buffer */
    uint8_t recv_buffer[BUFFER_SIZE];
    int recv_len;

    /* User data */
    uint32_t account_id;
    uint32_t character_id;
    char account_name[MAX_ACCOUNT_NAME];
    char character_name[MAX_CHARAC_NAME];

    /* Characters loaded from DB */
    DBCharacterInfo characters[MAX_CHARACTERS];
    int character_count;

    /* Activity tracking */
    uint32_t connect_time;
    uint32_t last_activity;

    /* Dungeon state */
    DungeonInstance dungeon;

    /* Inventory from DB */
    DBInventoryInfo inventory;

    /* Skill from DB */
    DBSkillInfo skill;

    /* Equipment from DB */
    DBEquipmentInfo equipment;

    /* Calculated combat stats */
    CombatStats combat_stats;

    /* Current character status */
    uint32_t current_hp;
    uint32_t current_mp;

    /* Quest tracking (simplified) */
    uint32_t active_quests[10];    /* Quest IDs */
    uint8_t  quest_progress[10];   /* Progress 0-100 */
    uint8_t  active_quest_count;
    uint32_t completed_quests[64]; /* Completed quest IDs */
    uint8_t  completed_quest_count;

    /* Social features */
    uint32_t guild_id;            /* Guild ID (0 = no guild) */
    uint32_t party_id;            /* Party ID (0 = no party) */
    uint32_t current_village;     /* Current area/map */

    int active;
} ClientSession;

/*==============================================================================
 * Global State
 *============================================================================*/

static int g_running = 1;
static int g_listen_fd = -1;
static int g_epoll_fd = -1;
static uint16_t g_sequence = 0;

static ClientSession g_sessions[MAX_SESSIONS];
static int g_session_count = 0;

/* Database connections */
static MYSQL* g_db_account = NULL;
static MYSQL* g_db_charac = NULL;
static MYSQL* g_db_charac2 = NULL;
static MYSQL* g_db_guild = NULL;
/* Check if a character is currently online */
static int is_character_online(uint32_t charac_no) {
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active &&
            g_sessions[i].state == STATE_IN_GAME &&
            g_sessions[i].character_id == charac_no) {
            return 1;
        }
    }
    return 0;
}

/* Get online status for multiple characters */
static void update_online_status(uint32_t* charac_nos, uint8_t* online_status, int count) {
    for (int i = 0; i < count; i++) {
        online_status[i] = (uint8_t)is_character_online(charac_nos[i]);
    }
}


/*==============================================================================
 * Database Functions
 *============================================================================*/

/* Initialize database connections */
static int db_init() {
    printf("[DB] Initializing database connections...\n");

    unsigned int timeout = 5;  /* 5 second timeout */

    /* Account database (d_taiwan) */
    g_db_account = mysql_init(NULL);
    if (!g_db_account) {
        printf("[DB] Failed to init account db handle\n");
        return -1;
    }
    mysql_options(g_db_account, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&timeout);
    mysql_options(g_db_account, MYSQL_OPT_READ_TIMEOUT, (const char*)&timeout);
    mysql_options(g_db_account, MYSQL_OPT_WRITE_TIMEOUT, (const char*)&timeout);

    if (!mysql_real_connect(g_db_account, DB_HOST, DB_USER, DB_PASS,
                            DB_ACCOUNT, DB_PORT, NULL, 0)) {
        printf("[DB] Account DB connect failed: %s\n", mysql_error(g_db_account));
        mysql_close(g_db_account);
        g_db_account = NULL;
        return -1;
    }
    printf("[DB] Connected to %s\n", DB_ACCOUNT);

    /* Character database (taiwan_cain) */
    g_db_charac = mysql_init(NULL);
    if (!g_db_charac) {
        printf("[DB] Failed to init charac db handle\n");
        mysql_close(g_db_account);
        g_db_account = NULL;
        return -1;
    }
    mysql_options(g_db_charac, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&timeout);
    mysql_options(g_db_charac, MYSQL_OPT_READ_TIMEOUT, (const char*)&timeout);
    mysql_options(g_db_charac, MYSQL_OPT_WRITE_TIMEOUT, (const char*)&timeout);

    if (!mysql_real_connect(g_db_charac, DB_HOST, DB_USER, DB_PASS,
                            DB_CHARAC, DB_PORT, NULL, 0)) {
        printf("[DB] Character DB connect failed: %s\n", mysql_error(g_db_charac));
        mysql_close(g_db_account);
        mysql_close(g_db_charac);
        g_db_account = NULL;
        g_db_charac = NULL;
        return -1;
    }
    printf("[DB] Connected to %s\n", DB_CHARAC);

    /* Character database 2 (taiwan_cain_2nd) - inventory */
    g_db_charac2 = mysql_init(NULL);
    if (!g_db_charac2) {
        printf("[DB] Failed to init charac2 db handle\n");
        mysql_close(g_db_account);
        mysql_close(g_db_charac);
        g_db_account = NULL;
        g_db_charac = NULL;
        return -1;
    }
    mysql_options(g_db_charac2, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&timeout);
    mysql_options(g_db_charac2, MYSQL_OPT_READ_TIMEOUT, (const char*)&timeout);
    mysql_options(g_db_charac2, MYSQL_OPT_WRITE_TIMEOUT, (const char*)&timeout);

    if (!mysql_real_connect(g_db_charac2, DB_HOST, DB_USER, DB_PASS,
                            DB_CHARAC2, DB_PORT, NULL, 0)) {
        printf("[DB] Charac2 DB connect failed: %s\n", mysql_error(g_db_charac2));
        mysql_close(g_db_account);
        mysql_close(g_db_charac);
        mysql_close(g_db_charac2);
        g_db_account = NULL;
        g_db_charac = NULL;
        g_db_charac2 = NULL;
        return -1;
    }
    printf("[DB] Connected to %s\n", DB_CHARAC2);

    /* Guild database (d_guild) */
    g_db_guild = mysql_init(NULL);
    if (!g_db_guild) {
        printf("[DB] Failed to init guild db handle\n");
        mysql_close(g_db_account);
        mysql_close(g_db_charac);
        mysql_close(g_db_charac2);
        g_db_account = NULL;
        g_db_charac = NULL;
        g_db_charac2 = NULL;
        return -1;
    }
    mysql_options(g_db_guild, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&timeout);
    mysql_options(g_db_guild, MYSQL_OPT_READ_TIMEOUT, (const char*)&timeout);
    mysql_options(g_db_guild, MYSQL_OPT_WRITE_TIMEOUT, (const char*)&timeout);

    if (!mysql_real_connect(g_db_guild, DB_HOST, DB_USER, DB_PASS,
                            DB_GUILD, DB_PORT, NULL, 0)) {
        printf("[DB] Guild DB connect failed: %s\n", mysql_error(g_db_guild));
        mysql_close(g_db_account);
        mysql_close(g_db_charac);
        mysql_close(g_db_charac2);
        mysql_close(g_db_guild);
        g_db_account = NULL;
        g_db_charac = NULL;
        g_db_charac2 = NULL;
        g_db_guild = NULL;
        return -1;
    }
    printf("[DB] Connected to %s\n", DB_GUILD);

    /* Set character encoding */
    mysql_set_character_set(g_db_account, "utf8");
    mysql_set_character_set(g_db_charac, "utf8");
    mysql_set_character_set(g_db_charac2, "utf8");
    mysql_set_character_set(g_db_guild, "utf8");

    printf("[DB] Database initialization complete\n");
    return 0;
}

/* Cleanup database connections */
static void db_cleanup() {
    if (g_db_account) {
        mysql_close(g_db_account);
        g_db_account = NULL;
    }
    if (g_db_charac) {
        mysql_close(g_db_charac);
        g_db_charac = NULL;
    }
    if (g_db_charac2) {
        mysql_close(g_db_charac2);
        g_db_charac2 = NULL;
    }
    if (g_db_guild) {
        mysql_close(g_db_guild);
        g_db_guild = NULL;
    }
    printf("[DB] Database connections closed\n");
}

/* Get inventory data from taiwan_cain_2nd.inventory + user_items */
static int db_get_inventory(uint32_t charac_no, DBInventoryInfo* inv) {
    if (!g_db_charac2 || !inv) return -1;

    memset(inv, 0, sizeof(DBInventoryInfo));
    inv->charac_no = charac_no;

    /* Query 1: Get money/coin from inventory table */
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT money, coin, pay_coin, event_coin, inventory_capacity "
             "FROM inventory WHERE charac_no=%u",
             charac_no);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Inventory query failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (result) {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row) {
            inv->money = row[0] ? (uint32_t)strtoul(row[0], NULL, 10) : 0;
            inv->coin = row[1] ? (uint32_t)strtoul(row[1], NULL, 10) : 0;
            inv->pay_coin = row[2] ? (uint32_t)strtoul(row[2], NULL, 10) : 0;
            inv->event_coin = row[3] ? (uint32_t)strtoul(row[3], NULL, 10) : 0;
            inv->inventory_capacity = row[4] ? (uint32_t)strtoul(row[4], NULL, 10) : 56;
        }
        mysql_free_result(result);
    }

    /* Query 2: Get bag items from user_items (slot >= 11 are bag slots) */
    snprintf(query, sizeof(query),
             "SELECT ui_id, slot, it_id, stat FROM user_items "
             "WHERE charac_no=%u AND slot >= 11 ORDER BY slot LIMIT %d",
             charac_no, MAX_BAG_ITEMS);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Bag items query failed: %s\n", mysql_error(g_db_charac2));
    } else {
        result = mysql_store_result(g_db_charac2);
        if (result) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result)) && inv->item_count < MAX_BAG_ITEMS) {
                DBBagItem* item = &inv->items[inv->item_count];
                item->ui_id = row[0] ? (uint32_t)strtoul(row[0], NULL, 10) : 0;
                item->slot = row[1] ? (uint16_t)strtoul(row[1], NULL, 10) : 0;
                item->it_id = row[2] ? (uint32_t)strtoul(row[2], NULL, 10) : 0;
                item->upgrade = row[3] ? (uint8_t)strtoul(row[3], NULL, 10) : 0;
                item->count = 1;  /* Default count */
                item->seal = 0;
                item->flags = 0;
                inv->item_count++;
            }
            mysql_free_result(result);
        }
    }

    inv->inventory_loaded = 1;
    printf("[DB] Loaded inventory for charac_no=%u: money=%u, items=%u\n",
           charac_no, inv->money, inv->item_count);
    return 0;
}

/* Parse skill_slot blob - zlib compressed 408-byte skill array
 * Format: [4 bytes: uncompressed_size][N bytes: zlib data]
 * Uncompressed: 204 x 2-byte skill IDs (102 slots x 2 trees)
 */
static int parse_skill_slot_blob(const unsigned char* blob, unsigned long blob_len,
                                  DBSkillInfo* skill) {
    if (!blob || blob_len < 8 || !skill) return -1;

    /* First 4 bytes is uncompressed size (little-endian) */
    uint32_t uncompressed_size = blob[0] | (blob[1] << 8) | (blob[2] << 16) | (blob[3] << 24);
    if (uncompressed_size > 1024) {
        printf("[DB] Skill blob uncompressed size too large: %u\n", uncompressed_size);
        return -1;
    }

    /* Decompress the zlib data */
    unsigned char decompressed[1024];
    uLongf dest_len = sizeof(decompressed);

    int ret = uncompress(decompressed, &dest_len, blob + 4, blob_len - 4);
    if (ret != Z_OK) {
        printf("[DB] Skill slot decompression failed: ret=%d\n", ret);
        return -1;
    }

    /* Parse 2-byte skill IDs from decompressed data */
    skill->skill_count = 0;
    for (unsigned long i = 0; i + 1 < dest_len && skill->skill_count < MAX_PARSED_SKILLS; i += 2) {
        uint16_t skill_id = decompressed[i] | (decompressed[i + 1] << 8);
        if (skill_id > 0) {
            skill->skill_ids[skill->skill_count++] = skill_id;
        }
    }

    return 0;
}

/* Get skill data from taiwan_cain_2nd.skill */
static int db_get_skill(uint32_t charac_no, DBSkillInfo* skill) {
    if (!g_db_charac2 || !skill) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT remain_sp, used_sp, remain_sp_2nd, skill_slot FROM skill WHERE charac_no=%u",
             charac_no);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Skill query failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) {
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        return -1;
    }

    /* Get field lengths for blob */
    unsigned long* lengths = mysql_fetch_lengths(result);

    memset(skill, 0, sizeof(DBSkillInfo));
    skill->charac_no = charac_no;
    skill->remain_sp = row[0] ? (uint32_t)strtoul(row[0], NULL, 10) : 0;
    skill->used_sp = row[1] ? (uint32_t)strtoul(row[1], NULL, 10) : 0;
    skill->remain_sp_2nd = row[2] ? (uint32_t)strtoul(row[2], NULL, 10) : 0;

    /* Parse skill_slot blob if present */
    if (row[3] && lengths[3] > 4) {
        parse_skill_slot_blob((const unsigned char*)row[3], lengths[3], skill);
    }

    skill->skill_loaded = 1;

    mysql_free_result(result);
    printf("[DB] Loaded skill for charac_no=%u: remain_sp=%u, used_sp=%u, skills=%u\n",
           charac_no, skill->remain_sp, skill->used_sp, skill->skill_count);

    /* Log first few skill IDs for debugging */
    if (skill->skill_count > 0) {
        printf("[DB] Skill IDs:");
        for (int i = 0; i < skill->skill_count && i < 10; i++) {
            printf(" %u", skill->skill_ids[i]);
        }
        if (skill->skill_count > 10) printf(" ...");
        printf("\n");
    }

    return 0;
}

/* Get equipment data from taiwan_cain_2nd.user_items */
static int db_get_equipment(uint32_t charac_no, DBEquipmentInfo* equip) {
    if (!g_db_charac2 || !equip) return -1;

    memset(equip, 0, sizeof(DBEquipmentInfo));
    equip->charac_no = charac_no;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT ui_id, slot, it_id, stat, hidden_option FROM user_items "
             "WHERE charac_no=%u AND slot < %d ORDER BY slot",
             charac_no, MAX_EQUIP_SLOTS);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Equipment query failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) {
        return -1;
    }

    MYSQL_ROW row;
    int count = 0;
    while ((row = mysql_fetch_row(result)) && count < MAX_EQUIP_SLOTS) {
        int slot = row[1] ? atoi(row[1]) : 0;
        if (slot >= 0 && slot < MAX_EQUIP_SLOTS) {
            equip->equipment[slot].ui_id = row[0] ? (uint32_t)strtoul(row[0], NULL, 10) : 0;
            equip->equipment[slot].slot = slot;
            equip->equipment[slot].it_id = row[2] ? (uint32_t)strtoul(row[2], NULL, 10) : 0;
            equip->equipment[slot].stat = row[3] ? (uint8_t)atoi(row[3]) : 0;
            equip->equipment[slot].hidden_option = row[4] ? (uint16_t)atoi(row[4]) : 0;
            count++;
        }
    }

    equip->equip_count = count;
    equip->equip_loaded = 1;

    mysql_free_result(result);
    printf("[DB] Loaded %d equipment items for charac_no=%u\n", count, charac_no);
    return 0;
}

/* Update gold/money in taiwan_cain_2nd.inventory */
static int db_update_gold(uint32_t charac_no, uint32_t new_gold) {
    if (!g_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE inventory SET money=%u WHERE charac_no=%u",
             new_gold, charac_no);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Update gold failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac2);
    printf("[DB] Updated gold for charac_no=%u: new_gold=%u (affected=%d)\n",
           charac_no, new_gold, affected);
    return affected > 0 ? 0 : -1;
}

/* Add gold to character in taiwan_cain_2nd.inventory */
static int db_add_gold(uint32_t charac_no, uint32_t amount) {
    if (!g_db_charac2 || amount == 0) return -1;

    /* Check connection */
    if (mysql_ping(g_db_charac2) != 0) {
        printf("[DB] Connection lost, cannot add gold\n");
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE inventory SET money=money+%u WHERE charac_no=%u",
             amount, charac_no);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Add gold failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac2);
    printf("[DB] Added %u gold for charac_no=%u (affected=%d)\n",
           amount, charac_no, affected);
    return affected > 0 ? 0 : -1;
}

/* Update EXP in taiwan_cain.charac_stat */
static int db_add_exp(uint32_t charac_no, uint32_t amount) {
    if (!g_db_charac || amount == 0) return -1;

    /* Check connection */
    if (mysql_ping(g_db_charac) != 0) {
        printf("[DB] Connection lost, cannot add EXP\n");
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_stat SET exp=exp+%u WHERE charac_no=%u",
             amount, charac_no);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Add EXP failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac);
    printf("[DB] Added %u EXP for charac_no=%u (affected=%d)\n",
           amount, charac_no, affected);
    return affected > 0 ? 0 : -1;
}

/* Add item to inventory in taiwan_cain_2nd.user_items */
static int db_add_item(uint32_t charac_no, uint32_t item_id, int slot, uint8_t stat) {
    if (!g_db_charac2) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO user_items (charac_no, slot, it_id, stat, reg_date, m_time) "
             "VALUES (%u, %d, %u, %u, NOW(), NOW())",
             charac_no, slot, item_id, stat);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Add item failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    uint32_t new_id = (uint32_t)mysql_insert_id(g_db_charac2);
    printf("[DB] Added item for charac_no=%u: item_id=%u, slot=%d, new_ui_id=%u\n",
           charac_no, item_id, slot, new_id);
    return (int)new_id;
}

/* Find next available inventory slot (slot >= MAX_EQUIP_SLOTS for inventory) */
static int db_find_free_slot(uint32_t charac_no, int start_slot) {
    if (!g_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT slot FROM user_items WHERE charac_no=%u AND slot >= %d ORDER BY slot",
             charac_no, start_slot);

    if (mysql_query(g_db_charac2, query)) {
        return start_slot;  /* Default to start_slot on error */
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) {
        return start_slot;
    }

    int expected_slot = start_slot;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        int used_slot = row[0] ? atoi(row[0]) : 0;
        if (used_slot > expected_slot) {
            break;  /* Found a gap */
        }
        expected_slot = used_slot + 1;
    }

    mysql_free_result(result);
    return expected_slot;
}

/* Delete item from inventory by slot */
static int db_delete_item(uint32_t charac_no, int slot) {
    if (!g_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM user_items WHERE charac_no=%u AND slot=%d LIMIT 1",
             charac_no, slot);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Delete item failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac2);
    printf("[DB] Deleted item from charac_no=%u slot=%d (affected=%d)\n",
           charac_no, slot, affected);
    return affected > 0 ? 0 : -1;
}

/* Update item slot (for equip/unequip) */
static int db_update_item_slot(uint32_t charac_no, int old_slot, int new_slot) {
    if (!g_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE user_items SET slot=%d WHERE charac_no=%u AND slot=%d LIMIT 1",
             new_slot, charac_no, old_slot);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Update item slot failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac2);
    printf("[DB] Moved item for charac_no=%u: slot %d -> %d (affected=%d)\n",
           charac_no, old_slot, new_slot, affected);
    return affected > 0 ? 0 : -1;
}

/* Delete item at slot */
static int db_delete_item_at_slot(uint32_t charac_no, int slot) {
    if (!g_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM user_items WHERE charac_no=%u AND slot=%d LIMIT 1",
             charac_no, slot);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Delete item failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac2);
    printf("[DB] Deleted item for charac_no=%u at slot %d (affected=%d)\n",
           charac_no, slot, affected);
    return affected > 0 ? 0 : -1;
}

/* Get item by slot */
static int db_get_item_at_slot(uint32_t charac_no, int slot, uint32_t* item_id) {
    if (!g_db_charac2 || !item_id) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT it_id FROM user_items WHERE charac_no=%u AND slot=%d LIMIT 1",
             charac_no, slot);

    if (mysql_query(g_db_charac2, query)) {
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) return -1;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row && row[0]) {
        *item_id = (uint32_t)atoi(row[0]);
        mysql_free_result(result);
        return 0;
    }

    mysql_free_result(result);
    return -1;
}

/* Get item info at slot including durability */
static int db_get_item_info_at_slot(uint32_t charac_no, int slot, uint32_t* item_id, uint8_t* durability) {
    if (!g_db_charac2 || !item_id) return -1;
    char query[256];
    snprintf(query, sizeof(query), "SELECT it_id, stat FROM user_items WHERE charac_no=%u AND slot=%d LIMIT 1", charac_no, slot);
    if (mysql_query(g_db_charac2, query)) return -1;
    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) return -1;
    MYSQL_ROW row = mysql_fetch_row(result);
    if (row && row[0]) {
        *item_id = (uint32_t)atoi(row[0]);
        if (durability && row[1]) *durability = (uint8_t)atoi(row[1]);
        mysql_free_result(result);
        return 0;
    }
    mysql_free_result(result);
    return -1;
}

/* Repair single item */
static int db_repair_item(uint32_t charac_no, int slot) {
    if (!g_db_charac2) return -1;
    char query[256];
    snprintf(query, sizeof(query), "UPDATE user_items SET stat=100 WHERE charac_no=%u AND slot=%d LIMIT 1", charac_no, slot);
    if (mysql_query(g_db_charac2, query)) return -1;
    return (int)mysql_affected_rows(g_db_charac2) > 0 ? 0 : -1;
}

/* Repair all equipped items */
static int db_repair_all_items(uint32_t charac_no) {
    if (!g_db_charac2) return -1;
    char query[256];
    snprintf(query, sizeof(query), "UPDATE user_items SET stat=100 WHERE charac_no=%u AND slot BETWEEN 0 AND 12", charac_no);
    if (mysql_query(g_db_charac2, query)) return -1;
    return (int)mysql_affected_rows(g_db_charac2);
}

/* Calculate repair cost */
static uint32_t calc_repair_cost(uint32_t item_id, uint8_t cur_dur) {
    uint32_t lvl = (item_id / 1000) % 100;
    if (lvl == 0) lvl = 1;
    return (100 - cur_dur) * lvl * 10;
}

/* Transfer item from one character to another by ui_id */
static int db_transfer_item(uint32_t ui_id, uint32_t from_charac, uint32_t to_charac) {
    if (!g_db_charac2) return -1;

    /* First verify the item belongs to from_charac */
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT it_id, slot FROM user_items WHERE ui_id=%u AND charac_no=%u",
             ui_id, from_charac);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Transfer item query failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) return -1;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        printf("[DB] Transfer failed: item ui_id=%u not owned by charac_no=%u\n",
               ui_id, from_charac);
        mysql_free_result(result);
        return -1;
    }

    uint32_t it_id = row[0] ? (uint32_t)atoi(row[0]) : 0;
    mysql_free_result(result);

    /* Find a free slot for the receiving character (bag slots start at 11) */
    int new_slot = db_find_free_slot(to_charac, 11);
    if (new_slot < 0 || new_slot > 200) {
        printf("[DB] Transfer failed: no free slot for charac_no=%u\n", to_charac);
        return -1;
    }

    /* Transfer the item: update charac_no and slot */
    snprintf(query, sizeof(query),
             "UPDATE user_items SET charac_no=%u, slot=%d, m_time=NOW() "
             "WHERE ui_id=%u AND charac_no=%u",
             to_charac, new_slot, ui_id, from_charac);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Transfer item update failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac2);
    if (affected > 0) {
        printf("[DB] Transferred item ui_id=%u (it_id=%u) from charac=%u to charac=%u slot=%d\n",
               ui_id, it_id, from_charac, to_charac, new_slot);
        return new_slot;  /* Return new slot on success */
    }

    return -1;
}

/* Update character exp and level in charac_info */
static int db_update_character_exp(uint32_t charac_no, uint32_t exp, uint8_t level) {
    if (!g_db_charac) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_info SET exp=%u, lev=%u WHERE charac_no=%u",
             exp, level, charac_no);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Update exp failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac);
    printf("[DB] Updated charac_no=%u: exp=%u, level=%u (affected=%d)\n",
           charac_no, exp, level, affected);
    return affected > 0 ? 0 : -1;
}

/* Update character HP/MP in charac_info */
static int db_update_character_hp_mp(uint32_t charac_no, uint16_t hp, uint16_t mp) {
    if (!g_db_charac) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_info SET HP=%u, maxHP=%u, maxMP=%u WHERE charac_no=%u",
             hp, hp, mp, charac_no);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Update HP/MP failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    return (int)mysql_affected_rows(g_db_charac) > 0 ? 0 : -1;
}

/* Update skill points in skill table */
static int db_update_skill_sp(uint32_t charac_no, uint32_t remain_sp, uint32_t used_sp) {
    if (!g_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE skill SET remain_sp=%u, used_sp=%u WHERE charac_no=%u",
             remain_sp, used_sp, charac_no);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Update skill SP failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac2);
    printf("[DB] Updated skill for charac_no=%u: remain_sp=%u, used_sp=%u (affected=%d)\n",
           charac_no, remain_sp, used_sp, affected);
    return affected > 0 ? 0 : -1;
}

/* Calculate combat stats based on level, job, and equipment */
static void calculate_combat_stats(ClientSession* s) {
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

/* Skill info for damage calculation */
typedef struct {
    uint16_t skill_id;
    uint8_t  job;               /* Required job (0xFF = any) */
    uint8_t  type;              /* SkillDamageType */
    uint16_t base_damage;       /* Base damage at level 1 */
    uint16_t damage_per_level;  /* Additional damage per skill level */
    uint16_t attack_percent;    /* % of attack stat (100 = 100%) */
    uint16_t hit_count;         /* Number of hits */
    uint16_t cooldown_ms;       /* Cooldown in milliseconds */
    uint16_t mp_cost;           /* MP cost */
} SkillDamageInfo;

/* Skill damage table - based on common DNF skill IDs */
static const SkillDamageInfo g_skill_damage_table[] = {
    /* Slayer skills (job=0) */
    {260, 0, SKILL_TYPE_PHYSICAL, 150, 30, 120, 1, 1000, 10},    /* Basic attack */
    {261, 0, SKILL_TYPE_PHYSICAL, 300, 50, 150, 3, 2000, 25},    /* Triple slash */
    {262, 0, SKILL_TYPE_PHYSICAL, 500, 80, 200, 1, 3000, 40},    /* Heavy strike */
    {263, 0, SKILL_TYPE_PHYSICAL, 800, 120, 250, 5, 5000, 60},   /* Blade storm */
    {264, 0, SKILL_TYPE_PHYSICAL, 1200, 180, 300, 1, 8000, 100}, /* Ultimate slash */

    /* Fighter skills (job=1) */
    {300, 1, SKILL_TYPE_PHYSICAL, 180, 35, 130, 2, 1200, 12},    /* Double punch */
    {301, 1, SKILL_TYPE_PHYSICAL, 350, 55, 160, 4, 2200, 30},    /* Rapid strikes */
    {302, 1, SKILL_TYPE_PHYSICAL, 550, 85, 210, 1, 3200, 45},    /* Rising uppercut */
    {303, 1, SKILL_TYPE_PHYSICAL, 900, 130, 280, 8, 5500, 70},   /* Hundred fists */
    {304, 1, SKILL_TYPE_PHYSICAL, 1400, 200, 350, 1, 9000, 120}, /* Dragon kick */

    /* Gunner skills (job=2) - hybrid damage */
    {400, 2, SKILL_TYPE_HYBRID, 160, 32, 125, 1, 800, 8},        /* Quick shot */
    {401, 2, SKILL_TYPE_HYBRID, 280, 48, 145, 5, 1800, 22},      /* Rapid fire */
    {402, 2, SKILL_TYPE_PHYSICAL, 450, 75, 190, 1, 2800, 38},    /* Grenade */
    {403, 2, SKILL_TYPE_MAGICAL, 700, 110, 240, 1, 4500, 55},    /* Laser cannon */
    {404, 2, SKILL_TYPE_HYBRID, 1100, 160, 290, 10, 7000, 90},   /* Satellite beam */

    /* Mage skills (job=3) */
    {500, 3, SKILL_TYPE_MAGICAL, 200, 40, 140, 1, 1500, 15},     /* Magic missile */
    {501, 3, SKILL_TYPE_MAGICAL, 400, 65, 180, 1, 2500, 35},     /* Fireball */
    {502, 3, SKILL_TYPE_MAGICAL, 650, 100, 230, 3, 4000, 55},    /* Ice storm */
    {503, 3, SKILL_TYPE_MAGICAL, 1000, 150, 300, 1, 6000, 80},   /* Meteor */
    {504, 3, SKILL_TYPE_MAGICAL, 1600, 220, 400, 1, 10000, 150}, /* Apocalypse */

    /* Priest skills (job=4) - mix of healing and damage */
    {600, 4, SKILL_TYPE_MAGICAL, 120, 25, 100, 1, 1000, 10},     /* Holy strike */
    {601, 4, SKILL_TYPE_MAGICAL, 250, 45, 140, 1, 2000, 25},     /* Divine smite */
    {602, 4, SKILL_TYPE_MAGICAL, 450, 70, 180, 1, 3500, 45},     /* Holy explosion */
    {603, 4, SKILL_TYPE_MAGICAL, 750, 115, 250, 1, 5500, 70},    /* Judgment */
    {604, 4, SKILL_TYPE_MAGICAL, 1300, 190, 350, 1, 8500, 110},  /* Divine wrath */

    /* Thief skills (job=5) */
    {700, 5, SKILL_TYPE_PHYSICAL, 170, 33, 135, 2, 900, 9},      /* Backstab */
    {701, 5, SKILL_TYPE_PHYSICAL, 320, 52, 165, 3, 1900, 28},    /* Shadow strike */
    {702, 5, SKILL_TYPE_PHYSICAL, 500, 82, 215, 5, 3000, 42},    /* Blade dance */
    {703, 5, SKILL_TYPE_PHYSICAL, 850, 125, 275, 1, 5200, 65},   /* Assassinate */
    {704, 5, SKILL_TYPE_PHYSICAL, 1350, 195, 340, 1, 8000, 105}, /* Shadow execution */

    /* Common skills (any job) */
    {100, 0xFF, SKILL_TYPE_PHYSICAL, 100, 20, 100, 1, 500, 0},   /* Normal attack */
    {101, 0xFF, SKILL_TYPE_PHYSICAL, 50, 10, 50, 1, 0, 0},       /* Basic hit */

    /* Awakening/Ultimate skills - high damage, long cooldown */
    {1000, 0, SKILL_TYPE_PHYSICAL, 3000, 400, 500, 10, 60000, 200}, /* Slayer awakening */
    {1001, 1, SKILL_TYPE_PHYSICAL, 3200, 420, 520, 15, 60000, 220}, /* Fighter awakening */
    {1002, 2, SKILL_TYPE_HYBRID, 2800, 380, 480, 20, 60000, 180},   /* Gunner awakening */
    {1003, 3, SKILL_TYPE_MAGICAL, 3500, 450, 550, 1, 60000, 250},   /* Mage awakening */
    {1004, 4, SKILL_TYPE_MAGICAL, 2500, 350, 450, 1, 60000, 150},   /* Priest awakening */
    {1005, 5, SKILL_TYPE_PHYSICAL, 3100, 410, 510, 1, 60000, 210},  /* Thief awakening */

    /* End marker */
    {0, 0, 0, 0, 0, 0, 0, 0, 0}
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

/* Calculate skill damage
 * @param stats      - Character combat stats
 * @param skill_id   - Skill being used
 * @param skill_level - Level of the skill (1-30)
 * @param job        - Character job
 * @param monster_def - Monster's defense (physical or magical based on skill type)
 * @param is_crit    - Output: whether this was a critical hit
 * @return           - Final damage value
 */
static uint32_t calculate_skill_damage(CombatStats* stats, uint16_t skill_id,
                                       uint8_t skill_level, uint8_t job,
                                       uint32_t monster_def, uint8_t* is_crit) {
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

    /* Get attack power based on skill type */
    uint32_t attack_power;
    uint32_t defense;
    switch (skill->type) {
        case SKILL_TYPE_MAGICAL:
            attack_power = stats->mag_attack;
            defense = monster_def;  /* Use mag_def for magical */
            break;
        case SKILL_TYPE_HYBRID:
            attack_power = (stats->phys_attack > stats->mag_attack) ?
                           stats->phys_attack : stats->mag_attack;
            defense = monster_def / 2;  /* Hybrid bypasses half defense */
            break;
        case SKILL_TYPE_FIXED:
            attack_power = 0;  /* Fixed damage ignores attack */
            defense = 0;       /* And defense */
            break;
        case SKILL_TYPE_PHYSICAL:
        default:
            attack_power = stats->phys_attack;
            defense = monster_def;
            break;
    }

    /* Calculate base skill damage */
    uint32_t skill_base = skill->base_damage + (skill->damage_per_level * (skill_level - 1));

    /* Apply attack power percentage */
    uint32_t attack_bonus = (attack_power * skill->attack_percent) / 100;

    /* Total damage before defense */
    uint32_t total_damage = skill_base + attack_bonus;

    /* Apply hit count multiplier (for multi-hit skills) */
    /* Individual hits are shown, so we don't multiply here */
    /* This would be called per hit in real implementation */

    /* Apply defense reduction */
    uint32_t damage_after_def;
    if (total_damage > defense) {
        damage_after_def = total_damage - defense;
    } else {
        damage_after_def = total_damage / 10;  /* Minimum 10% damage */
        if (damage_after_def < 1) damage_after_def = 1;
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

/* Get skill MP cost */
static uint16_t get_skill_mp_cost(uint16_t skill_id, uint8_t skill_level) {
    const SkillDamageInfo* skill = find_skill_info(skill_id);
    if (!skill) return 0;

    /* MP cost scales with skill level */
    return skill->mp_cost + (skill->mp_cost * (skill_level - 1) / 10);
}

/* Get skill cooldown */
static uint16_t get_skill_cooldown(uint16_t skill_id) {
    const SkillDamageInfo* skill = find_skill_info(skill_id);
    if (!skill) return 1000;  /* Default 1 second */
    return skill->cooldown_ms;
}

/* Get skill hit count */
static uint8_t get_skill_hit_count(uint16_t skill_id) {
    const SkillDamageInfo* skill = find_skill_info(skill_id);
    if (!skill) return 1;
    return (uint8_t)skill->hit_count;
}

/* Get account by UID */
static int db_get_account_by_uid(uint32_t uid, DBAccountInfo* account) {
    if (!g_db_account || !account) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT UID, accountname, password, VIP FROM accounts WHERE UID=%u",
             uid);

    if (mysql_query(g_db_account, query)) {
        printf("[DB] Query failed: %s\n", mysql_error(g_db_account));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_account);
    if (!result) {
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        return -1;
    }

    memset(account, 0, sizeof(DBAccountInfo));
    account->uid = atoi(row[0]);
    if (row[1]) strncpy(account->accountname, row[1], sizeof(account->accountname) - 1);
    if (row[2]) strncpy(account->password, row[2], sizeof(account->password) - 1);
    if (row[3]) strncpy(account->vip, row[3], sizeof(account->vip) - 1);

    mysql_free_result(result);
    return 0;
}

/* Get characters by account UID (m_id) */
static int db_get_characters(uint32_t m_id, DBCharacterInfo* chars, int max_count) {
    if (!g_db_charac || !chars || max_count <= 0) return -1;

    char query[1024];
    snprintf(query, sizeof(query),
             "SELECT charac_no, charac_name, job, grow_type, lev, exp, HP, maxHP, maxMP, "
             "village, fatigue, sex, delete_flag, guild_id, dungeon_clear_point, "
             "UNIX_TIMESTAMP(create_time), expert_job, skill_tree_index, "
             "phy_attack, phy_defense, mag_attack, mag_defense, "
             "move_speed, attack_speed, cast_speed, hit_recovery, jump, "
             "charac_weight, max_fatigue, guild_right, member_flag "
             "FROM charac_info WHERE m_id=%u AND delete_flag=0 ORDER BY charac_no LIMIT %d",
             m_id, max_count);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Query failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) {
        return -1;
    }

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) && count < max_count) {
        memset(&chars[count], 0, sizeof(DBCharacterInfo));

        /* 基础信息 */
        chars[count].charac_no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(chars[count].charac_name, row[1], sizeof(chars[count].charac_name) - 1);
        chars[count].job = row[2] ? atoi(row[2]) : 0;
        chars[count].grow_type = row[3] ? atoi(row[3]) : 0;
        chars[count].level = row[4] ? atoi(row[4]) : 1;
        chars[count].exp = row[5] ? atoi(row[5]) : 0;
        chars[count].hp = row[6] ? atoi(row[6]) : 1000;
        chars[count].max_hp = row[7] ? atoi(row[7]) : 1000;
        chars[count].max_mp = row[8] ? atoi(row[8]) : 500;
        chars[count].village = row[9] ? atoi(row[9]) : 1;
        chars[count].fatigue = row[10] ? atoi(row[10]) : 156;
        chars[count].sex = row[11] ? atoi(row[11]) : 1;
        chars[count].deleted = row[12] ? atoi(row[12]) : 0;
        chars[count].guild_id = row[13] ? atoi(row[13]) : 0;
        chars[count].dungeon_clear_point = row[14] ? atoi(row[14]) : 0;
        chars[count].create_time = row[15] ? atoi(row[15]) : 0;

        /* 扩展字段 - 职业/技能 */
        chars[count].expert_job = row[16] ? atoi(row[16]) : 0;
        chars[count].skill_tree_index = row[17] ? atoi(row[17]) : -1;

        /* 战斗属性 */
        chars[count].phy_attack = row[18] ? atoi(row[18]) : 100;
        chars[count].phy_defense = row[19] ? atoi(row[19]) : 50;
        chars[count].mag_attack = row[20] ? atoi(row[20]) : 100;
        chars[count].mag_defense = row[21] ? atoi(row[21]) : 50;
        chars[count].move_speed = row[22] ? atoi(row[22]) : 100;
        chars[count].attack_speed = row[23] ? atoi(row[23]) : 100;
        chars[count].cast_speed = row[24] ? atoi(row[24]) : 100;
        chars[count].hit_recovery = row[25] ? atoi(row[25]) : 0;
        chars[count].jump = row[26] ? atoi(row[26]) : 100;
        chars[count].charac_weight = row[27] ? atoi(row[27]) : 0;
        chars[count].max_fatigue = row[28] ? atoi(row[28]) : 156;

        /* 公会信息 */
        chars[count].guild_right = row[29] ? atoi(row[29]) : 0;
        chars[count].member_flag = row[30] ? atoi(row[30]) : 0;

        /* 默认值处理 */
        if (chars[count].max_hp == 0) {
            chars[count].max_hp = 35200;  /* Default max HP for Lv85 */
        }
        if (chars[count].hp == 0) {
            chars[count].hp = chars[count].max_hp;
        }
        if (chars[count].fatigue <= 0) {
            chars[count].fatigue = 156;  /* Default fatigue */
        }
        if (chars[count].max_fatigue <= 0) {
            chars[count].max_fatigue = 156;
        }

        count++;
    }

    mysql_free_result(result);

    /* 加载每个角色的装备外观数据 */
    for (int i = 0; i < count; i++) {
        if (chars[i].charac_no == 0) continue;

        /* 初始化装备数组 */
        memset(chars[i].equip_item_ids, 0, sizeof(chars[i].equip_item_ids));

        /* 查询装备槽 (slot 0-10) */
        char equip_query[256];
        snprintf(equip_query, sizeof(equip_query),
                 "SELECT slot, it_id FROM user_items WHERE charac_no=%u AND slot < %d",
                 chars[i].charac_no, MAX_EQUIP_SLOTS);

        if (mysql_query(g_db_charac2, equip_query) == 0) {
            MYSQL_RES* eq_result = mysql_store_result(g_db_charac2);
            if (eq_result) {
                MYSQL_ROW eq_row;
                while ((eq_row = mysql_fetch_row(eq_result))) {
                    int slot = eq_row[0] ? atoi(eq_row[0]) : -1;
                    uint32_t it_id = eq_row[1] ? (uint32_t)strtoul(eq_row[1], NULL, 10) : 0;
                    if (slot >= 0 && slot < MAX_EQUIP_SLOTS) {
                        chars[i].equip_item_ids[slot] = it_id;
                    }
                }
                mysql_free_result(eq_result);
            }
        }
    }

    printf("[DB] Loaded %d characters for m_id=%u (with equipment)\n", count, m_id);
    return count;
}

/* Get single character by charac_no */
static int db_get_character(uint32_t charac_no, DBCharacterInfo* ch) {
    if (!g_db_charac || !ch) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT charac_no, charac_name, job, grow_type, lev, exp, HP, maxHP, maxMP, "
             "village, fatigue, sex, delete_flag "
             "FROM charac_info WHERE charac_no=%u",
             charac_no);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Query failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) {
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        return -1;
    }

    memset(ch, 0, sizeof(DBCharacterInfo));
    ch->charac_no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(ch->charac_name, row[1], sizeof(ch->charac_name) - 1);
    ch->job = row[2] ? atoi(row[2]) : 0;
    ch->grow_type = row[3] ? atoi(row[3]) : 0;
    ch->level = row[4] ? atoi(row[4]) : 1;
    ch->exp = row[5] ? atoi(row[5]) : 0;
    ch->hp = row[6] ? atoi(row[6]) : 1000;
    ch->max_hp = row[7] ? atoi(row[7]) : 1000;
    ch->max_mp = row[8] ? atoi(row[8]) : 500;
    ch->village = row[9] ? atoi(row[9]) : 1;
    ch->fatigue = row[10] ? atoi(row[10]) : 156;
    ch->sex = row[11] ? atoi(row[11]) : 1;
    ch->deleted = row[12] ? atoi(row[12]) : 0;

    if (ch->max_hp == 0) ch->max_hp = 35200;
    if (ch->hp == 0) ch->hp = ch->max_hp;

    mysql_free_result(result);
    return 0;
}

/* Mail structure for database */
typedef struct {
    uint32_t postal_id;
    uint32_t send_charac_no;
    char     send_name[20];
    uint32_t item_id;
    uint32_t gold;
    uint32_t occ_time;
    uint8_t  read_flag;
} DBMailInfo;

/* Get mail list from taiwan_cain_2nd.postal */
static int db_get_mail_list(uint32_t charac_no, DBMailInfo* mails, int max_count) {
    if (!g_db_charac2 || !mails) return 0;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT postal_id, send_charac_no, send_charac_name, item_id, gold, "
             "UNIX_TIMESTAMP(occ_time), delete_flag "
             "FROM postal WHERE receive_charac_no=%u AND delete_flag=0 "
             "ORDER BY occ_time DESC LIMIT %d",
             charac_no, max_count);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Mail query failed: %s\n", mysql_error(g_db_charac2));
        return 0;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) return 0;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) && count < max_count) {
        DBMailInfo* m = &mails[count];
        memset(m, 0, sizeof(DBMailInfo));
        m->postal_id = row[0] ? atoi(row[0]) : 0;
        m->send_charac_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(m->send_name, row[2], sizeof(m->send_name) - 1);
        m->item_id = row[3] ? atoi(row[3]) : 0;
        m->gold = row[4] ? atoi(row[4]) : 0;
        m->occ_time = row[5] ? atoi(row[5]) : 0;
        m->read_flag = 0;
        count++;
    }

    mysql_free_result(result);
    printf("[DB] Loaded %d mails for charac_no=%u\n", count, charac_no);
    return count;
}

/* Get unread mail count */
static int db_get_unread_mail_count(uint32_t charac_no) {
    if (!g_db_charac2) return 0;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM postal WHERE receive_charac_no=%u AND delete_flag=0",
             charac_no);

    if (mysql_query(g_db_charac2, query)) {
        return 0;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) return 0;

    MYSQL_ROW row = mysql_fetch_row(result);
    int count = row && row[0] ? atoi(row[0]) : 0;
    mysql_free_result(result);
    return count;
}

/* Mail attachment structure */
typedef struct {
    uint32_t postal_id;
    uint32_t item_id;
    uint32_t gold;
    uint8_t  claimed;
} DBMailAttachment;

/* Get mail attachment info */
static int db_get_mail_attachment(uint32_t postal_id, uint32_t charac_no, DBMailAttachment* att) {
    if (!g_db_charac2 || !att) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT postal_id, item_id, gold, delete_flag FROM postal "
             "WHERE postal_id=%u AND receive_charac_no=%u",
             postal_id, charac_no);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Get mail attachment failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_db_charac2);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        return -1;
    }

    att->postal_id = row[0] ? atoi(row[0]) : 0;
    att->item_id = row[1] ? atoi(row[1]) : 0;
    att->gold = row[2] ? strtoul(row[2], NULL, 10) : 0;
    att->claimed = row[3] ? atoi(row[3]) : 0;

    mysql_free_result(res);
    return 0;
}

/* Mark mail as claimed (set delete_flag=1) */
static int db_claim_mail_attachment(uint32_t postal_id) {
    if (!g_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE postal SET delete_flag=1 WHERE postal_id=%u",
             postal_id);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Claim mail attachment failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    printf("[DB] Mail %u marked as claimed\n", postal_id);
    return 0;
}

/* Get single mail by postal_id */
static int db_get_mail_by_id(uint32_t postal_id, uint32_t charac_no, DBMailInfo* mail) {
    if (!g_db_charac2 || !mail) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT postal_id, send_charac_no, send_charac_name, item_id, gold, "
             "UNIX_TIMESTAMP(occ_time) "
             "FROM postal WHERE postal_id=%u AND receive_charac_no=%u AND delete_flag=0",
             postal_id, charac_no);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Get mail by id failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(g_db_charac2);
    if (!res) return -1;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        return -1;
    }

    memset(mail, 0, sizeof(DBMailInfo));
    mail->postal_id = row[0] ? atoi(row[0]) : 0;
    mail->send_charac_no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(mail->send_name, row[2], sizeof(mail->send_name) - 1);
    mail->item_id = row[3] ? atoi(row[3]) : 0;
    mail->gold = row[4] ? strtoul(row[4], NULL, 10) : 0;
    mail->occ_time = row[5] ? atoi(row[5]) : 0;
    mail->read_flag = 0;

    mysql_free_result(res);
    printf("[DB] Got mail %u: from=%s gold=%u item=%u\n",
           postal_id, mail->send_name, mail->gold, mail->item_id);
    return 0;
}

/* Delete mail (soft delete - set delete_flag=1) */
static int db_delete_mail(uint32_t postal_id, uint32_t charac_no) {
    if (!g_db_charac2) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE postal SET delete_flag=1 WHERE postal_id=%u AND receive_charac_no=%u",
             postal_id, charac_no);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Delete mail failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac2);
    if (affected > 0) {
        printf("[DB] Mail %u deleted for charac_no=%u\n", postal_id, charac_no);
        return 0;
    }
    return -1;
}

/* Send mail - insert into postal table */
static int db_send_mail(uint32_t send_charac_no, const char* send_name,
                        uint32_t receive_charac_no, uint32_t item_id, uint32_t gold) {
    if (!g_db_charac2) return -1;

    char safe_name[41];
    memset(safe_name, 0, sizeof(safe_name));
    strncpy(safe_name, send_name, 20);
    /* Escape special characters */
    for (int i = 0; i < 40 && safe_name[i]; i++) {
        if (safe_name[i] == '\'' || safe_name[i] == '\\') safe_name[i] = '_';
    }

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO postal (occ_time, send_charac_no, send_charac_name, "
             "receive_charac_no, item_id, gold, delete_flag) "
             "VALUES (NOW(), %u, '%s', %u, %u, %u, 0)",
             send_charac_no, safe_name, receive_charac_no, item_id, gold);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Send mail failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    uint32_t postal_id = (uint32_t)mysql_insert_id(g_db_charac2);
    printf("[DB] Mail sent: from=%u to=%u gold=%u item=%u postal_id=%u\n",
           send_charac_no, receive_charac_no, gold, item_id, postal_id);
    return (int)postal_id;
}

/* Get character no by name */
static uint32_t db_get_charac_no_by_name(const char* name) {
    if (!g_db_charac || !name) return 0;

    char safe_name[41];
    memset(safe_name, 0, sizeof(safe_name));
    strncpy(safe_name, name, 20);
    for (int i = 0; i < 40 && safe_name[i]; i++) {
        if (safe_name[i] == '\'' || safe_name[i] == '\\') safe_name[i] = '_';
    }

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT charac_no FROM charac_info WHERE charac_name='%s' AND delete_flag=0",
             safe_name);

    if (mysql_query(g_db_charac, query)) return 0;

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return 0;

    MYSQL_ROW row = mysql_fetch_row(result);
    uint32_t charac_no = row && row[0] ? atoi(row[0]) : 0;
    mysql_free_result(result);
    return charac_no;
}

/* Add friend to database */
static int db_add_friend(uint32_t charac_no, uint32_t friend_no) {
    if (!g_db_charac) return -1;

    char query[256];
    /* Insert both directions for bidirectional friendship */
    snprintf(query, sizeof(query),
             "INSERT IGNORE INTO charac_friends (charac_no, friend_no) VALUES (%u, %u)",
             charac_no, friend_no);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Add friend failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac);
    printf("[DB] Added friend: %u -> %u (affected=%d)\n", charac_no, friend_no, affected);
    return affected > 0 ? 0 : -1;
}

/* Remove friend from database */
static int db_remove_friend(uint32_t charac_no, uint32_t friend_no) {
    if (!g_db_charac) return -1;

    char query[256];
    /* Delete both directions */
    snprintf(query, sizeof(query),
             "DELETE FROM charac_friends WHERE "
             "(charac_no=%u AND friend_no=%u) OR (charac_no=%u AND friend_no=%u)",
             charac_no, friend_no, friend_no, charac_no);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Remove friend failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac);
    printf("[DB] Removed friend: %u <-> %u (affected=%d)\n", charac_no, friend_no, affected);
    return affected > 0 ? 0 : -1;
}

/* Ranking entry structure */
typedef struct {
    uint32_t charac_no;
    char     charac_name[20];
    uint8_t  lev;
    uint8_t  job;
    uint32_t money;
} DBRankingEntry;

/* Get level ranking from charac_info - ORDER BY lev DESC */
static int db_get_level_ranking(DBRankingEntry* entries, int max_count) {
    if (!g_db_charac || !entries) return 0;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT charac_no, charac_name, lev, job "
             "FROM charac_info WHERE delete_flag=0 "
             "ORDER BY lev DESC, exp DESC LIMIT %d", max_count);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Level ranking query failed: %s\n", mysql_error(g_db_charac));
        return 0;
    }

    MYSQL_RES* res = mysql_store_result(g_db_charac);
    if (!res) return 0;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) && count < max_count) {
        entries[count].charac_no = row[0] ? atoi(row[0]) : 0;
        memset(entries[count].charac_name, 0, 20);
        if (row[1]) strncpy(entries[count].charac_name, row[1], 19);
        entries[count].lev = row[2] ? atoi(row[2]) : 1;
        entries[count].job = row[3] ? atoi(row[3]) : 0;
        entries[count].money = 0;
        count++;
    }
    mysql_free_result(res);

    printf("[DB] Level ranking: found %d entries\n", count);
    return count;
}

/* Get wealth ranking - JOIN charac_info with inventory for money */
static int db_get_wealth_ranking(DBRankingEntry* entries, int max_count) {
    if (!g_db_charac2 || !entries) return 0;

    /* Query inventory table for money, then get character info */
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT i.charac_no, c.charac_name, c.lev, c.job, i.money "
             "FROM inventory i "
             "JOIN taiwan_cain.charac_info c ON i.charac_no = c.charac_no "
             "WHERE c.delete_flag=0 "
             "ORDER BY i.money DESC LIMIT %d", max_count);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Wealth ranking query failed: %s\n", mysql_error(g_db_charac2));
        return 0;
    }

    MYSQL_RES* res = mysql_store_result(g_db_charac2);
    if (!res) return 0;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) && count < max_count) {
        entries[count].charac_no = row[0] ? atoi(row[0]) : 0;
        memset(entries[count].charac_name, 0, 20);
        if (row[1]) strncpy(entries[count].charac_name, row[1], 19);
        entries[count].lev = row[2] ? atoi(row[2]) : 1;
        entries[count].job = row[3] ? atoi(row[3]) : 0;
        entries[count].money = row[4] ? strtoul(row[4], NULL, 10) : 0;
        count++;
    }
    mysql_free_result(res);

    printf("[DB] Wealth ranking: found %d entries\n", count);
    return count;
}

/* Create guild in d_guild.guild_info */
static int db_create_guild(const char* guild_name, uint32_t master_charac_no, const char* master_name) {
    if (!g_db_guild) {
        printf("[DB] Guild database not connected\n");
        return -1;
    }

    /* Escape guild name */
    char safe_name[41];
    memset(safe_name, 0, sizeof(safe_name));
    strncpy(safe_name, guild_name, 20);
    for (int i = 0; i < 40 && safe_name[i]; i++) {
        if (safe_name[i] == '\'' || safe_name[i] == '\\') safe_name[i] = '_';
    }

    char safe_master[41];
    memset(safe_master, 0, sizeof(safe_master));
    strncpy(safe_master, master_name, 20);
    for (int i = 0; i < 40 && safe_master[i]; i++) {
        if (safe_master[i] == '\'' || safe_master[i] == '\\') safe_master[i] = '_';
    }

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO guild_info (guild_name, master_id, master_name, member_cnt, guild_lev, "
             "occ_time, guild_notice) "
             "VALUES ('%s', %u, '%s', 1, 1, NOW(), '')",
             safe_name, master_charac_no, safe_master);

    if (mysql_query(g_db_guild, query)) {
        printf("[DB] Create guild failed: %s\n", mysql_error(g_db_guild));
        return -1;
    }

    int guild_id = (int)mysql_insert_id(g_db_guild);
    printf("[DB] Guild created: id=%d name=%s master=%s\n", guild_id, safe_name, safe_master);
    return guild_id;
}

/* Add member to guild in d_guild.guild_member */
static int db_add_guild_member(uint32_t guild_id, uint32_t charac_no, const char* charac_name, uint8_t grade) {
    if (!g_db_guild) return -1;

    char safe_name[41];
    memset(safe_name, 0, sizeof(safe_name));
    strncpy(safe_name, charac_name, 20);
    for (int i = 0; i < 40 && safe_name[i]; i++) {
        if (safe_name[i] == '\'' || safe_name[i] == '\\') safe_name[i] = '_';
    }

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO guild_member (guild_id, charac_no, charac_name, grade, join_time) "
             "VALUES (%u, %u, '%s', %u, NOW())",
             guild_id, charac_no, safe_name, grade);

    if (mysql_query(g_db_guild, query)) {
        printf("[DB] Add guild member failed: %s\n", mysql_error(g_db_guild));
        return -1;
    }

    printf("[DB] Added guild member: guild=%u charac=%u name=%s grade=%u\n",
           guild_id, charac_no, safe_name, grade);
    return 0;
}

/* Update guild member count */
static int db_update_guild_member_count(uint32_t guild_id, int delta) {
    if (!g_db_guild) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE guild_info SET member_cnt = member_cnt + %d WHERE guild_id = %u",
             delta, guild_id);

    if (mysql_query(g_db_guild, query)) {
        printf("[DB] Update guild member count failed: %s\n", mysql_error(g_db_guild));
        return -1;
    }
    return 0;
}

/* Update character's guild_id in charac_info */
static int db_update_character_guild(uint32_t charac_no, uint32_t guild_id) {
    if (!g_db_charac) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE charac_info SET guild_id = %u WHERE charac_no = %u",
             guild_id, charac_no);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Update character guild failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }
    return 0;
}

/* Remove member from guild (set member_flag=0) */
static int db_remove_guild_member(uint32_t guild_id, uint32_t charac_no) {
    if (!g_db_guild) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "UPDATE guild_member SET member_flag=0 WHERE guild_id=%u AND charac_no=%u",
             guild_id, charac_no);

    if (mysql_query(g_db_guild, query)) {
        printf("[DB] Remove guild member failed: %s\n", mysql_error(g_db_guild));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_guild);
    printf("[DB] Removed guild member: guild=%u charac=%u (affected=%d)\n",
           guild_id, charac_no, affected);
    return affected > 0 ? 0 : -1;
}

/* Disband guild (set expire_flag=1) */
static int db_disband_guild(uint32_t guild_id) {
    if (!g_db_guild) return -1;

    char query[256];
    /* Mark guild as expired */
    snprintf(query, sizeof(query),
             "UPDATE guild_info SET expire_flag=1 WHERE guild_id=%u",
             guild_id);

    if (mysql_query(g_db_guild, query)) {
        printf("[DB] Disband guild failed: %s\n", mysql_error(g_db_guild));
        return -1;
    }

    /* Mark all members as removed */
    snprintf(query, sizeof(query),
             "UPDATE guild_member SET member_flag=0 WHERE guild_id=%u",
             guild_id);
    mysql_query(g_db_guild, query);

    printf("[DB] Guild %u disbanded\n", guild_id);
    return 0;
}

/* Friend structure for database */
typedef struct {
    uint32_t friend_charac_no;
    char     friend_name[20];
    uint8_t  online;
} DBFriendInfo;

/* Get friend list from taiwan_cain.charac_friends */
static int db_get_friend_list(uint32_t charac_no, DBFriendInfo* friends, int max_count) {
    if (!g_db_charac || !friends) return 0;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT f.friend_no, c.charac_name "
             "FROM charac_friends f "
             "LEFT JOIN charac_info c ON f.friend_no = c.charac_no "
             "WHERE f.charac_no=%u LIMIT %d",
             charac_no, max_count);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Friend query failed: %s\n", mysql_error(g_db_charac));
        return 0;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return 0;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) && count < max_count) {
        DBFriendInfo* f = &friends[count];
        memset(f, 0, sizeof(DBFriendInfo));
        f->friend_charac_no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(f->friend_name, row[1], sizeof(f->friend_name) - 1);
        f->online = (uint8_t)is_character_online(f->friend_charac_no);
        count++;
    }

    mysql_free_result(result);
    printf("[DB] Loaded %d friends for charac_no=%u\n", count, charac_no);
    return count;
}

/* Soft delete a character (set delete_flag=1) */
static int db_soft_delete_character(uint32_t charac_no, uint32_t m_id) {
    if (!g_db_charac) return -1;

    /* Verify character belongs to this account */
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT charac_no FROM charac_info WHERE charac_no=%u AND m_id=%u AND delete_flag=0",
             charac_no, m_id);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Delete character verify failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return -1;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        printf("[DB] Delete character failed: charac_no=%u not owned by m_id=%u or already deleted\n",
               charac_no, m_id);
        mysql_free_result(result);
        return -1;
    }
    mysql_free_result(result);

    /* Mark as deleted (soft delete) */
    snprintf(query, sizeof(query),
             "UPDATE charac_info SET delete_flag=1, delete_time=NOW() WHERE charac_no=%u AND m_id=%u",
             charac_no, m_id);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Delete character update failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac);
    if (affected > 0) {
        printf("[DB] Soft deleted character charac_no=%u (m_id=%u)\n", charac_no, m_id);
        return 0;
    }

    return -1;
}

/* Optional: Clean up character items (move to deleted_items or just clear) */
static int db_cleanup_deleted_character_items(uint32_t charac_no) {
    if (!g_db_charac2) return -1;

    /* Delete items from user_items for the deleted character */
    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM user_items WHERE charac_no=%u",
             charac_no);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Cleanup items failed: %s\n", mysql_error(g_db_charac2));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac2);
    printf("[DB] Cleaned up %d items for deleted charac_no=%u\n", affected, charac_no);
    return 0;
}

/* Guild structure for database */
typedef struct {
    uint32_t guild_id;
    char     guild_name[40];
    char     master_name[20];
    uint8_t  level;
    uint32_t member_count;
    uint32_t guild_point;
} DBGuildInfo;

/* Guild member structure */
typedef struct {
    uint32_t charac_no;
    char     charac_name[20];
    uint8_t  grade;
    uint8_t  job;
    uint8_t  level;
    uint8_t  online;
} DBGuildMemberInfo;

/* Get guild info by charac_no */
static int db_get_guild_by_charac(uint32_t charac_no, DBGuildInfo* guild) {
    if (!g_db_guild || !guild) return -1;

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT g.guild_id, g.guild_name, g.master_name, g.lev, g.member_count, g.guild_point "
             "FROM guild_member m "
             "JOIN guild_info g ON m.guild_id = g.guild_id "
             "WHERE m.charac_no=%u AND m.member_flag=1 AND g.expire_flag=0",
             charac_no);

    if (mysql_query(g_db_guild, query)) {
        printf("[DB] Guild query failed: %s\n", mysql_error(g_db_guild));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_guild);
    if (!result) return -1;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        return -1;  /* Not in guild */
    }

    memset(guild, 0, sizeof(DBGuildInfo));
    guild->guild_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(guild->guild_name, row[1], sizeof(guild->guild_name) - 1);
    if (row[2]) strncpy(guild->master_name, row[2], sizeof(guild->master_name) - 1);
    guild->level = row[3] ? atoi(row[3]) : 1;
    guild->member_count = row[4] ? atoi(row[4]) : 0;
    guild->guild_point = row[5] ? atoi(row[5]) : 0;

    mysql_free_result(result);
    printf("[DB] Loaded guild %s (id=%u) for charac_no=%u\n",
           guild->guild_name, guild->guild_id, charac_no);
    return 0;
}

/* Get guild members */
static int db_get_guild_members(uint32_t guild_id, DBGuildMemberInfo* members, int max_count) {
    if (!g_db_guild || !members) return 0;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT charac_no, charac_name, grade, job, lev "
             "FROM guild_member WHERE guild_id=%u AND member_flag=1 "
             "ORDER BY grade ASC, lev DESC LIMIT %d",
             guild_id, max_count);

    if (mysql_query(g_db_guild, query)) {
        printf("[DB] Guild member query failed: %s\n", mysql_error(g_db_guild));
        return 0;
    }

    MYSQL_RES* result = mysql_store_result(g_db_guild);
    if (!result) return 0;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) && count < max_count) {
        DBGuildMemberInfo* m = &members[count];
        memset(m, 0, sizeof(DBGuildMemberInfo));
        m->charac_no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(m->charac_name, row[1], sizeof(m->charac_name) - 1);
        m->grade = row[2] ? atoi(row[2]) : 9;
        m->job = row[3] ? atoi(row[3]) : 0;
        m->level = row[4] ? atoi(row[4]) : 1;
        m->online = (uint8_t)is_character_online(m->charac_no);
        count++;
    }

    mysql_free_result(result);
    printf("[DB] Loaded %d guild members for guild_id=%u\n", count, guild_id);
    return count;
}

/* Storage structure */
typedef struct {
    uint32_t money;
    uint8_t  capacity;
    /* cargo blob would need special handling */
} DBStorageInfo;

/* Get account storage from taiwan_cain.account_cargo */
static int db_get_storage(uint32_t m_id, DBStorageInfo* storage) {
    if (!g_db_charac || !storage) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT money, capacity FROM account_cargo WHERE m_id=%u",
             m_id);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Storage query failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac);
    if (!result) return -1;

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        memset(storage, 0, sizeof(DBStorageInfo));
        return 0;  /* No storage record */
    }

    storage->money = row[0] ? atoi(row[0]) : 0;
    storage->capacity = row[1] ? atoi(row[1]) : 8;

    mysql_free_result(result);
    printf("[DB] Loaded storage for m_id=%u: money=%u, capacity=%u\n",
           m_id, storage->money, storage->capacity);
    return 0;
}

/* Create storage record if not exists */
static int db_create_storage(uint32_t m_id) {
    if (!g_db_charac) return -1;

    char query[256];
    snprintf(query, sizeof(query),
             "INSERT IGNORE INTO account_cargo (m_id, money, capacity, cargo, occ_time) "
             "VALUES (%u, 0, 8, '', NOW())",
             m_id);

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Create storage failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    int affected = (int)mysql_affected_rows(g_db_charac);
    if (affected > 0) {
        printf("[DB] Created storage record for m_id=%u\n", m_id);
    }
    return 0;
}

/* Update storage gold (deposit or withdraw) */
static int db_update_storage_gold(uint32_t m_id, int32_t delta) {
    if (!g_db_charac) return -1;

    /* Create if not exists */
    db_create_storage(m_id);

    char query[256];
    if (delta >= 0) {
        snprintf(query, sizeof(query),
                 "UPDATE account_cargo SET money=money+%d WHERE m_id=%u",
                 delta, m_id);
    } else {
        /* For withdrawal, ensure we don't go negative */
        snprintf(query, sizeof(query),
                 "UPDATE account_cargo SET money=GREATEST(0, CAST(money AS SIGNED) + (%d)) WHERE m_id=%u",
                 delta, m_id);
    }

    if (mysql_query(g_db_charac, query)) {
        printf("[DB] Update storage gold failed: %s\n", mysql_error(g_db_charac));
        return -1;
    }

    printf("[DB] Updated storage gold for m_id=%u: delta=%d\n", m_id, delta);
    return 0;
}

/* Get storage gold */
static uint32_t db_get_storage_gold(uint32_t m_id) {
    DBStorageInfo storage;
    if (db_get_storage(m_id, &storage) < 0) {
        return 0;
    }
    return storage.money;
}

/* Pet structure */
#define MAX_PETS 10
typedef struct {
    uint32_t ui_id;
    uint32_t it_id;
    char     name[13];
    uint32_t exp;
    uint8_t  endurance;
    uint8_t  slot;
    uint8_t  creature_type;
    uint8_t  stat;
} DBPetInfo;

/* Get pet list from taiwan_cain_2nd.creature_items */
static int db_get_pet_list(uint32_t charac_no, DBPetInfo* pets, int max_count) {
    if (!g_db_charac2 || !pets) return 0;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT ui_id, it_id, name, exp, endurance, slot, creature_type, stat "
             "FROM creature_items WHERE charac_no=%u AND stat=0 ORDER BY slot LIMIT %d",
             charac_no, max_count);

    if (mysql_query(g_db_charac2, query)) {
        printf("[DB] Pet query failed: %s\n", mysql_error(g_db_charac2));
        return 0;
    }

    MYSQL_RES* result = mysql_store_result(g_db_charac2);
    if (!result) return 0;

    int count = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) && count < max_count) {
        DBPetInfo* p = &pets[count];
        memset(p, 0, sizeof(DBPetInfo));
        p->ui_id = row[0] ? atoi(row[0]) : 0;
        p->it_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(p->name, row[2], sizeof(p->name) - 1);
        p->exp = row[3] ? atoi(row[3]) : 0;
        p->endurance = row[4] ? atoi(row[4]) : 100;
        p->slot = row[5] ? atoi(row[5]) : 0;
        p->creature_type = row[6] ? atoi(row[6]) : 0;
        p->stat = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    mysql_free_result(result);
    printf("[DB] Loaded %d pets for charac_no=%u\n", count, charac_no);
    return count;
}

/*==============================================================================
 * Utility Functions
 *============================================================================*/

/* Set socket non-blocking */
static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/* Find session by fd */
static ClientSession* find_session(int fd) {
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].fd == fd) {
            return &g_sessions[i];
        }
    }
    return NULL;
}

/* Create new session */
static ClientSession* create_session(int fd, uint32_t ip, uint16_t port) {
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (!g_sessions[i].active) {
            ClientSession* s = &g_sessions[i];
            memset(s, 0, sizeof(ClientSession));
            s->fd = fd;
            s->ip_addr = ip;
            s->port = port;
            s->state = STATE_CONNECTED;
            s->connect_time = (uint32_t)time(NULL);
            s->last_activity = s->connect_time;
            s->active = 1;
            g_session_count++;
            return s;
        }
    }
    return NULL;
}

/* Save character data to database on logout */
static void save_character_on_logout(ClientSession* s) {
    if (!s || s->character_id == 0) return;

    printf("[GAME] Saving character %s (id=%u) on logout\n",
           s->character_name, s->character_id);

    /* Save gold */
    db_update_gold(s->character_id, s->inventory.money);

    /* Save HP/MP */
    db_update_character_hp_mp(s->character_id, (uint16_t)s->current_hp, 0);

    printf("[GAME] Character saved: gold=%u hp=%u\n",
           s->inventory.money, s->current_hp);
}

/* Destroy session */
static void destroy_session(ClientSession* s) {
    if (s && s->active) {
        /* Save character data before destroying session */
        save_character_on_logout(s);

        s->active = 0;
        g_session_count--;
        printf("[GAME] Session destroyed for %s (id=%u)\n",
               s->character_name, s->character_id);
    }
}

/*==============================================================================
 * Packet Functions
 *============================================================================*/

/* Send response packet */
static int send_response(ClientSession* s, uint8_t msg_no, const uint8_t* data, int data_len) {
    int total_len = PACKET_HEADER_SIZE + data_len;
    uint8_t* packet = (uint8_t*)malloc(total_len);
    if (!packet) return -1;

    memset(packet, 0, total_len);

    /* Build header */
    packet[0] = 0;  /* classification */
    memcpy(packet + 1, &total_len, 4);  /* length */
    packet[5] = msg_no;  /* msg_no */
    packet[6] = 0;  /* reserved */
    uint32_t checksum = 0;
    memcpy(packet + 7, &checksum, 4);  /* checksum */
    uint16_t seq = ++g_sequence;
    memcpy(packet + 11, &seq, 2);  /* sequence */

    /* Copy data */
    if (data && data_len > 0) {
        memcpy(packet + PACKET_HEADER_SIZE, data, data_len);
    }

    int sent = send(s->fd, packet, total_len, 0);
    free(packet);

    if (sent == total_len) {
        printf("[GAME] Sent msg_no=%d len=%d to fd=%d\n", msg_no, total_len, s->fd);
        return 0;
    }
    return -1;
}

/*==============================================================================
 * Packet Building Helpers
 *============================================================================*/

/* Build USERINFO response (after login)
 * 生产协议格式 - 基于 df_game_r 分析
 * NOTIPACKET_USERINFO = 2
 *
 * Format:
 * - result: uint32 (4 bytes) - 0=成功
 * - char_count: uint32 (4 bytes) - 角色数量
 * - characters[]: 每个角色的详细信息
 * - account_info: 账号级别信息
 */
static int build_userinfo_packet(uint8_t* buffer, int max_len, ClientSession* s) {
    if (!buffer || !s || max_len < 512) return -1;

    int offset = 0;

    /* Result code (0 = success) - 4 bytes */
    uint32_t result = 0;
    memcpy(buffer + offset, &result, 4);
    offset += 4;

    /* Character count - 4 bytes */
    uint32_t char_count = (uint32_t)s->character_count;
    memcpy(buffer + offset, &char_count, 4);
    offset += 4;

    /* Character list - 扩展格式 (128 bytes each) */
    for (int i = 0; i < s->character_count && i < MAX_CHARACTERS; i++) {
        DBCharacterInfo* ch = &s->characters[i];
        int char_start = offset;

        /* 基础信息 */
        /* Character number (4 bytes) */
        memcpy(buffer + offset, &ch->charac_no, 4);
        offset += 4;

        /* Character name (32 bytes) */
        memset(buffer + offset, 0, 32);
        strncpy((char*)(buffer + offset), ch->charac_name, 31);
        offset += 32;

        /* Job class (1 byte) */
        buffer[offset++] = ch->job;

        /* Grow type / 转职 (1 byte) */
        buffer[offset++] = ch->grow_type;

        /* Level (1 byte) */
        buffer[offset++] = ch->level;

        /* Expert job (1 byte) */
        buffer[offset++] = ch->expert_job;

        /* Skill tree index (1 byte, signed) */
        buffer[offset++] = (uint8_t)ch->skill_tree_index;

        /* Sex (1 byte) */
        buffer[offset++] = ch->sex;

        /* Delete status (1 byte) */
        buffer[offset++] = ch->deleted;

        /* Slot index (1 byte) */
        buffer[offset++] = (uint8_t)i;

        /* 战斗属性 */
        /* Max HP (2 bytes) */
        memcpy(buffer + offset, &ch->max_hp, 2);
        offset += 2;

        /* Max MP (2 bytes) */
        memcpy(buffer + offset, &ch->max_mp, 2);
        offset += 2;

        /* Physical attack (2 bytes) */
        memcpy(buffer + offset, &ch->phy_attack, 2);
        offset += 2;

        /* Physical defense (2 bytes) */
        memcpy(buffer + offset, &ch->phy_defense, 2);
        offset += 2;

        /* Magic attack (2 bytes) */
        memcpy(buffer + offset, &ch->mag_attack, 2);
        offset += 2;

        /* Magic defense (2 bytes) */
        memcpy(buffer + offset, &ch->mag_defense, 2);
        offset += 2;

        /* Move speed (2 bytes) */
        memcpy(buffer + offset, &ch->move_speed, 2);
        offset += 2;

        /* Attack speed (2 bytes) */
        memcpy(buffer + offset, &ch->attack_speed, 2);
        offset += 2;

        /* Cast speed (2 bytes) */
        memcpy(buffer + offset, &ch->cast_speed, 2);
        offset += 2;

        /* 状态信息 */
        /* Experience (4 bytes) */
        memcpy(buffer + offset, &ch->exp, 4);
        offset += 4;

        /* Gold (4 bytes) - 从 inventory.money 读取 */
        uint32_t gold = ch->gold;
        if (gold == 0) gold = 999999999; /* 默认值 */
        memcpy(buffer + offset, &gold, 4);
        offset += 4;

        /* Village ID (4 bytes) */
        memcpy(buffer + offset, &ch->village, 4);
        offset += 4;

        /* Fatigue (2 bytes) */
        memcpy(buffer + offset, &ch->fatigue, 2);
        offset += 2;

        /* Max fatigue (2 bytes) */
        memcpy(buffer + offset, &ch->max_fatigue, 2);
        offset += 2;

        /* 公会信息 */
        /* Guild ID (4 bytes) */
        memcpy(buffer + offset, &ch->guild_id, 4);
        offset += 4;

        /* Guild right (1 byte) */
        buffer[offset++] = ch->guild_right;

        /* Member flag (1 byte) */
        buffer[offset++] = ch->member_flag;

        /* 其他信息 */
        /* Dungeon clear point (4 bytes) */
        memcpy(buffer + offset, &ch->dungeon_clear_point, 4);
        offset += 4;

        /* Create time (4 bytes) - UNIX timestamp */
        memcpy(buffer + offset, &ch->create_time, 4);
        offset += 4;

        /* 装备外观数据 - 11个装备槽的物品ID (44 bytes) */
        for (int slot = 0; slot < MAX_EQUIP_SLOTS; slot++) {
            memcpy(buffer + offset, &ch->equip_item_ids[slot], 4);
            offset += 4;
        }

        /* Padding to 192 bytes (扩展后包含装备数据) */
        int char_size = offset - char_start;
        if (char_size < 192) {
            memset(buffer + offset, 0, 192 - char_size);
            offset = char_start + 192;
        }
    }

    /* 账号级别信息 */
    /* Account UID (4 bytes) */
    memcpy(buffer + offset, &s->account_id, 4);
    offset += 4;

    /* Premium expire time (4 bytes) - UNIX timestamp */
    uint32_t premium = 0;
    memcpy(buffer + offset, &premium, 4);
    offset += 4;

    /* CERA point (4 bytes) - 商城点数 */
    uint32_t cera = 0;
    memcpy(buffer + offset, &cera, 4);
    offset += 4;

    /* Balance (4 bytes) - 点券余额 */
    uint32_t balance = 500; /* 默认500点券 */
    memcpy(buffer + offset, &balance, 4);
    offset += 4;

    /* Max character slots (1 byte) */
    buffer[offset++] = MAX_CHARACTERS;

    /* VIP level (1 byte) */
    buffer[offset++] = 0;

    /* Reserved (2 bytes) */
    buffer[offset++] = 0;
    buffer[offset++] = 0;

    return offset;
}

/* Build CHARACTER_STATUS response */
static int build_character_status(uint8_t* buffer, int max_len, const DBCharacterInfo* ch) {
    if (!buffer || !ch || max_len < 128) return -1;

    int offset = 0;

    /* Result code (4 bytes for consistency with test client) */
    uint32_t result = 0;
    memcpy(buffer + offset, &result, 4);
    offset += 4;

    memcpy(buffer + offset, &ch->charac_no, 4);
    offset += 4;
    memset(buffer + offset, 0, MAX_CHARAC_NAME);
    strncpy((char*)(buffer + offset), ch->charac_name, MAX_CHARAC_NAME - 1);
    offset += MAX_CHARAC_NAME;
    buffer[offset++] = ch->job;
    buffer[offset++] = ch->grow_type;
    buffer[offset++] = ch->level;
    memcpy(buffer + offset, &ch->exp, 4);
    offset += 4;
    memcpy(buffer + offset, &ch->hp, 2);
    offset += 2;
    memcpy(buffer + offset, &ch->max_hp, 2);
    offset += 2;
    uint16_t mp = 30000;  /* Default MP */
    memcpy(buffer + offset, &mp, 2);
    offset += 2;
    memcpy(buffer + offset, &ch->max_mp, 2);
    offset += 2;
    uint32_t gold = 999999999;
    memcpy(buffer + offset, &gold, 4);
    offset += 4;
    memcpy(buffer + offset, &ch->village, 4);
    offset += 4;
    int32_t fatigue32 = ch->fatigue;
    memcpy(buffer + offset, &fatigue32, 4);
    offset += 4;

    return offset;
}

/*==============================================================================
 * Protocol Handlers
 *============================================================================*/

/* msg_no=0: CHECK_CONNECTION */
static int handle_check_connection(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] CHECK_CONNECTION from fd=%d\n", s->fd);

    uint8_t response[8];
    uint32_t result = 0;
    uint32_t server_time = htonl((uint32_t)time(NULL));
    memcpy(response, &result, 4);
    memcpy(response + 4, &server_time, 4);
    return send_response(s, 0, response, 8);
}

/* msg_no=1: LOGIN - Now with database lookup */
static int handle_login(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] LOGIN from fd=%d, data_len=%d\n", s->fd, len);

    /* Parse account info from packet */
    /* The login packet typically contains account_id or session info */
    uint32_t account_uid = 1;  /* Default to test account */

    /* Try to extract account_id from data if present */
    if (len >= 4) {
        memcpy(&account_uid, data, 4);
        if (account_uid == 0) account_uid = 1;
    }

    printf("[GAME] LOGIN: Looking up account UID=%u\n", account_uid);

    /* Load account from database */
    DBAccountInfo account;
    if (db_get_account_by_uid(account_uid, &account) < 0) {
        printf("[GAME] LOGIN: Account %u not found, using UID=1\n", account_uid);
        account_uid = 1;
        if (db_get_account_by_uid(account_uid, &account) < 0) {
            printf("[GAME] LOGIN: Failed to load account\n");
            uint8_t error[4] = {1, 0, 0, 0};
            return send_response(s, 1, error, 4);
        }
    }

    /* Update session */
    s->state = STATE_AUTHENTICATED;
    s->account_id = account.uid;
    strncpy(s->account_name, account.accountname, sizeof(s->account_name) - 1);

    /* Load characters from database */
    s->character_count = db_get_characters(s->account_id, s->characters, MAX_CHARACTERS);
    if (s->character_count < 0) {
        s->character_count = 0;
    }

    printf("[GAME] LOGIN success: account=%s uid=%u chars=%d\n",
           s->account_name, s->account_id, s->character_count);

    /* Build and send USERINFO response
     * Production: NOTIPACKET_USERINFO = 2 (not same as LOGIN cmd)
     */
    uint8_t response[1024];
    int response_len = build_userinfo_packet(response, sizeof(response), s);

    if (response_len > 0) {
        printf("[GAME] Sending USERINFO (%d bytes) with msg_no=2\n", response_len);
        return send_response(s, 2, response, response_len);  /* NOTIPACKET_USERINFO = 2 */
    }

    uint8_t error[4] = {1, 0, 0, 0};
    return send_response(s, 2, error, 4);
}

/* msg_no=2: SET_UDP_IP_PORT */
static int handle_set_udp(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] SET_UDP_IP_PORT from fd=%d\n", s->fd);

    /* Production: NOTIPACKET_USER_UDP_IP_PORT = 11 */
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 11, response, 4);
}

/* msg_no=3: EXIT */
static int handle_exit(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] EXIT from fd=%d\n", s->fd);

    s->state = STATE_DISCONNECTING;

    uint8_t response[4] = {0, 0, 0, 0};
    send_response(s, 3, response, 4);

    return -1;  /* Signal disconnect */
}

/* msg_no=4: SELECT_CHARACTER - Now with database lookup
 * Test client sends char_index (slot 0, 1, 2...)
 * Production client may send charac_no
 */
static int handle_select_character(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] SELECT_CHARACTER from fd=%d\n", s->fd);

    if (s->state < STATE_AUTHENTICATED) {
        printf("[GAME] SELECT_CHARACTER: not authenticated\n");
        uint8_t error[4] = {2, 0, 0, 0};  /* Not authenticated */
        return send_response(s, 4, error, 4);
    }

    if (len < 4) {
        printf("[GAME] SELECT_CHARACTER: packet too short\n");
        uint8_t error[4] = {1, 0, 0, 0};  /* Invalid packet */
        return send_response(s, 4, error, 4);
    }

    /* Parse character index or charac_no */
    uint32_t char_id;
    memcpy(&char_id, data, 4);
    printf("[GAME] SELECT_CHARACTER: id=%u (chars loaded: %d)\n", char_id, s->character_count);

    DBCharacterInfo* selected = NULL;

    /* First try as slot index (0-15) */
    if (char_id < (uint32_t)s->character_count) {
        selected = &s->characters[char_id];
        printf("[GAME] SELECT_CHARACTER: found by slot index %u\n", char_id);
    }

    /* If not found, try as charac_no */
    if (!selected) {
        for (int i = 0; i < s->character_count; i++) {
            if (s->characters[i].charac_no == char_id) {
                selected = &s->characters[i];
                printf("[GAME] SELECT_CHARACTER: found by charac_no %u\n", char_id);
                break;
            }
        }
    }

    /* If still not found, try loading from database */
    if (!selected && char_id > 0) {
        printf("[GAME] SELECT_CHARACTER: trying DB lookup for charac_no=%u\n", char_id);
        DBCharacterInfo ch;
        if (db_get_character(char_id, &ch) == 0 && ch.charac_no > 0) {
            /* Add to session */
            if (s->character_count < MAX_CHARACTERS) {
                s->characters[s->character_count] = ch;
                selected = &s->characters[s->character_count];
                s->character_count++;
            }
        }
    }

    if (!selected) {
        printf("[GAME] SELECT_CHARACTER: character not found\n");
        uint8_t error[4] = {4, 0, 0, 0};  /* Character not found */
        return send_response(s, 4, error, 4);
    }

    /* Update session */
    s->state = STATE_IN_GAME;
    s->character_id = selected->charac_no;
    strncpy(s->character_name, selected->charac_name, sizeof(s->character_name) - 1);

    /* Load inventory data from DB */
    if (db_get_inventory(selected->charac_no, &s->inventory) < 0) {
        printf("[GAME] SELECT_CHARACTER: inventory load failed, using defaults\n");
        s->inventory.charac_no = selected->charac_no;
        s->inventory.money = 0;
        s->inventory.inventory_capacity = 56;
        s->inventory.inventory_loaded = 0;
    }

    /* Load skill data from DB */
    if (db_get_skill(selected->charac_no, &s->skill) < 0) {
        printf("[GAME] SELECT_CHARACTER: skill load failed, using defaults\n");
        s->skill.charac_no = selected->charac_no;
        s->skill.remain_sp = 0;
        s->skill.skill_loaded = 0;
    }

    /* Load equipment data from DB */
    if (db_get_equipment(selected->charac_no, &s->equipment) < 0) {
        printf("[GAME] SELECT_CHARACTER: equipment load failed, using defaults\n");
        s->equipment.charac_no = selected->charac_no;
        s->equipment.equip_count = 0;
        s->equipment.equip_loaded = 0;
    }

    printf("[GAME] SELECT_CHARACTER: selected '%s' charac_no=%u (job=%d, level=%d, gold=%u, sp=%u, equips=%d)\n",
           selected->charac_name, selected->charac_no, selected->job, selected->level,
           s->inventory.money, s->skill.remain_sp, s->equipment.equip_count);

    /* Build and send CHARACTER_STATUS */
    uint8_t response[128];
    int response_len = build_character_status(response, sizeof(response), selected);

    if (response_len > 0) {
        return send_response(s, 4, response, response_len);
    }

    return 0;
}

/* msg_no=7: RETURN_SELECT_CHARACTER */
static int handle_return_select(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] RETURN_SELECT_CHARACTER from fd=%d\n", s->fd);

    s->state = STATE_CHAR_SELECT;
    s->character_id = 0;
    memset(s->character_name, 0, sizeof(s->character_name));

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 7, response, 4);
}

/* msg_no=8: GET_USERINFO */
static int handle_get_userinfo(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] GET_USERINFO from fd=%d\n", s->fd);

    if (s->state < STATE_AUTHENTICATED) {
        uint8_t error[4] = {3, 0, 0, 0};
        return send_response(s, 8, error, 4);
    }

    uint8_t response[1024];
    int response_len = build_userinfo_packet(response, sizeof(response), s);

    if (response_len > 0) {
        return send_response(s, 8, response, response_len);
    }

    return 0;
}

/* msg_no=15: START_GAME */
static int handle_start_game(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] START_GAME from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {3, 0, 0, 0};
        return send_response(s, 27, error, 4);  /* NOTIPACKET_START_GAME = 27 */
    }

    /* Response: result(1) + village_id(4) + position data */
    uint8_t response[32];
    memset(response, 0, sizeof(response));
    response[0] = 0;  /* Success */

    /* Get village from selected character */
    uint32_t village_id = 1;
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            village_id = s->characters[i].village;
            break;
        }
    }
    memcpy(response + 1, &village_id, 4);

    /* Initialize combat stats and current HP/MP */
    calculate_combat_stats(s);
    s->current_hp = s->combat_stats.max_hp;
    s->current_mp = s->combat_stats.max_mp;
    s->current_village = village_id;
    s->guild_id = 0;  /* TODO: Load from DB */
    s->party_id = 0;

    printf("[GAME] START_GAME: entering village %u (HP=%u, MP=%u)\n",
           village_id, s->current_hp, s->current_mp);
    return send_response(s, 27, response, 16);  /* NOTIPACKET_START_GAME = 27 */
}

/* msg_no=40: FINISH_LOADING */
static int handle_finish_loading(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] FINISH_LOADING from fd=%d, charac=%u\n", s->fd, s->character_id);

    /* Send FINISH_LOADING response first */
    uint8_t response[4] = {0, 0, 0, 0};
    send_response(s, 30, response, 4);  /* NOTIPACKET_FINISH_LOADING = 30 */

    /* Send initial game state after loading completes:
     * 1. ITEM_LIST (msg_no=13) - inventory
     * 2. SKILLINFO (msg_no=19) - skills
     * 3. USER_POSITION (msg_no=22) - initial position
     * 4. FATIGUE (msg_no=36) - fatigue value
     */

    /* Send ITEM_LIST if inventory was loaded */
    if (s->inventory.item_count > 0) {
        uint8_t item_buf[4096];
        int item_len = 0;

        /* Item count (4 bytes) */
        uint32_t count = (uint32_t)s->inventory.item_count;
        memcpy(item_buf + item_len, &count, 4);
        item_len += 4;

        /* Item entries (simplified: 20 bytes each) */
        for (int i = 0; i < s->inventory.item_count && i < 100; i++) {
            memcpy(item_buf + item_len, &s->inventory.items[i].it_id, 4);  /* item id */
            item_len += 4;
            memcpy(item_buf + item_len, &s->inventory.items[i].count, 2);  /* count */
            item_len += 2;
            memcpy(item_buf + item_len, &s->inventory.items[i].slot, 2);   /* slot */
            item_len += 2;
            item_buf[item_len++] = s->inventory.items[i].upgrade;          /* upgrade level */
            item_buf[item_len++] = s->inventory.items[i].seal;             /* seal flag */
            memcpy(item_buf + item_len, &s->inventory.items[i].flags, 2);  /* flags */
            item_len += 2;
            memset(item_buf + item_len, 0, 8);  /* reserved/expire */
            item_len += 8;
        }

        printf("[GAME] Sending ITEM_LIST with %d items\n", s->inventory.item_count);
        send_response(s, 13, item_buf, item_len);  /* NOTIPACKET_ITEM_LIST = 13 */
    }

    /* Send SKILLINFO (SP info only, no individual skills in this struct) */
    if (s->skill.skill_loaded) {
        uint8_t skill_buf[32];
        int skill_len = 0;

        /* SP data: remain_sp(4) + used_sp(4) + remain_sp_2nd(4) */
        memcpy(skill_buf + skill_len, &s->skill.remain_sp, 4);
        skill_len += 4;
        memcpy(skill_buf + skill_len, &s->skill.used_sp, 4);
        skill_len += 4;
        memcpy(skill_buf + skill_len, &s->skill.remain_sp_2nd, 4);
        skill_len += 4;
        memset(skill_buf + skill_len, 0, 8);  /* padding */
        skill_len += 8;

        printf("[GAME] Sending SKILLINFO (SP: remain=%u used=%u)\n",
               s->skill.remain_sp, s->skill.used_sp);
        send_response(s, 19, skill_buf, skill_len);  /* NOTIPACKET_SKILLINFO = 19 */
    }

    /* Send FATIGUE */
    uint8_t fatigue_buf[8];
    int16_t fatigue = 156;
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            fatigue = s->characters[i].fatigue;
            break;
        }
    }
    memcpy(fatigue_buf, &fatigue, 2);
    memset(fatigue_buf + 2, 0, 6);
    send_response(s, 36, fatigue_buf, 8);  /* NOTIPACKET_FATIGUE = 36 */

    printf("[GAME] FINISH_LOADING complete, game state sent\n");
    return 0;
}

/* msg_no=45: GIVEUP_GAME */
static int handle_giveup_game(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] GIVEUP_GAME from fd=%d\n", s->fd);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 45, response, 4);
}

/*==============================================================================
 * Extended Protocol Handlers
 *============================================================================*/

/* Job base stats for character creation */
typedef struct {
    uint16_t max_hp;
    uint16_t max_mp;
    uint16_t phy_attack;
    uint16_t mag_attack;
    uint16_t phy_defense;
    uint16_t mag_defense;
    uint16_t move_speed;
    uint16_t attack_speed;
    uint16_t cast_speed;
} JobBaseStats;

static const JobBaseStats g_job_base_stats[] = {
    /* JOB_SLAYER (0) - Balanced physical */
    {500, 200, 50, 30, 35, 25, 100, 100, 90},
    /* JOB_FIGHTER (1) - High attack, lower defense */
    {450, 180, 55, 25, 30, 20, 105, 110, 85},
    /* JOB_GUNNER (2) - Ranged, medium stats */
    {400, 220, 45, 40, 28, 28, 100, 95, 95},
    /* JOB_MAGE (3) - High magic, low physical */
    {350, 350, 25, 60, 20, 35, 95, 85, 110},
    /* JOB_PRIEST (4) - Support, balanced */
    {420, 300, 35, 45, 32, 38, 98, 90, 100},
    /* JOB_THIEF (5) - Fast, evasive */
    {380, 250, 48, 35, 25, 22, 115, 115, 95},
};

/* msg_no=5: CREATE_CHARACTER */
static int handle_create_character(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] CREATE_CHARACTER from fd=%d\n", s->fd);

    if (s->state < STATE_AUTHENTICATED) {
        uint8_t error[4] = {2, 0, 0, 0};  /* Not authenticated */
        return send_response(s, 5, error, 4);
    }

    if (len < 24) {
        uint8_t error[4] = {1, 0, 0, 0};  /* Invalid data */
        return send_response(s, 5, error, 4);
    }

    /* Parse create character data */
    char charac_name[MAX_CHARAC_NAME];
    memset(charac_name, 0, sizeof(charac_name));
    memcpy(charac_name, data, 16);
    uint8_t job = data[16];
    uint8_t grow_type = data[17];
    uint8_t sex = data[18];

    /* Validate job */
    if (job > 5) job = 0;

    printf("[GAME] CREATE_CHARACTER: name=%s job=%d grow=%d sex=%d\n",
           charac_name, job, grow_type, sex);

    /* Get base stats for job */
    const JobBaseStats* stats = &g_job_base_stats[job];

    /* Check if name exists */
    if (g_db_charac) {
        char check_query[256];
        char escaped_name[MAX_CHARAC_NAME * 2 + 1];
        mysql_real_escape_string(g_db_charac, escaped_name, charac_name, strlen(charac_name));

        snprintf(check_query, sizeof(check_query),
                 "SELECT charac_no FROM charac_info WHERE charac_name='%s'",
                 escaped_name);

        if (mysql_query(g_db_charac, check_query) == 0) {
            MYSQL_RES* check_res = mysql_store_result(g_db_charac);
            if (check_res) {
                if (mysql_num_rows(check_res) > 0) {
                    mysql_free_result(check_res);
                    printf("[GAME] Name already exists: %s\n", charac_name);
                    uint8_t error[4] = {3, 0, 0, 0};  /* Name exists */
                    return send_response(s, 5, error, 4);
                }
                mysql_free_result(check_res);
            }
        }
    }

    /* Insert into database */
    uint32_t charac_no = 0;
    if (g_db_charac) {
        char query[1024];
        char escaped_name[MAX_CHARAC_NAME * 2 + 1];
        mysql_real_escape_string(g_db_charac, escaped_name, charac_name, strlen(charac_name));

        snprintf(query, sizeof(query),
            "INSERT INTO charac_info (m_id, charac_name, village, job, lev, exp, grow_type, "
            "HP, maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
            "move_speed, attack_speed, cast_speed, max_fatigue, sex) VALUES "
            "(%u, '%s', 1, %d, 1, 0, %d, 100, %d, %d, %d, %d, %d, %d, %d, %d, %d, 70, %d)",
            s->account_id, escaped_name, job, grow_type,
            stats->max_hp, stats->max_mp,
            stats->phy_attack, stats->phy_defense,
            stats->mag_attack, stats->mag_defense,
            stats->move_speed, stats->attack_speed, stats->cast_speed,
            sex);

        if (mysql_query(g_db_charac, query)) {
            printf("[DB] Insert character failed: %s\n", mysql_error(g_db_charac));
            uint8_t error[4] = {4, 0, 0, 0};  /* DB error */
            return send_response(s, 5, error, 4);
        }

        charac_no = (uint32_t)mysql_insert_id(g_db_charac);
        printf("[DB] Created character %s with charac_no=%u\n", charac_name, charac_no);

        /* Create charac_stat entry */
        snprintf(query, sizeof(query),
            "INSERT INTO charac_stat (charac_no, village, exp, HP, fatigue, max_fatigue) "
            "VALUES (%u, 1, 0, 100, 0, 70)", charac_no);
        mysql_query(g_db_charac, query);

        /* Create inventory entry (taiwan_cain_2nd) */
        if (g_db_charac2) {
            snprintf(query, sizeof(query),
                "INSERT INTO inventory (charac_no, money, slot) VALUES (%u, 10000, 0)",
                charac_no);
            mysql_query(g_db_charac2, query);

            /* Give starter items based on job */
            uint32_t weapon_id = 100001 + job * 100;  /* Basic weapon per job */
            snprintf(query, sizeof(query),
                "INSERT INTO user_items (charac_no, slot, it_id, stat, reg_date, m_time) "
                "VALUES (%u, 0, %u, 0, NOW(), NOW())",
                charac_no, weapon_id);
            mysql_query(g_db_charac2, query);

            printf("[DB] Created starter inventory for charac_no=%u\n", charac_no);
        }
    } else {
        /* No DB - return mock */
        charac_no = 90000 + (s->account_id % 1000);
    }

    /* Response */
    uint8_t response[16];
    memset(response, 0, sizeof(response));
    uint32_t result = 0;  /* Success */
    memcpy(response, &result, 4);
    memcpy(response + 4, &charac_no, 4);

    printf("[GAME] Character created successfully: charac_no=%u\n", charac_no);
    return send_response(s, 5, response, 16);
}

/* msg_no=6: DELETE_CHARACTER */
static int handle_delete_character(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DELETE_CHARACTER from fd=%d\n", s->fd);

    if (s->state < STATE_AUTHENTICATED || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 6, error, 4);
    }

    uint32_t charac_no;
    memcpy(&charac_no, data, 4);
    printf("[GAME] DELETE_CHARACTER: charac_no=%u\n", charac_no);

    /* Soft delete character */
    if (db_soft_delete_character(charac_no, s->account_id) < 0) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Delete failed */
        return send_response(s, 6, error, 4);
    }

    /* Optionally clean up items (disabled for recovery purposes) */
    /* db_cleanup_deleted_character_items(charac_no); */

    printf("[GAME] Character %u deleted successfully\n", charac_no);
    uint8_t response[4] = {0, 0, 0, 0};  /* Success */
    return send_response(s, 6, response, 4);
}

/* msg_no=9: HEARTBEAT/PING */
static int handle_heartbeat(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    s->last_activity = (uint32_t)time(NULL);

    uint8_t response[8];
    uint32_t result = 0;
    uint32_t server_time = (uint32_t)time(NULL);
    memcpy(response, &result, 4);
    memcpy(response + 4, &server_time, 4);

    return send_response(s, 9, response, 8);
}

/* Chat type definitions */
#define CHAT_TYPE_NORMAL   0   /* Area/Normal chat */
#define CHAT_TYPE_WHISPER  1   /* Private message */
#define CHAT_TYPE_PARTY    2   /* Party chat */
#define CHAT_TYPE_GUILD    3   /* Guild chat */
#define CHAT_TYPE_WORLD    4   /* World chat */
#define CHAT_TYPE_SYSTEM   5   /* System message */

/* Helper: Find session by character name */
static ClientSession* find_session_by_name(const char* name) {
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].fd > 0 && g_sessions[i].state >= STATE_IN_GAME) {
            if (strncmp(g_sessions[i].character_name, name, 20) == 0) {
                return &g_sessions[i];
            }
        }
    }
    return NULL;
}

/* Helper: Broadcast chat message */
static void broadcast_chat(ClientSession* sender, uint8_t chat_type,
                          const char* message, int msg_len, uint32_t area_filter) {
    uint8_t response[300];
    memset(response, 0, sizeof(response));
    int offset = 0;

    response[offset++] = 0;  /* Result */
    response[offset++] = chat_type;

    /* Sender name */
    memset(response + offset, 0, 20);
    strncpy((char*)(response + offset), sender->character_name, 19);
    offset += 20;

    /* Message */
    response[offset++] = (uint8_t)msg_len;
    memcpy(response + offset, message, msg_len);
    offset += msg_len;

    /* Broadcast to relevant sessions */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        ClientSession* target = &g_sessions[i];
        if (target->fd <= 0 || target->state < STATE_IN_GAME) continue;

        int should_send = 0;

        switch (chat_type) {
            case CHAT_TYPE_WORLD:
                /* Send to all online players */
                should_send = 1;
                break;

            case CHAT_TYPE_NORMAL:
                /* Send to players in same area (same village) */
                if (area_filter == 0 || target->current_village == area_filter) {
                    should_send = 1;
                }
                break;

            case CHAT_TYPE_GUILD:
                /* Send to same guild members */
                if (sender->guild_id > 0 && target->guild_id == sender->guild_id) {
                    should_send = 1;
                }
                break;

            case CHAT_TYPE_PARTY:
                /* Send to same party members */
                if (sender->party_id > 0 && target->party_id == sender->party_id) {
                    should_send = 1;
                }
                break;

            default:
                break;
        }

        if (should_send) {
            send_response(target, 10, response, offset);
        }
    }
}

/* msg_no=10: CHAT */
static int handle_chat(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] CHAT from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 2) {
        return 0;  /* Silently ignore */
    }

    /* Parse chat type and message */
    uint8_t chat_type = data[0];
    uint8_t msg_len = data[1];
    char message[256];
    memset(message, 0, sizeof(message));

    if (msg_len > 0 && msg_len < 250 && len >= (int)(msg_len + 2)) {
        memcpy(message, data + 2, msg_len);
    }

    printf("[GAME] CHAT: type=%d msg='%s'\n", chat_type, message);

    switch (chat_type) {
        case CHAT_TYPE_WORLD:
            printf("[GAME] World chat from %s: %s\n", s->character_name, message);
            broadcast_chat(s, chat_type, message, msg_len, 0);
            break;

        case CHAT_TYPE_GUILD:
            if (s->guild_id > 0) {
                printf("[GAME] Guild chat from %s (guild=%u): %s\n",
                       s->character_name, s->guild_id, message);
                broadcast_chat(s, chat_type, message, msg_len, 0);
            } else {
                /* Not in guild - send error */
                uint8_t error[4] = {1, 0, 0, 0};
                return send_response(s, 10, error, 4);
            }
            break;

        case CHAT_TYPE_PARTY:
            if (s->party_id > 0) {
                printf("[GAME] Party chat from %s (party=%u): %s\n",
                       s->character_name, s->party_id, message);
                broadcast_chat(s, chat_type, message, msg_len, 0);
            } else {
                uint8_t error[4] = {2, 0, 0, 0};
                return send_response(s, 10, error, 4);
            }
            break;

        case CHAT_TYPE_WHISPER: {
            /* Whisper: target name follows message */
            char target_name[21];
            memset(target_name, 0, sizeof(target_name));

            /* Format: type(1) + msg_len(1) + msg + target_name_len(1) + target_name */
            int name_offset = 2 + msg_len;
            if (len > name_offset) {
                uint8_t name_len = data[name_offset];
                if (name_len > 0 && name_len <= 20 && len >= name_offset + 1 + name_len) {
                    memcpy(target_name, data + name_offset + 1, name_len);
                }
            }

            printf("[GAME] Whisper from %s to %s: %s\n",
                   s->character_name, target_name, message);

            ClientSession* target = find_session_by_name(target_name);
            if (target) {
                /* Send to target */
                uint8_t response[300];
                memset(response, 0, sizeof(response));
                int offset = 0;

                response[offset++] = 0;  /* Result: success */
                response[offset++] = CHAT_TYPE_WHISPER;
                memset(response + offset, 0, 20);
                strncpy((char*)(response + offset), s->character_name, 19);
                offset += 20;
                response[offset++] = msg_len;
                memcpy(response + offset, message, msg_len);
                offset += msg_len;

                send_response(target, 10, response, offset);

                /* Send confirmation to sender */
                uint8_t confirm[4] = {0, 0, 0, 0};  /* Success */
                return send_response(s, 10, confirm, 4);
            } else {
                /* Target not found */
                uint8_t error[4] = {3, 0, 0, 0};  /* User not found */
                return send_response(s, 10, error, 4);
            }
        }

        case CHAT_TYPE_NORMAL:
        default:
            /* Area chat - broadcast to same village */
            printf("[GAME] Area chat from %s: %s\n", s->character_name, message);
            broadcast_chat(s, CHAT_TYPE_NORMAL, message, msg_len, s->current_village);
            break;
    }

    return 0;
}

/* msg_no=16: MOVE_AREA (change map) */
static int handle_move_area(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] SET_USER_AREA from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 23, error, 4);  /* NOTIPACKET_USER_AREA = 23 */
    }

    uint32_t village_id;
    memcpy(&village_id, data, 4);
    printf("[GAME] SET_USER_AREA: village_id=%u\n", village_id);

    /* Update character's village */
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            s->characters[i].village = village_id;
            break;
        }
    }
    s->current_village = village_id;

    /* Response: result + village_id + spawn position */
    uint8_t response[24];
    memset(response, 0, sizeof(response));
    uint32_t result = 0;
    memcpy(response, &result, 4);
    memcpy(response + 4, &village_id, 4);

    /* Default spawn position */
    uint16_t spawn_x = 400;
    uint16_t spawn_y = 300;
    memcpy(response + 8, &spawn_x, 2);
    memcpy(response + 10, &spawn_y, 2);

    return send_response(s, 23, response, 16);  /* NOTIPACKET_USER_AREA = 23 */
}

/* msg_no=37: SET_USER_POSITION (position update) */
static int handle_move(ClientSession* s, const uint8_t* data, int len) {
    if (s->state < STATE_IN_GAME || len < 4) {
        return 0;
    }

    /* Parse position */
    uint16_t x, y;
    memcpy(&x, data, 2);
    memcpy(&y, data + 2, 2);

    /* Direction and state */
    uint8_t direction = (len >= 5) ? data[4] : 0;
    uint8_t move_type = (len >= 6) ? data[5] : 0;

    printf("[GAME] SET_USER_POSITION: x=%u y=%u dir=%d type=%d\n", x, y, direction, move_type);

    /* ACK back with NOTIPACKET_USER_POSITION = 22 */
    uint8_t response[16];
    memset(response, 0, sizeof(response));
    memcpy(response, &x, 2);
    memcpy(response + 2, &y, 2);
    response[4] = direction;
    response[5] = move_type;

    return send_response(s, 22, response, 8);  /* NOTIPACKET_USER_POSITION = 22 */
}

/* msg_no=21: MOVE_STOP */
static int handle_move_stop(ClientSession* s, const uint8_t* data, int len) {
    if (s->state < STATE_IN_GAME || len < 4) {
        return 0;
    }

    uint16_t x, y;
    memcpy(&x, data, 2);
    memcpy(&y, data + 2, 2);

    printf("[GAME] MOVE_STOP: x=%u y=%u\n", x, y);

    uint8_t response[8];
    memcpy(response, &x, 2);
    memcpy(response + 2, &y, 2);
    memset(response + 4, 0, 4);

    return send_response(s, 21, response, 8);
}

/* msg_no=25: GET_INVENTORY */
static int handle_get_inventory(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] GET_INVENTORY from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 13, error, 4);  /* NOTIPACKET_ITEM_LIST = 13 */
    }

    /* Reload inventory if not loaded */
    if (!s->inventory.inventory_loaded && s->character_id > 0) {
        db_get_inventory(s->character_id, &s->inventory);
    }

    /* Build inventory response */
    uint8_t response[1024];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Result */
    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Gold - from DB */
    uint32_t gold = s->inventory.money;
    memcpy(response + offset, &gold, 4);
    offset += 4;

    /* Inventory capacity - from DB */
    uint16_t capacity = (uint16_t)s->inventory.inventory_capacity;
    if (capacity == 0) capacity = 56;
    memcpy(response + offset, &capacity, 2);
    offset += 2;

    /* Coin (points) - from DB */
    uint32_t coin = s->inventory.coin;
    memcpy(response + offset, &coin, 4);
    offset += 4;

    /* Pay coin - from DB */
    uint32_t pay_coin = s->inventory.pay_coin;
    memcpy(response + offset, &pay_coin, 4);
    offset += 4;

    /* Event coin - from DB */
    uint32_t event_coin = s->inventory.event_coin;
    memcpy(response + offset, &event_coin, 4);
    offset += 4;

    /* Item count - from DB */
    uint16_t item_count = s->inventory.item_count;
    memcpy(response + offset, &item_count, 2);
    offset += 2;

    /* Real items from DB: slot(2) + item_id(4) + count(2) + upgrade(1) + seal(1) + flags(2) = 12 bytes each */
    for (uint16_t i = 0; i < item_count && i < MAX_BAG_ITEMS; i++) {
        DBBagItem* item = &s->inventory.items[i];

        memcpy(response + offset, &item->slot, 2);
        offset += 2;
        memcpy(response + offset, &item->it_id, 4);
        offset += 4;
        memcpy(response + offset, &item->count, 2);
        offset += 2;
        response[offset++] = item->upgrade;
        response[offset++] = item->seal;
        memcpy(response + offset, &item->flags, 2);
        offset += 2;
    }

    printf("[GAME] GET_INVENTORY: gold=%u, capacity=%u, items=%u\n", gold, capacity, item_count);
    return send_response(s, 13, response, offset);  /* NOTIPACKET_ITEM_LIST = 13 */
}

/* Item effect types */
#define ITEM_EFFECT_HP_RESTORE    1
#define ITEM_EFFECT_MP_RESTORE    2
#define ITEM_EFFECT_HP_MP_RESTORE 3
#define ITEM_EFFECT_BUFF          4

/* msg_no=26: USE_ITEM */
static int handle_use_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] USE_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 6) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 26, error, 4);
    }

    /* Parse: slot(2) + item_id(4) */
    uint16_t slot;
    uint32_t item_id;
    memcpy(&slot, data, 2);
    memcpy(&item_id, data + 2, 4);

    printf("[GAME] USE_ITEM: slot=%u item_id=%u\n", slot, item_id);

    /* Determine item effect based on item_id ranges
     * 10001-19999: HP potions (restore HP by item_id % 1000 * 10)
     * 20001-29999: MP potions (restore MP by item_id % 1000 * 10)
     * 30001-39999: HP+MP potions (restore both)
     * Others: Generic item use
     */
    uint32_t old_hp = s->current_hp;
    uint32_t old_mp = s->current_mp;
    uint32_t effect_type = 0;
    uint32_t effect_value = 0;

    if (item_id >= 10001 && item_id < 20000) {
        /* HP Potion */
        effect_type = ITEM_EFFECT_HP_RESTORE;
        effect_value = (item_id % 1000) * 10;
        if (effect_value == 0) effect_value = 100;
        s->current_hp += effect_value;
        if (s->current_hp > s->combat_stats.max_hp) {
            s->current_hp = s->combat_stats.max_hp;
        }
        printf("[GAME] HP Potion: restored %u HP (%u -> %u)\n",
               effect_value, old_hp, s->current_hp);
    }
    else if (item_id >= 20001 && item_id < 30000) {
        /* MP Potion */
        effect_type = ITEM_EFFECT_MP_RESTORE;
        effect_value = (item_id % 1000) * 10;
        if (effect_value == 0) effect_value = 100;
        s->current_mp += effect_value;
        if (s->current_mp > s->combat_stats.max_mp) {
            s->current_mp = s->combat_stats.max_mp;
        }
        printf("[GAME] MP Potion: restored %u MP (%u -> %u)\n",
               effect_value, old_mp, s->current_mp);
    }
    else if (item_id >= 30001 && item_id < 40000) {
        /* HP+MP Potion */
        effect_type = ITEM_EFFECT_HP_MP_RESTORE;
        effect_value = (item_id % 1000) * 10;
        if (effect_value == 0) effect_value = 100;
        s->current_hp += effect_value;
        s->current_mp += effect_value;
        if (s->current_hp > s->combat_stats.max_hp) {
            s->current_hp = s->combat_stats.max_hp;
        }
        if (s->current_mp > s->combat_stats.max_mp) {
            s->current_mp = s->combat_stats.max_mp;
        }
        printf("[GAME] HP+MP Potion: restored %u each (HP: %u->%u, MP: %u->%u)\n",
               effect_value, old_hp, s->current_hp, old_mp, s->current_mp);
    }
    else {
        /* Generic item - just acknowledge use */
        effect_type = 0;
        effect_value = 0;
        printf("[GAME] Generic item used: %u\n", item_id);
    }

    /* Delete consumable item from database */
    if (effect_type != 0) {
        /* Item was a consumable (potion), delete from DB */
        if (db_delete_item(s->character_id, slot) == 0) {
            printf("[GAME] Consumable item deleted from DB: slot=%u\n", slot);
        }
    }

    /* Response: result(4) + slot(2) + item_id(4) + effect_type(1) + hp(4) + mp(4) */
    uint8_t response[24];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;
    memcpy(response + offset, &slot, 2);
    offset += 2;
    memcpy(response + offset, &item_id, 4);
    offset += 4;
    response[offset++] = (uint8_t)effect_type;
    memcpy(response + offset, &s->current_hp, 4);
    offset += 4;
    memcpy(response + offset, &s->current_mp, 4);
    offset += 4;

    return send_response(s, 26, response, offset);
}

/* msg_no=27: MOVE_ITEM (inventory management) */
static int handle_move_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] MOVE_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 27, error, 4);
    }

    uint16_t from_slot, to_slot;
    memcpy(&from_slot, data, 2);
    memcpy(&to_slot, data + 2, 2);

    printf("[GAME] MOVE_ITEM: from=%u to=%u\n", from_slot, to_slot);

    /* Update database */
    if (from_slot != to_slot) {
        /* Check if target slot has an item (for swap) */
        uint32_t target_item = 0;
        if (db_get_item_at_slot(s->character_id, to_slot, &target_item) == 0 && target_item > 0) {
            /* Swap: move target item to temp slot, move source to target, move temp to source */
            int temp_slot = 9999;  /* Temporary slot */
            db_update_item_slot(s->character_id, to_slot, temp_slot);
            db_update_item_slot(s->character_id, from_slot, to_slot);
            db_update_item_slot(s->character_id, temp_slot, from_slot);
            printf("[MOVE] Swapped items between slots %u and %u\n", from_slot, to_slot);
        } else {
            /* Simple move */
            db_update_item_slot(s->character_id, from_slot, to_slot);
            printf("[MOVE] Moved item from slot %u to %u\n", from_slot, to_slot);
        }
    }

    uint8_t response[4] = {0, 0, 0, 0};  /* Success */
    return send_response(s, 19, response, 4);  /* Production: MOVE_ITEMSPACE = 19 */
}

/* msg_no=18: DELETE_ITEM - Production protocol */
static int handle_delete_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DELETE_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 2) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 18, error, 4);
    }

    uint16_t slot;
    memcpy(&slot, data, 2);
    printf("[GAME] DELETE_ITEM: slot=%u\n", slot);

    /* Delete item from database */
    db_delete_item_at_slot(s->character_id, slot);

    uint8_t response[4] = {0, 0, 0, 0};  /* Success */
    return send_response(s, 18, response, 4);
}

/* msg_no=20: SORT_ITEM - Production protocol */
static int handle_sort_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] SORT_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 20, error, 4);
    }

    /* Sort items in inventory - for now just acknowledge */
    printf("[GAME] Sorting inventory for character %u\n", s->character_id);

    uint8_t response[4] = {0, 0, 0, 0};  /* Success */
    return send_response(s, 20, response, 4);
}

/* msg_no=25: REPAIR_EQUIPMENT - Production protocol */
static int handle_repair_equipment(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] REPAIR_EQUIPMENT from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 3) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 25, error, 4);
    }

    uint16_t slot;
    memcpy(&slot, data, 2);
    uint8_t repair_all = data[2];

    printf("[GAME] REPAIR_EQUIPMENT: slot=%u repair_all=%u\n", slot, repair_all);

    uint32_t total_cost = 0;
    int repaired_count = 0;

    if (repair_all) {
        /* Repair all equipped items (slots 0-12) */
        int i;
        for (i = 0; i <= 12; i++) {
            uint32_t item_id = 0;
            uint8_t durability = 100;
            if (db_get_item_info_at_slot(s->character_id, i, &item_id, &durability) == 0) {
                if (item_id > 0 && durability < 100) {
                    total_cost += calc_repair_cost(item_id, durability);
                }
            }
        }

        /* Check if player has enough gold */
        if (s->inventory.money < total_cost) {
            uint8_t response[4] = {2, 0, 0, 0}; /* Error: not enough gold */
            return send_response(s, 25, response, 4);
        }

        /* Deduct gold and repair */
        if (total_cost > 0) {
            s->inventory.money -= total_cost;
            db_update_gold(s->character_id, s->inventory.money);
        }
        repaired_count = db_repair_all_items(s->character_id);
    } else {
        /* Repair single item */
        uint32_t item_id = 0;
        uint8_t durability = 100;
        if (db_get_item_info_at_slot(s->character_id, slot, &item_id, &durability) == 0) {
            if (item_id > 0 && durability < 100) {
                total_cost = calc_repair_cost(item_id, durability);

                if (s->inventory.money < total_cost) {
                    uint8_t response[4] = {2, 0, 0, 0}; /* Error: not enough gold */
                    return send_response(s, 25, response, 4);
                }

                s->inventory.money -= total_cost;
                db_update_gold(s->character_id, s->inventory.money);
                if (db_repair_item(s->character_id, slot) == 0) {
                    repaired_count = 1;
                }
            }
        }
    }

    /* Build response: result(4) + repaired_count(4) + total_cost(4) */
    uint8_t response[12];
    memset(response, 0, 12);
    memcpy(response + 4, &repaired_count, 4);
    memcpy(response + 8, &total_cost, 4);

    printf("[GAME] REPAIR_EQUIPMENT: repaired=%d cost=%u gold_left=%u\n",
           repaired_count, total_cost, s->inventory.money);

    return send_response(s, 25, response, 12);
}

/* msg_no=27: COMPOUND_ITEM - Production protocol */
static int handle_compound_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] COMPOUND_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 27, error, 4);
    }

    /* TODO: Implement item compound/crafting */
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 27, response, 4);
}

/* msg_no=28: DISJOINT_ITEM - Production protocol */
static int handle_disjoint_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DISJOINT_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 28, error, 4);
    }

    /* TODO: Implement item disassembly */
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 28, response, 4);
}

/* msg_no=30: CHANGE_SKILLSLOT - Production protocol */
static int handle_change_skillslot(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] CHANGE_SKILLSLOT from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 30, error, 4);
    }

    /* TODO: Update skill slot bindings */
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 30, response, 4);
}

/* msg_no=31: BUY_SKILL - Production protocol */
static int handle_buy_skill(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] BUY_SKILL from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 2) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 31, error, 4);
    }

    uint16_t skill_id;
    memcpy(&skill_id, data, 2);
    printf("[GAME] BUY_SKILL: skill_id=%u\n", skill_id);

    /* TODO: Check SP, deduct, add skill to character */
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 31, response, 4);
}

/* msg_no=32: INCREASE_STATUS - Production protocol */
static int handle_increase_status(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] INCREASE_STATUS from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 1) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 32, error, 4);
    }

    uint8_t stat_type = data[0];  /* 0=STR, 1=INT, 2=VIT, 3=MEN */
    printf("[GAME] INCREASE_STATUS: stat_type=%u\n", stat_type);

    /* TODO: Check stat points, increase stat in database */
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 32, response, 4);
}

/* msg_no=34: GIVEUP_QUEST - Production protocol */
static int handle_giveup_quest(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] GIVEUP_QUEST from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 34, error, 4);
    }

    uint32_t quest_id;
    memcpy(&quest_id, data, 4);
    printf("[GAME] GIVEUP_QUEST: quest_id=%u\n", quest_id);

    /* TODO: Remove quest from character's quest list */
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 34, response, 4);
}

/* msg_no=35: SET_QUEST_TRIGGER - Production protocol */
static int handle_set_quest_trigger(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] SET_QUEST_TRIGGER from fd=%d\n", s->fd);

    /* Quest trigger update - acknowledge */
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 35, response, 4);
}

/* msg_no=42: DIE_MONSTER - Production protocol */
static int handle_die_monster(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DIE_MONSTER from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        return 0;
    }

    /* Monster death notification from client - for P2P sync */
    /* Server should verify and broadcast to other party members */
    return 0;  /* No response needed */
}

/* msg_no=43: DIE_CHARACTER - Production protocol */
static int handle_die_character(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DIE_CHARACTER from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        return 0;
    }

    /* Character death notification */
    return 0;
}

/* msg_no=44: USE_COIN - Production protocol (resurrection) */
static int handle_use_coin(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] USE_COIN from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 44, error, 4);
    }

    /* TODO: Check/deduct resurrection coins, resurrect character */
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 44, response, 4);
}

/* msg_no=46: GET_ITEM - Production protocol (pickup) */
static int handle_get_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] GET_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        return 0;
    }

    /* Item pickup notification - handled in dungeon context */
    return 0;
}

/* msg_no=50: DROP_ITEM - Production protocol */
static int handle_drop_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DROP_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 2) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 50, error, 4);
    }

    uint16_t slot;
    memcpy(&slot, data, 2);
    printf("[GAME] DROP_ITEM: slot=%u\n", slot);

    /* Delete from inventory */
    db_delete_item_at_slot(s->character_id, slot);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 50, response, 4);
}

/* msg_no=53: MAKE_PVP_ROOM - Production protocol */
static int handle_pvp_create_room(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] MAKE_PVP_ROOM from fd=%d\n", s->fd);
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 53, response, 4);
}

/* msg_no=54: ENTER_PVP_ROOM - Production protocol */
static int handle_pvp_enter_room(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] ENTER_PVP_ROOM from fd=%d\n", s->fd);
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 54, response, 4);
}

/* msg_no=55: SET_PVP_SEAT_STATE - Production protocol */
static int handle_pvp_set_seat(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] SET_PVP_SEAT_STATE from fd=%d\n", s->fd);
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 55, response, 4);
}

/* msg_no=65: DEBUG_COMMAND - Production protocol */
static int handle_debug_command(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DEBUG_COMMAND from fd=%d\n", s->fd);
    /* Debug commands - should be restricted in production */
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 65, response, 4);
}

/* msg_no=83: UPGRADE_ITEM - Production protocol */
static int handle_upgrade_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] UPGRADE_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 83, error, 4);
    }

    /* TODO: Implement item enhancement/upgrade */
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 83, response, 4);
}

/* msg_no=100: DEPOSIT_MONEY - Production protocol */
static int handle_storage_deposit(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DEPOSIT_MONEY from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 100, error, 4);
    }

    uint32_t amount;
    memcpy(&amount, data, 4);
    printf("[GAME] DEPOSIT_MONEY: amount=%u\n", amount);

    /* TODO: Transfer gold from character to storage */
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 100, response, 4);
}

/* msg_no=101: WITHDRAW_MONEY - Production protocol */
static int handle_storage_withdraw(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] WITHDRAW_MONEY from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 101, error, 4);
    }

    uint32_t amount;
    memcpy(&amount, data, 4);
    printf("[GAME] WITHDRAW_MONEY: amount=%u\n", amount);

    /* TODO: Transfer gold from storage to character */
    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 101, response, 4);
}

/* msg_no=41: USE_SKILL - Production protocol */
static int handle_use_skill(ClientSession* s, const uint8_t* data, int len) {
    if (s->state < STATE_IN_GAME || len < 6) {
        return 0;
    }

    uint16_t skill_id;
    uint16_t target_x, target_y;
    memcpy(&skill_id, data, 2);
    memcpy(&target_x, data + 2, 2);
    memcpy(&target_y, data + 4, 2);

    printf("[GAME] USE_SKILL: skill_id=%u target=(%u,%u)\n", skill_id, target_x, target_y);

    /* Response: result + cooldown time */
    uint8_t response[8];
    memset(response, 0, sizeof(response));
    uint32_t result = 0;
    uint16_t cooldown = 1000;  /* 1 second cooldown */
    memcpy(response, &result, 4);
    memcpy(response + 4, &cooldown, 2);

    return send_response(s, 30, response, 8);
}

/* msg_no=31: GET_SKILL_LIST */
static int handle_get_skill_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] GET_SKILL_LIST from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 19, error, 4);  /* NOTIPACKET_SKILLINFO = 19 */
    }

    /* Reload skill if not loaded */
    if (!s->skill.skill_loaded && s->character_id > 0) {
        db_get_skill(s->character_id, &s->skill);
    }

    /* Build skill list response */
    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Result */
    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* SP (skill points) - from DB */
    uint32_t sp = s->skill.remain_sp;
    memcpy(response + offset, &sp, 4);
    offset += 4;

    /* Used SP - from DB */
    uint32_t used_sp = s->skill.used_sp;
    memcpy(response + offset, &used_sp, 4);
    offset += 4;

    /* 2nd awakening SP - from DB */
    uint32_t sp_2nd = s->skill.remain_sp_2nd;
    memcpy(response + offset, &sp_2nd, 4);
    offset += 4;

    /* Skill count (placeholder - would need to parse skill_slot blob) */
    uint16_t skill_count = 5;
    memcpy(response + offset, &skill_count, 2);
    offset += 2;

    /* Mock skills: skill_id(2) + level(1) + slot(1) = 4 bytes each */
    /* TODO: Parse skill_slot blob for real skills */
    uint16_t base_skills[] = {1, 2, 3, 4, 5};  /* Basic attack skills */
    for (int i = 0; i < 5; i++) {
        memcpy(response + offset, &base_skills[i], 2);
        offset += 2;
        response[offset++] = 1;  /* Level 1 */
        response[offset++] = i;  /* Slot */
    }

    printf("[GAME] GET_SKILL_LIST: remain_sp=%u, used_sp=%u, sp_2nd=%u\n", sp, used_sp, sp_2nd);
    return send_response(s, 19, response, offset);  /* NOTIPACKET_SKILLINFO = 19 */
}

/* msg_no=32: GET_EQUIPMENT */
static int handle_get_equipment(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] GET_EQUIPMENT from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 32, error, 4);
    }

    /* Reload equipment if not loaded */
    if (!s->equipment.equip_loaded && s->character_id > 0) {
        db_get_equipment(s->character_id, &s->equipment);
    }

    /* Build equipment response */
    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Result */
    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Equipment count */
    uint8_t equip_count = s->equipment.equip_count;
    response[offset++] = equip_count;

    /* Each equipment: slot(1) + item_id(4) + stat(1) + hidden_option(2) = 8 bytes */
    for (int i = 0; i < MAX_EQUIP_SLOTS; i++) {
        if (s->equipment.equipment[i].it_id > 0) {
            response[offset++] = (uint8_t)i;  /* Slot */
            memcpy(response + offset, &s->equipment.equipment[i].it_id, 4);
            offset += 4;
            response[offset++] = s->equipment.equipment[i].stat;
            memcpy(response + offset, &s->equipment.equipment[i].hidden_option, 2);
            offset += 2;
        }
    }

    printf("[GAME] GET_EQUIPMENT: count=%d\n", equip_count);
    return send_response(s, 32, response, offset);
}

/* msg_no=33: EQUIP_ITEM */
static int handle_equip_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] EQUIP_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 33, error, 4);
    }

    uint8_t equip_slot = data[0];  /* Target equipment slot (0-10) */
    uint32_t item_id;
    memcpy(&item_id, data + 1, 4);

    /* Check if source slot is provided (extended format) */
    int source_slot = -1;
    if (len >= 9) {
        memcpy(&source_slot, data + 5, 4);
    }

    printf("[GAME] EQUIP_ITEM: equip_slot=%d, item_id=%u, source_slot=%d\n",
           equip_slot, item_id, source_slot);

    /* Validate slot */
    if (equip_slot >= MAX_EQUIP_SLOTS) {
        uint8_t error[4] = {2, 0, 0, 0};  /* Invalid slot */
        return send_response(s, 33, error, 4);
    }

    /* Update database - move item from inventory to equipment slot */
    if (source_slot >= MAX_EQUIP_SLOTS) {
        /* If there's already an item in equipment slot, swap to source slot */
        uint32_t existing_item = 0;
        if (db_get_item_at_slot(s->character_id, equip_slot, &existing_item) == 0) {
            /* Move existing equipped item to source slot */
            db_update_item_slot(s->character_id, equip_slot, source_slot);
            printf("[EQUIP] Swapped existing item %u from equip to slot %d\n", existing_item, source_slot);
        }
        /* Move new item from source to equipment slot */
        db_update_item_slot(s->character_id, source_slot, equip_slot);
        printf("[EQUIP] Equipped item %u from slot %d to equip slot %d\n", item_id, source_slot, equip_slot);
    }

    /* Update in-memory equipment */
    s->equipment.equipment[equip_slot].slot = equip_slot;
    s->equipment.equipment[equip_slot].it_id = item_id;
    s->equipment.equipment[equip_slot].stat = 0;

    /* Response: result + slot + item_id */
    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;
    response[offset++] = (uint8_t)equip_slot;
    memcpy(response + offset, &item_id, 4);
    offset += 4;

    return send_response(s, 33, response, offset);
}

/* msg_no=34: UNEQUIP_ITEM */
static int handle_unequip_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] UNEQUIP_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 1) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 34, error, 4);
    }

    uint8_t equip_slot = data[0];
    printf("[GAME] UNEQUIP_ITEM: slot=%d\n", equip_slot);

    /* Validate slot */
    if (equip_slot >= MAX_EQUIP_SLOTS) {
        uint8_t error[4] = {2, 0, 0, 0};  /* Invalid slot */
        return send_response(s, 34, error, 4);
    }

    /* Get the item being unequipped */
    uint32_t item_id = s->equipment.equipment[equip_slot].it_id;

    if (item_id == 0) {
        /* No item in this slot */
        uint8_t error[4] = {3, 0, 0, 0};
        return send_response(s, 34, error, 4);
    }

    /* Find a free inventory slot */
    int dest_slot = db_find_free_slot(s->character_id, MAX_EQUIP_SLOTS);
    if (dest_slot < MAX_EQUIP_SLOTS || dest_slot >= 200) {
        printf("[UNEQUIP] No free inventory slot\n");
        uint8_t error[4] = {4, 0, 0, 0};  /* Inventory full */
        return send_response(s, 34, error, 4);
    }

    /* Move item from equipment slot to inventory */
    if (db_update_item_slot(s->character_id, equip_slot, dest_slot) < 0) {
        printf("[UNEQUIP] Failed to update database\n");
        /* Continue anyway for in-memory update */
    } else {
        printf("[UNEQUIP] Moved item %u from equip slot %d to bag slot %d\n",
               item_id, equip_slot, dest_slot);
    }

    /* Clear in-memory equipment */
    memset(&s->equipment.equipment[equip_slot], 0, sizeof(DBItemInfo));

    /* Response: result + slot + item_id + new_slot */
    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;
    response[offset++] = equip_slot;
    memcpy(response + offset, &item_id, 4);
    offset += 4;
    memcpy(response + offset, &dest_slot, 4);
    offset += 4;

    return send_response(s, 34, response, offset);
}

/* msg_no=38: GET_CHARACTER_STATS */
static int handle_get_character_stats(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] GET_CHARACTER_STATS from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 38, error, 4);
    }

    /* Recalculate combat stats based on current equipment */
    calculate_combat_stats(s);

    /* Response: result + 16 stat values (4 bytes each) */
    uint8_t response[68];  /* 4 + 16*4 = 68 */
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Copy all stats */
    CombatStats* stats = &s->combat_stats;
    memcpy(response + offset, &stats->str, 4);
    offset += 4;
    memcpy(response + offset, &stats->intel, 4);
    offset += 4;
    memcpy(response + offset, &stats->vit, 4);
    offset += 4;
    memcpy(response + offset, &stats->spr, 4);
    offset += 4;
    memcpy(response + offset, &stats->phys_attack, 4);
    offset += 4;
    memcpy(response + offset, &stats->mag_attack, 4);
    offset += 4;
    memcpy(response + offset, &stats->phys_def, 4);
    offset += 4;
    memcpy(response + offset, &stats->mag_def, 4);
    offset += 4;
    memcpy(response + offset, &stats->max_hp, 4);
    offset += 4;
    memcpy(response + offset, &stats->max_mp, 4);
    offset += 4;
    memcpy(response + offset, &stats->attack_speed, 4);
    offset += 4;
    memcpy(response + offset, &stats->move_speed, 4);
    offset += 4;
    memcpy(response + offset, &stats->cast_speed, 4);
    offset += 4;
    memcpy(response + offset, &stats->crit_rate, 4);
    offset += 4;
    memcpy(response + offset, &stats->hit_rate, 4);
    offset += 4;
    memcpy(response + offset, &stats->evasion, 4);
    offset += 4;

    printf("[GAME] Stats: STR=%u INT=%u VIT=%u SPR=%u PATK=%u MATK=%u\n",
           stats->str, stats->intel, stats->vit, stats->spr,
           stats->phys_attack, stats->mag_attack);
    printf("[GAME] Stats: HP=%u MP=%u DEF=%u/%u CRIT=%u\n",
           stats->max_hp, stats->max_mp, stats->phys_def, stats->mag_def,
           stats->crit_rate);

    return send_response(s, 38, response, offset);
}

/* msg_no=35: NPC_DIALOG */
static int handle_npc_dialog(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] NPC_DIALOG from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 35, error, 4);
    }

    uint32_t npc_id;
    memcpy(&npc_id, data, 4);
    printf("[GAME] NPC_DIALOG: npc_id=%u\n", npc_id);

    /* Response: result + dialog options */
    uint8_t response[64];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* NPC type and options */
    uint8_t npc_type = 1;  /* Shop NPC */
    response[offset++] = npc_type;

    /* Option count */
    uint8_t option_count = 3;
    response[offset++] = option_count;

    /* Options: 1=Buy, 2=Sell, 3=Close */
    response[offset++] = 1;
    response[offset++] = 2;
    response[offset++] = 3;

    return send_response(s, 35, response, offset);
}

/* msg_no=36: NPC_SHOP_BUY */
static int handle_npc_shop_buy(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] NPC_SHOP_BUY from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 8) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 36, error, 4);
    }

    uint32_t npc_id, item_id;
    uint16_t count;
    memcpy(&npc_id, data, 4);
    memcpy(&item_id, data + 4, 4);
    if (len >= 10) {
        memcpy(&count, data + 8, 2);
    } else {
        count = 1;
    }

    printf("[GAME] NPC_SHOP_BUY: npc=%u item=%u count=%u\n", npc_id, item_id, count);

    /* Simple price calculation: item_id % 10000 as base price */
    uint32_t price_per_item = (item_id % 10000) * 10;
    if (price_per_item == 0) price_per_item = 100;  /* Minimum price */
    uint32_t total_price = price_per_item * count;

    /* Check if player has enough gold */
    uint32_t current_gold = s->inventory.money;
    if (current_gold < total_price) {
        printf("[GAME] NPC_SHOP_BUY: Not enough gold! have=%u need=%u\n", current_gold, total_price);
        uint8_t error[4] = {3, 0, 0, 0};  /* Error: not enough gold */
        return send_response(s, 36, error, 4);
    }

    /* Find free inventory slot (starting after equipment slots) */
    int free_slot = db_find_free_slot(s->character_id, MAX_EQUIP_SLOTS);
    if (free_slot < 0 || free_slot >= 200) {
        printf("[GAME] NPC_SHOP_BUY: No free slot!\n");
        uint8_t error[4] = {4, 0, 0, 0};  /* Error: inventory full */
        return send_response(s, 36, error, 4);
    }

    /* Deduct gold */
    uint32_t new_gold = current_gold - total_price;
    if (db_update_gold(s->character_id, new_gold) < 0) {
        printf("[GAME] NPC_SHOP_BUY: Failed to update gold in DB!\n");
        uint8_t error[4] = {5, 0, 0, 0};  /* Error: DB error */
        return send_response(s, 36, error, 4);
    }

    /* Update in-memory gold */
    s->inventory.money = new_gold;

    /* Add item to inventory */
    int new_ui_id = db_add_item(s->character_id, item_id, free_slot, 0);
    if (new_ui_id < 0) {
        /* Rollback gold if item add fails */
        db_update_gold(s->character_id, current_gold);
        s->inventory.money = current_gold;
        printf("[GAME] NPC_SHOP_BUY: Failed to add item, gold rolled back!\n");
        uint8_t error[4] = {5, 0, 0, 0};
        return send_response(s, 36, error, 4);
    }

    printf("[GAME] NPC_SHOP_BUY: SUCCESS! item=%u, slot=%d, price=%u, new_gold=%u\n",
           item_id, free_slot, total_price, new_gold);

    /* Response: result + new gold + slot where item placed */
    uint8_t response[16];
    memset(response, 0, sizeof(response));
    uint32_t result = 0;
    memcpy(response, &result, 4);
    memcpy(response + 4, &new_gold, 4);
    uint16_t slot16 = (uint16_t)free_slot;
    memcpy(response + 8, &slot16, 2);
    memcpy(response + 10, &item_id, 4);

    /* Send UPDATE_ITEM_LIST to notify client of inventory change */
    return send_response(s, 14, response, 14);  /* NOTIPACKET_UPDATE_ITEM_LIST = 14 */
}

/* msg_no=37: NPC_SHOP_SELL */
static int handle_npc_shop_sell(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] NPC_SHOP_SELL from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 37, error, 4);
    }

    uint16_t slot, count;
    memcpy(&slot, data, 2);
    if (len >= 4) {
        memcpy(&count, data + 2, 2);
    } else {
        count = 1;
    }

    printf("[GAME] NPC_SHOP_SELL: slot=%u count=%u\n", slot, count);

    /* Get item info from session inventory */
    uint32_t item_id = 0;
    for (uint16_t i = 0; i < s->inventory.item_count; i++) {
        if (s->inventory.items[i].slot == slot) {
            item_id = s->inventory.items[i].it_id;
            break;
        }
    }

    if (item_id == 0) {
        printf("[GAME] NPC_SHOP_SELL: Item not found in slot %u\n", slot);
        uint8_t error[4] = {2, 0, 0, 0};  /* Item not found */
        return send_response(s, 37, error, 4);
    }

    /* Calculate sell price (50% of buy price) */
    uint32_t sell_price = ((item_id % 10000) * 10) / 2;
    if (sell_price == 0) sell_price = 50;
    uint32_t total_price = sell_price * count;

    /* Delete item from database */
    if (db_delete_item(s->character_id, slot) < 0) {
        printf("[GAME] NPC_SHOP_SELL: Failed to delete item from DB!\n");
        uint8_t error[4] = {5, 0, 0, 0};
        return send_response(s, 37, error, 4);
    }

    /* Add gold */
    uint32_t current_gold = s->inventory.money;
    uint32_t new_gold = current_gold + total_price;
    if (db_update_gold(s->character_id, new_gold) < 0) {
        printf("[GAME] NPC_SHOP_SELL: Failed to update gold!\n");
        /* Item already deleted, proceed anyway */
    }
    s->inventory.money = new_gold;

    /* Remove from session inventory */
    for (uint16_t i = 0; i < s->inventory.item_count; i++) {
        if (s->inventory.items[i].slot == slot) {
            /* Shift remaining items */
            for (uint16_t j = i; j < s->inventory.item_count - 1; j++) {
                s->inventory.items[j] = s->inventory.items[j + 1];
            }
            s->inventory.item_count--;
            break;
        }
    }

    printf("[GAME] NPC_SHOP_SELL: SUCCESS! item=%u sold for %u, new_gold=%u\n",
           item_id, total_price, new_gold);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    uint32_t result = 0;
    memcpy(response, &result, 4);
    memcpy(response + 4, &new_gold, 4);

    /* Send UPDATE_ITEM_LIST to notify client of inventory change */
    return send_response(s, 14, response, 12);  /* NOTIPACKET_UPDATE_ITEM_LIST = 14 */
}

/* Quest status constants */
#define QUEST_STATUS_AVAILABLE   0
#define QUEST_STATUS_ACTIVE      1
#define QUEST_STATUS_COMPLETE    2
#define QUEST_STATUS_REWARDED    3

/* msg_no=50: GET_QUEST_LIST */
static int handle_get_quest_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] GET_QUEST_LIST from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 50, error, 4);
    }

    /* Response: result(4) + active_count(1) + [quest_id(4) + progress(1)]... */
    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Active quest count */
    response[offset++] = s->active_quest_count;

    /* List active quests */
    for (int i = 0; i < s->active_quest_count && i < 10; i++) {
        memcpy(response + offset, &s->active_quests[i], 4);
        offset += 4;
        response[offset++] = s->quest_progress[i];
    }

    printf("[GAME] Quest list: %d active quests\n", s->active_quest_count);
    return send_response(s, 50, response, offset);
}

/* msg_no=51: ACCEPT_QUEST */
static int handle_accept_quest(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] ACCEPT_QUEST from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 51, error, 4);
    }

    uint32_t quest_id;
    memcpy(&quest_id, data, 4);
    printf("[GAME] Accept quest: %u\n", quest_id);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Check if quest already active */
    for (int i = 0; i < s->active_quest_count; i++) {
        if (s->active_quests[i] == quest_id) {
            uint32_t result = 2;  /* Already accepted */
            memcpy(response + offset, &result, 4);
            return send_response(s, 51, response, 4);
        }
    }

    /* Check if quest slot available */
    if (s->active_quest_count >= 10) {
        uint32_t result = 3;  /* Quest log full */
        memcpy(response + offset, &result, 4);
        return send_response(s, 51, response, 4);
    }

    /* Accept the quest */
    int idx = s->active_quest_count;
    s->active_quests[idx] = quest_id;
    s->quest_progress[idx] = 0;
    s->active_quest_count++;

    uint32_t result = 0;  /* Success */
    memcpy(response + offset, &result, 4);
    offset += 4;
    memcpy(response + offset, &quest_id, 4);
    offset += 4;
    response[offset++] = 0;  /* Initial progress */

    printf("[GAME] Quest %u accepted (slot %d)\n", quest_id, idx);
    return send_response(s, 51, response, offset);
}

/* msg_no=52: COMPLETE_QUEST */
static int handle_complete_quest(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] COMPLETE_QUEST from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 52, error, 4);
    }

    uint32_t quest_id;
    memcpy(&quest_id, data, 4);
    printf("[GAME] Complete quest: %u\n", quest_id);

    uint8_t response[32];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Find the quest */
    int found_idx = -1;
    for (int i = 0; i < s->active_quest_count; i++) {
        if (s->active_quests[i] == quest_id) {
            found_idx = i;
            break;
        }
    }

    if (found_idx < 0) {
        uint32_t result = 2;  /* Quest not found */
        memcpy(response + offset, &result, 4);
        return send_response(s, 52, response, 4);
    }

    /* Mark as complete and give rewards */
    uint32_t exp_reward = (quest_id % 100) * 100 + 500;
    uint32_t gold_reward = (quest_id % 100) * 50 + 200;

    /* TODO: Add rewards via DB (currently disabled for testing)
    if (db_add_exp(s->character_id, exp_reward) >= 0) {
        for (int i = 0; i < s->character_count; i++) {
            if (s->characters[i].charac_no == s->character_id) {
                s->characters[i].exp += exp_reward;
                break;
            }
        }
    }
    if (db_add_gold(s->character_id, gold_reward) >= 0) {
        s->inventory.money += gold_reward;
    }
    */
    /* For now just add locally */
    s->inventory.money += gold_reward;

    /* Remove from active, add to completed */
    if (s->completed_quest_count < 64) {
        s->completed_quests[s->completed_quest_count++] = quest_id;
    }

    /* Shift active quests down */
    for (int i = found_idx; i < s->active_quest_count - 1; i++) {
        s->active_quests[i] = s->active_quests[i + 1];
        s->quest_progress[i] = s->quest_progress[i + 1];
    }
    s->active_quest_count--;

    uint32_t result = 0;  /* Success */
    memcpy(response + offset, &result, 4);
    offset += 4;
    memcpy(response + offset, &quest_id, 4);
    offset += 4;
    memcpy(response + offset, &exp_reward, 4);
    offset += 4;
    memcpy(response + offset, &gold_reward, 4);
    offset += 4;

    printf("[GAME] Quest %u completed! Rewards: EXP=%u, Gold=%u\n",
           quest_id, exp_reward, gold_reward);
    return send_response(s, 52, response, offset);
}

/* msg_no=53: UPDATE_QUEST_PROGRESS */
static int handle_update_quest_progress(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] UPDATE_QUEST_PROGRESS from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 53, error, 4);
    }

    uint32_t quest_id;
    uint8_t progress;
    memcpy(&quest_id, data, 4);
    progress = data[4];

    printf("[GAME] Update quest %u progress to %u%%\n", quest_id, progress);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Find and update quest */
    int found = 0;
    for (int i = 0; i < s->active_quest_count; i++) {
        if (s->active_quests[i] == quest_id) {
            s->quest_progress[i] = progress;
            found = 1;
            break;
        }
    }

    uint32_t result = found ? 0 : 2;
    memcpy(response + offset, &result, 4);
    offset += 4;
    memcpy(response + offset, &quest_id, 4);
    offset += 4;
    response[offset++] = progress;

    return send_response(s, 53, response, offset);
}

/*==============================================================================
 * Dungeon System Handlers
 *============================================================================*/

/*------------------------------------------------------------------------------
 * Shared Party Dungeon Helper Functions
 *----------------------------------------------------------------------------*/

/* Find shared dungeon by party_id */
static SharedDungeonInstance* find_shared_dungeon_by_party(uint32_t party_id) {
    for (int i = 0; i < MAX_SHARED_DUNGEONS; i++) {
        if (g_shared_dungeons[i].active && g_shared_dungeons[i].party_id == party_id) {
            return &g_shared_dungeons[i];
        }
    }
    return NULL;
}

/* Find shared dungeon by ID */
static SharedDungeonInstance* find_shared_dungeon(uint32_t shared_id) {
    for (int i = 0; i < MAX_SHARED_DUNGEONS; i++) {
        if (g_shared_dungeons[i].active && g_shared_dungeons[i].shared_id == shared_id) {
            return &g_shared_dungeons[i];
        }
    }
    return NULL;
}

/* Create a new shared dungeon instance for party */
static SharedDungeonInstance* create_shared_dungeon(uint32_t party_id, uint32_t dungeon_id, uint8_t difficulty) {
    for (int i = 0; i < MAX_SHARED_DUNGEONS; i++) {
        if (!g_shared_dungeons[i].active) {
            SharedDungeonInstance* sd = &g_shared_dungeons[i];
            memset(sd, 0, sizeof(SharedDungeonInstance));
            sd->shared_id = g_next_shared_dungeon_id++;
            sd->party_id = party_id;
            sd->dungeon_id = dungeon_id;
            sd->difficulty = difficulty;
            sd->current_room = 0;
            sd->total_rooms = 3 + difficulty;
            sd->active = 1;
            sd->start_time = (uint32_t)time(NULL);
            sd->time_limit = 600;  /* 10 minutes */
            sd->monster_count = 0;
            sd->monsters_killed = 0;
            sd->member_count = 0;
            printf("[PARTY_DUNGEON] Created shared dungeon %u for party %u (dungeon=%u diff=%d)\n",
                   sd->shared_id, party_id, dungeon_id, difficulty);
            return sd;
        }
    }
    printf("[PARTY_DUNGEON] ERROR: No free shared dungeon slots!\n");
    return NULL;
}

/* Add member to shared dungeon */
static int shared_dungeon_add_member(SharedDungeonInstance* sd, int fd) {
    if (!sd || sd->member_count >= MAX_PARTY_MEMBERS_DUNGEON) return -1;

    /* Check not already added */
    for (int i = 0; i < (int)sd->member_count; i++) {
        if (sd->member_fds[i] == (uint32_t)fd) return -1;
    }

    sd->member_fds[sd->member_count++] = (uint32_t)fd;
    printf("[PARTY_DUNGEON] Added member fd=%d to shared dungeon %u (members=%d)\n",
           fd, sd->shared_id, sd->member_count);
    return 0;
}

/* Remove member from shared dungeon */
static int shared_dungeon_remove_member(SharedDungeonInstance* sd, int fd) {
    if (!sd) return -1;

    int found = -1;
    for (int i = 0; i < (int)sd->member_count; i++) {
        if (sd->member_fds[i] == (uint32_t)fd) {
            found = i;
            break;
        }
    }

    if (found < 0) return -1;

    /* Shift remaining members */
    for (int i = found; i < (int)sd->member_count - 1; i++) {
        sd->member_fds[i] = sd->member_fds[i + 1];
    }
    sd->member_count--;

    printf("[PARTY_DUNGEON] Removed member fd=%d from shared dungeon %u (members=%d)\n",
           fd, sd->shared_id, sd->member_count);

    /* If no members left, deactivate shared dungeon */
    if (sd->member_count == 0) {
        printf("[PARTY_DUNGEON] Shared dungeon %u closed (no members)\n", sd->shared_id);
        sd->active = 0;
    }

    return 0;
}

/* Broadcast dungeon update to all party members in dungeon */
static void shared_dungeon_broadcast(SharedDungeonInstance* sd, uint8_t msg_no,
                                      const uint8_t* data, int len) {
    if (!sd) return;

    for (int i = 0; i < (int)sd->member_count; i++) {
        int fd = (int)sd->member_fds[i];
        /* Find session by fd */
        for (int j = 0; j < MAX_SESSIONS; j++) {
            if (g_sessions[j].active && g_sessions[j].fd == fd) {
                send_response(&g_sessions[j], msg_no, data, len);
                break;
            }
        }
    }
}

/* Broadcast dungeon update to all party members except sender */
static void shared_dungeon_broadcast_except(SharedDungeonInstance* sd, int except_fd,
                                             uint8_t msg_no, const uint8_t* data, int len) {
    if (!sd) return;

    for (int i = 0; i < (int)sd->member_count; i++) {
        int fd = (int)sd->member_fds[i];
        if (fd == except_fd) continue;  /* Skip sender */

        for (int j = 0; j < MAX_SESSIONS; j++) {
            if (g_sessions[j].active && g_sessions[j].fd == fd) {
                send_response(&g_sessions[j], msg_no, data, len);
                break;
            }
        }
    }
}

/* Spawn monsters for shared dungeon */
static void shared_dungeon_spawn_monsters(SharedDungeonInstance* sd, int room);

/* Sync local dungeon state from shared instance */
static void sync_dungeon_from_shared(DungeonInstance* local, SharedDungeonInstance* sd) {
    if (!local || !sd) return;

    local->dungeon_id = sd->dungeon_id;
    local->difficulty = sd->difficulty;
    local->current_room = sd->current_room;
    local->total_rooms = sd->total_rooms;
    local->active = sd->active;
    local->start_time = sd->start_time;
    local->time_limit = sd->time_limit;
    local->monster_count = sd->monster_count;
    local->monsters_killed = sd->monsters_killed;

    /* Copy monster data */
    memcpy(local->monsters, sd->monsters, sizeof(MonsterInfo) * MAX_MONSTERS);
}

/* Sync shared instance from local dungeon state (after attack) */
static void sync_shared_from_dungeon(SharedDungeonInstance* sd, DungeonInstance* local) {
    if (!sd || !local) return;

    sd->current_room = local->current_room;
    sd->monster_count = local->monster_count;
    sd->monsters_killed = local->monsters_killed;

    /* Copy monster data */
    memcpy(sd->monsters, local->monsters, sizeof(MonsterInfo) * MAX_MONSTERS);
}

/* Helper: Spawn monsters for a dungeon room */
static void dungeon_spawn_monsters(DungeonInstance* d, int room) {
    d->monster_count = 0;

    /* Spawn 3-5 monsters per room based on difficulty */
    int base_count = 3 + d->difficulty;
    if (base_count > MAX_MONSTERS) base_count = MAX_MONSTERS;

    uint32_t current_time = (uint32_t)time(NULL);

    for (int i = 0; i < base_count; i++) {
        MonsterInfo* m = &d->monsters[i];
        m->monster_id = (room * 100) + i + 1;
        m->monster_type = 1000 + (d->dungeon_id % 10) * 100 + (i % 5);
        m->x = 100 + (i * 150) + (rand() % 50);
        m->y = 200 + (rand() % 100);
        m->max_hp = 1000 * (d->difficulty + 1) * (room + 1);
        m->hp = m->max_hp;
        m->alive = 1;

        /* Initialize AI fields */
        m->state = MONSTER_STATE_IDLE;
        m->ai_type = (i == base_count - 1 && room == d->total_rooms - 1) 
                     ? MONSTER_AI_BOSS : MONSTER_AI_NORMAL;
        m->spawn_x = m->x;
        m->spawn_y = m->y;
        m->target_x = m->x;
        m->target_y = m->y;
        m->target_player = 0;
        m->last_action_time = current_time;
        m->attack_cooldown = 0;
        m->move_speed = (m->ai_type == MONSTER_AI_BOSS) ? 3 : 5;
        m->aggro_range = (m->ai_type == MONSTER_AI_BOSS) ? 400 : 250;
        m->attack_range = (m->ai_type == MONSTER_AI_BOSS) ? 80 : 50;
        m->attack_damage = 100 * (d->difficulty + 1) * (room + 1);
        if (m->ai_type == MONSTER_AI_BOSS) {
            m->attack_damage *= 2;
            m->max_hp *= 5;
            m->hp = m->max_hp;
        }
        m->hit_stun_frames = 0;
        d->monster_count++;
    }

    printf("[DUNGEON] Spawned %d monsters in room %d (boss=%d)\n", d->monster_count, room, (room == d->total_rooms - 1) ? 1 : 0);
}

/* Spawn monsters for shared dungeon (uses same logic as dungeon_spawn_monsters) */
static void shared_dungeon_spawn_monsters(SharedDungeonInstance* sd, int room) {
    sd->monster_count = 0;

    int base_count = 3 + sd->difficulty;
    if (base_count > MAX_MONSTERS) base_count = MAX_MONSTERS;

    uint32_t current_time = (uint32_t)time(NULL);

    for (int i = 0; i < base_count; i++) {
        MonsterInfo* m = &sd->monsters[i];
        m->monster_id = (room * 100) + i + 1;
        m->monster_type = 1000 + (sd->dungeon_id % 10) * 100 + (i % 5);
        m->x = 100 + (i * 150) + (rand() % 50);
        m->y = 200 + (rand() % 100);
        /* Scale HP for party size (more members = more HP) */
        m->max_hp = 1000 * (sd->difficulty + 1) * (room + 1) * (1 + sd->member_count / 2);
        m->hp = m->max_hp;
        m->alive = 1;

        m->state = MONSTER_STATE_IDLE;
        m->ai_type = (i == base_count - 1 && room == (int)sd->total_rooms - 1)
                     ? MONSTER_AI_BOSS : MONSTER_AI_NORMAL;
        m->spawn_x = m->x;
        m->spawn_y = m->y;
        m->target_x = m->x;
        m->target_y = m->y;
        m->target_player = 0;
        m->last_action_time = current_time;
        m->attack_cooldown = 0;
        m->move_speed = (m->ai_type == MONSTER_AI_BOSS) ? 3 : 5;
        m->aggro_range = (m->ai_type == MONSTER_AI_BOSS) ? 400 : 250;
        m->attack_range = (m->ai_type == MONSTER_AI_BOSS) ? 80 : 50;
        m->attack_damage = 100 * (sd->difficulty + 1) * (room + 1);
        if (m->ai_type == MONSTER_AI_BOSS) {
            m->attack_damage *= 2;
            m->max_hp *= 5;
            m->hp = m->max_hp;
        }
        m->hit_stun_frames = 0;
        sd->monster_count++;
    }

    printf("[PARTY_DUNGEON] Shared dungeon %u: Spawned %d monsters in room %d (members=%d)\n",
           sd->shared_id, sd->monster_count, room, sd->member_count);
}

/* Calculate distance between two points */
static uint32_t monster_distance(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    int dx = (int)x1 - (int)x2;
    int dy = (int)y1 - (int)y2;
    /* Simple manhattan distance for performance */
    return (uint32_t)(abs(dx) + abs(dy));
}

/* Move monster towards target */
static void monster_move_towards(MonsterInfo* m, uint16_t tx, uint16_t ty) {
    int dx = (int)tx - (int)m->x;
    int dy = (int)ty - (int)m->y;
    
    /* Normalize movement by speed */
    if (abs(dx) > m->move_speed) {
        dx = (dx > 0) ? m->move_speed : -m->move_speed;
    }
    if (abs(dy) > m->move_speed) {
        dy = (dy > 0) ? m->move_speed : -m->move_speed;
    }
    
    m->x = (uint16_t)((int)m->x + dx);
    m->y = (uint16_t)((int)m->y + dy);
}

/* Update single monster AI (called per tick) */
static int monster_update_ai(MonsterInfo* m, uint16_t player_x, uint16_t player_y, 
                             uint32_t player_charac_no, uint32_t current_time) {
    if (!m->alive) return 0;
    
    uint32_t dist_to_player = monster_distance(m->x, m->y, player_x, player_y);
    int action_taken = 0;
    
    /* Reduce attack cooldown */
    if (m->attack_cooldown > 0) m->attack_cooldown--;
    
    /* Reduce hit stun */
    if (m->hit_stun_frames > 0) {
        m->hit_stun_frames--;
        m->state = MONSTER_STATE_HIT_STUN;
        return 0;  /* Can't act while stunned */
    }
    
    switch (m->state) {
        case MONSTER_STATE_IDLE:
            /* Check if player in aggro range */
            if (dist_to_player <= m->aggro_range) {
                m->state = MONSTER_STATE_CHASE;
                m->target_player = player_charac_no;
                m->target_x = player_x;
                m->target_y = player_y;
            }
            break;
            
        case MONSTER_STATE_CHASE:
            /* Update target position */
            m->target_x = player_x;
            m->target_y = player_y;
            
            /* Check if in attack range */
            if (dist_to_player <= m->attack_range && m->attack_cooldown == 0) {
                m->state = MONSTER_STATE_ATTACK;
            } else if (dist_to_player > m->aggro_range * 2) {
                /* Lost target, return to spawn */
                m->state = MONSTER_STATE_PATROL;
                m->target_player = 0;
            } else {
                /* Move towards player */
                monster_move_towards(m, player_x, player_y);
                action_taken = 1;
            }
            break;
            
        case MONSTER_STATE_ATTACK:
            if (m->attack_cooldown == 0 && dist_to_player <= m->attack_range) {
                /* Attack ready - set cooldown (30 ticks ~= 1 second at 30fps) */
                m->attack_cooldown = 30;
                action_taken = 2;  /* 2 = attack action */
            } else {
                /* Return to chase */
                m->state = MONSTER_STATE_CHASE;
            }
            break;
            
        case MONSTER_STATE_PATROL:
            /* Return to spawn point */
            if (monster_distance(m->x, m->y, m->spawn_x, m->spawn_y) > 10) {
                monster_move_towards(m, m->spawn_x, m->spawn_y);
                action_taken = 1;
            } else {
                m->state = MONSTER_STATE_IDLE;
            }
            /* Re-check for player */
            if (dist_to_player <= m->aggro_range) {
                m->state = MONSTER_STATE_CHASE;
                m->target_player = player_charac_no;
            }
            break;
            
        case MONSTER_STATE_HIT_STUN:
            /* Wait for stun to end */
            break;
            
        case MONSTER_STATE_DEAD:
            /* Do nothing */
            break;
    }
    
    m->last_action_time = current_time;
    return action_taken;
}

/* Apply hit stun to monster when damaged */
static void monster_apply_hit_stun(MonsterInfo* m, uint8_t stun_frames) {
    if (m->alive) {
        m->hit_stun_frames = stun_frames;
        m->state = MONSTER_STATE_HIT_STUN;
    }
}

/* Process all monster AI for a dungeon tick */
static int dungeon_tick_monsters(DungeonInstance* d, uint16_t player_x, uint16_t player_y,
                                  uint32_t player_charac_no, uint8_t* attack_flags) {
    uint32_t current_time = (uint32_t)time(NULL);
    int attacks_pending = 0;
    
    for (int i = 0; i < d->monster_count; i++) {
        int result = monster_update_ai(&d->monsters[i], player_x, player_y, 
                                       player_charac_no, current_time);
        if (result == 2) {
            /* Monster wants to attack */
            if (attack_flags && attacks_pending < 8) {
                attack_flags[attacks_pending] = (uint8_t)i;
                attacks_pending++;
            }
        }
    }
    
    return attacks_pending;
}

/* Helper: Check if room is cleared */
static int dungeon_room_cleared(DungeonInstance* d) {
    for (int i = 0; i < d->monster_count; i++) {
        if (d->monsters[i].alive) {
            return 0;
        }
    }
    return 1;
}

/* msg_no=60: DUNGEON_ENTER */
static int handle_dungeon_enter(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DUNGEON_ENTER from fd=%d party_id=%u\n", s->fd, s->party_id);

    if (s->state < STATE_IN_GAME || len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 60, error, 4);
    }

    uint32_t dungeon_id;
    uint8_t difficulty;
    memcpy(&dungeon_id, data, 4);
    difficulty = data[4];

    printf("[DUNGEON] Enter: dungeon_id=%u difficulty=%d\n", dungeon_id, difficulty);

    /* Check fatigue (simplified) */
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            if (s->characters[i].fatigue <= 0) {
                printf("[DUNGEON] No fatigue remaining\n");
                uint8_t error[4] = {5, 0, 0, 0};  /* No fatigue */
                return send_response(s, 60, error, 4);
            }
            /* Consume fatigue */
            s->characters[i].fatigue -= 8;
            if (s->characters[i].fatigue < 0) {
                s->characters[i].fatigue = 0;
            }
            break;
        }
    }

    /* Initialize local dungeon instance */
    DungeonInstance* d = &s->dungeon;
    memset(d, 0, sizeof(DungeonInstance));

    SharedDungeonInstance* sd = NULL;

    /* Check if player is in a party */
    if (s->party_id > 0) {
        PartyInfo* party = find_party(s->party_id);
        if (party && party->active) {
            /* Check if party already has a shared dungeon */
            sd = find_shared_dungeon_by_party(s->party_id);

            if (sd == NULL) {
                /* Party leader creates the shared dungeon */
                if (party->leader_id == s->character_id) {
                    sd = create_shared_dungeon(s->party_id, dungeon_id, difficulty);
                    if (sd == NULL) {
                        printf("[PARTY_DUNGEON] Failed to create shared dungeon\n");
                        uint8_t error[4] = {6, 0, 0, 0};  /* Server error */
                        return send_response(s, 60, error, 4);
                    }
                    /* Spawn monsters for shared dungeon */
                    shared_dungeon_add_member(sd, s->fd);
                    shared_dungeon_spawn_monsters(sd, 0);

                    /* Mark as host */
                    d->is_party_host = 1;
                    printf("[PARTY_DUNGEON] Leader created shared dungeon %u\n", sd->shared_id);
                } else {
                    /* Non-leader trying to enter without leader */
                    printf("[PARTY_DUNGEON] ERROR: Leader must enter first!\n");
                    uint8_t error[4] = {7, 0, 0, 0};  /* Party leader must enter first */
                    return send_response(s, 60, error, 4);
                }
            } else {
                /* Join existing shared dungeon */
                if (sd->dungeon_id != dungeon_id) {
                    printf("[PARTY_DUNGEON] ERROR: Dungeon mismatch (requested %u, party in %u)\n",
                           dungeon_id, sd->dungeon_id);
                    uint8_t error[4] = {8, 0, 0, 0};  /* Wrong dungeon */
                    return send_response(s, 60, error, 4);
                }
                shared_dungeon_add_member(sd, s->fd);
                d->is_party_host = 0;
                printf("[PARTY_DUNGEON] Member joined shared dungeon %u\n", sd->shared_id);
            }

            /* Store shared dungeon reference */
            d->party_id = s->party_id;
            d->shared_dungeon_id = sd->shared_id;

            /* Sync local dungeon from shared instance */
            sync_dungeon_from_shared(d, sd);

            /* Notify other party members that someone joined */
            uint8_t join_notify[16];
            memset(join_notify, 0, sizeof(join_notify));
            uint32_t member_id = s->character_id;
            memcpy(join_notify, &member_id, 4);
            join_notify[4] = 1;  /* Join notification */
            shared_dungeon_broadcast_except(sd, s->fd, 68, join_notify, 8);  /* msg_no=68: PARTY_DUNGEON_MEMBER_UPDATE */

            printf("[PARTY_DUNGEON] Entered shared dungeon, members=%d\n", sd->member_count);
        } else {
            /* Party not found, fall back to solo */
            printf("[DUNGEON] Party not found, entering solo\n");
            s->party_id = 0;
        }
    }

    /* Solo dungeon (no party or party not found) */
    if (s->party_id == 0) {
        d->dungeon_id = dungeon_id;
        d->difficulty = difficulty;
        d->current_room = 0;
        d->total_rooms = 3 + difficulty;
        d->active = 1;
        d->start_time = (uint32_t)time(NULL);
        d->time_limit = 600;
        d->monsters_killed = 0;
        d->party_id = 0;
        d->shared_dungeon_id = 0;
        d->is_party_host = 0;

        dungeon_spawn_monsters(d, 0);
    }

    s->state = STATE_IN_DUNGEON;

    /* Build response */
    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &d->dungeon_id, 4);
    offset += 4;

    response[offset++] = d->difficulty;
    response[offset++] = d->total_rooms;
    response[offset++] = d->current_room;

    /* Party info */
    response[offset++] = (d->party_id > 0) ? 1 : 0;  /* is_party_dungeon */
    if (d->party_id > 0 && sd) {
        response[offset++] = sd->member_count;
    } else {
        response[offset++] = 1;  /* Solo = 1 member */
    }

    /* Monster count for this room */
    uint8_t mon_count = (uint8_t)d->monster_count;
    response[offset++] = mon_count;

    /* Monster list: id(4) + type(2) + x(2) + y(2) + hp(4) = 14 bytes each */
    for (int i = 0; i < d->monster_count && offset < 240; i++) {
        MonsterInfo* m = &d->monsters[i];
        memcpy(response + offset, &m->monster_id, 4);
        offset += 4;
        memcpy(response + offset, &m->monster_type, 2);
        offset += 2;
        memcpy(response + offset, &m->x, 2);
        offset += 2;
        memcpy(response + offset, &m->y, 2);
        offset += 2;
        memcpy(response + offset, &m->hp, 4);
        offset += 4;
    }

    printf("[DUNGEON] Entered dungeon %u room 0/%d with %d monsters (party=%u)\n",
           d->dungeon_id, d->total_rooms, d->monster_count, d->party_id);

    return send_response(s, 60, response, offset);
}

/* msg_no=61: DUNGEON_EXIT */
static int handle_dungeon_exit(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] DUNGEON_EXIT from fd=%d\n", s->fd);

    if (s->state != STATE_IN_DUNGEON) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 61, error, 4);
    }

    DungeonInstance* d = &s->dungeon;

    /* Handle party dungeon exit */
    if (d->party_id > 0 && d->shared_dungeon_id > 0) {
        SharedDungeonInstance* sd = find_shared_dungeon(d->shared_dungeon_id);
        if (sd) {
            /* Notify other party members */
            uint8_t exit_notify[16];
            memset(exit_notify, 0, sizeof(exit_notify));
            uint32_t member_id = s->character_id;
            memcpy(exit_notify, &member_id, 4);
            exit_notify[4] = 0;  /* Leave notification */
            shared_dungeon_broadcast_except(sd, s->fd, 68, exit_notify, 8);

            /* Remove from shared dungeon */
            shared_dungeon_remove_member(sd, s->fd);

            printf("[PARTY_DUNGEON] Member fd=%d left shared dungeon %u (remaining=%d)\n",
                   s->fd, sd->shared_id, sd->member_count);
        }
    }

    /* Reset dungeon state */
    d->active = 0;
    d->party_id = 0;
    d->shared_dungeon_id = 0;
    s->state = STATE_IN_GAME;

    printf("[DUNGEON] Exited dungeon (killed %d monsters)\n", d->monsters_killed);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    uint32_t result = 0;
    memcpy(response, &result, 4);
    memcpy(response + 4, &d->monsters_killed, 4);

    return send_response(s, 61, response, 8);
}

/* msg_no=62: DUNGEON_INFO (get current dungeon state) */
static int handle_dungeon_info(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] DUNGEON_INFO from fd=%d\n", s->fd);

    uint8_t response[64];
    memset(response, 0, sizeof(response));
    int offset = 0;

    if (s->state != STATE_IN_DUNGEON || !s->dungeon.active) {
        uint32_t result = 1;  /* Not in dungeon */
        memcpy(response, &result, 4);
        return send_response(s, 62, response, 4);
    }

    DungeonInstance* d = &s->dungeon;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &d->dungeon_id, 4);
    offset += 4;

    response[offset++] = d->difficulty;
    response[offset++] = d->current_room;
    response[offset++] = d->total_rooms;

    /* Time remaining */
    uint32_t elapsed = (uint32_t)time(NULL) - d->start_time;
    uint32_t remaining = (elapsed < d->time_limit) ? (d->time_limit - elapsed) : 0;
    memcpy(response + offset, &remaining, 4);
    offset += 4;

    /* Monsters alive in current room */
    uint8_t alive_count = 0;
    for (int i = 0; i < d->monster_count; i++) {
        if (d->monsters[i].alive) alive_count++;
    }
    response[offset++] = alive_count;

    memcpy(response + offset, &d->monsters_killed, 4);
    offset += 4;

    return send_response(s, 62, response, offset);
}

/* msg_no=65: DUNGEON_ATTACK (attack a monster with skill damage calculation) */
static int handle_dungeon_attack(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DUNGEON_ATTACK from fd=%d\n", s->fd);

    if (s->state != STATE_IN_DUNGEON || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 65, error, 4);
    }

    uint32_t monster_id;
    uint16_t skill_id = 100;  /* Default: normal attack */
    uint8_t skill_level = 1;
    memcpy(&monster_id, data, 4);

    /* Parse skill_id if provided (offset 4-5) */
    if (len >= 6) {
        memcpy(&skill_id, data + 4, 2);
    }
    /* Parse skill_level if provided (offset 6) */
    if (len >= 7) {
        skill_level = data[6];
    }

    DungeonInstance* d = &s->dungeon;
    SharedDungeonInstance* sd = NULL;

    /* For party dungeons, operate on shared instance */
    if (d->party_id > 0 && d->shared_dungeon_id > 0) {
        sd = find_shared_dungeon(d->shared_dungeon_id);
        if (sd) {
            /* Sync local from shared before finding monster */
            sync_dungeon_from_shared(d, sd);
        }
    }

    /* Find monster */
    MonsterInfo* target = NULL;
    for (int i = 0; i < d->monster_count; i++) {
        if (d->monsters[i].monster_id == monster_id && d->monsters[i].alive) {
            target = &d->monsters[i];
            break;
        }
    }

    if (!target) {
        printf("[DUNGEON] Monster %u not found or dead\n", monster_id);
        uint8_t error[4] = {2, 0, 0, 0};  /* Monster not found */
        return send_response(s, 65, error, 4);
    }

    /* Get character job for skill calculation */
    uint8_t job = 0;
    for (int i = 0; i < s->character_count; i++) {
        if (s->characters[i].charac_no == s->character_id) {
            job = s->characters[i].job;
            break;
        }
    }

    /* Calculate damage using skill formula */
    uint32_t monster_def = 50 + d->difficulty * 20;  /* Monster defense scales with difficulty */
    uint8_t is_crit = 0;
    uint32_t damage = calculate_skill_damage(&s->combat_stats, skill_id, skill_level, job, monster_def, &is_crit);

    /* Get hit count for multi-hit skills */
    uint8_t hit_count = get_skill_hit_count(skill_id);
    uint32_t total_damage = damage * hit_count;

    printf("[DUNGEON] Skill %u (lv%d) -> Monster %u: %u damage x %d hits = %u total%s\n",
           skill_id, skill_level, monster_id, damage, hit_count, total_damage,
           is_crit ? " (CRIT!)" : "");

    /* Apply damage with hit stun */
    uint8_t killed = 0;
    if (total_damage >= target->hp) {
        target->hp = 0;
        target->alive = 0;
        target->state = MONSTER_STATE_DEAD;
        killed = 1;
        d->monsters_killed++;
        printf("[DUNGEON] Monster %u killed! Total kills: %d\n", monster_id, d->monsters_killed);
    } else {
        target->hp -= total_damage;
        /* Apply hit stun based on damage dealt */
        uint8_t stun_frames = (uint8_t)((total_damage / 100) + 3);
        if (stun_frames > 30) stun_frames = 30;
        monster_apply_hit_stun(target, stun_frames);
        printf("[DUNGEON] Monster %u took %u damage, HP: %u/%u, stun: %d frames\n",
               monster_id, total_damage, target->hp, target->max_hp, stun_frames);
    }

    /* Check if room is cleared */
    uint8_t room_cleared = dungeon_room_cleared(d) ? 1 : 0;

    /* Build response */
    uint8_t response[32];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &monster_id, 4);
    offset += 4;

    memcpy(response + offset, &total_damage, 4);
    offset += 4;

    memcpy(response + offset, &target->hp, 4);
    offset += 4;

    response[offset++] = killed;
    response[offset++] = is_crit;
    response[offset++] = hit_count;
    response[offset++] = room_cleared;

    /* Add attacker ID for party sync */
    memcpy(response + offset, &s->character_id, 4);
    offset += 4;

    /* For party dungeons, sync state and broadcast to other members */
    if (sd) {
        /* Sync shared from local (after damage applied) */
        sync_shared_from_dungeon(sd, d);

        /* Broadcast damage update to other party members (msg_no=69: PARTY_DUNGEON_ATTACK_SYNC) */
        shared_dungeon_broadcast_except(sd, s->fd, 69, response, offset);

        /* Sync updated monster state to all party sessions */
        for (int i = 0; i < (int)sd->member_count; i++) {
            int member_fd = (int)sd->member_fds[i];
            if (member_fd == s->fd) continue;  /* Skip attacker */

            for (int j = 0; j < MAX_SESSIONS; j++) {
                if (g_sessions[j].active && g_sessions[j].fd == member_fd) {
                    /* Sync their local dungeon from shared */
                    sync_dungeon_from_shared(&g_sessions[j].dungeon, sd);
                    break;
                }
            }
        }

        printf("[PARTY_DUNGEON] Synced attack to %d other members\n", sd->member_count - 1);
    }

    return send_response(s, 65, response, offset);
}

/* msg_no=66: DUNGEON_NEXT_ROOM (move to next room) */
static int handle_dungeon_next_room(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] DUNGEON_NEXT_ROOM from fd=%d\n", s->fd);

    if (s->state != STATE_IN_DUNGEON) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 66, error, 4);
    }

    DungeonInstance* d = &s->dungeon;
    SharedDungeonInstance* sd = NULL;

    /* For party dungeons, sync from shared instance first */
    if (d->party_id > 0 && d->shared_dungeon_id > 0) {
        sd = find_shared_dungeon(d->shared_dungeon_id);
        if (sd) {
            sync_dungeon_from_shared(d, sd);
        }
    }

    /* Check if current room is cleared */
    if (!dungeon_room_cleared(d)) {
        printf("[DUNGEON] Room not cleared yet\n");
        uint8_t error[4] = {2, 0, 0, 0};  /* Room not cleared */
        return send_response(s, 66, error, 4);
    }

    /* Check if this was the last room */
    if (d->current_room >= d->total_rooms - 1) {
        printf("[DUNGEON] Already at final room\n");
        uint8_t error[4] = {3, 0, 0, 0};  /* Already at final room */
        return send_response(s, 66, error, 4);
    }

    /* For party dungeons, only leader can advance room */
    if (sd && !d->is_party_host) {
        printf("[PARTY_DUNGEON] Only party leader can advance room\n");
        uint8_t error[4] = {4, 0, 0, 0};  /* Not party leader */
        return send_response(s, 66, error, 4);
    }

    /* Move to next room */
    d->current_room++;

    /* For party dungeons, spawn on shared instance and sync */
    if (sd) {
        sd->current_room = d->current_room;
        shared_dungeon_spawn_monsters(sd, sd->current_room);
        sync_dungeon_from_shared(d, sd);

        printf("[PARTY_DUNGEON] Advanced to room %d/%d with %d monsters\n",
               sd->current_room, sd->total_rooms, sd->monster_count);
    } else {
        dungeon_spawn_monsters(d, d->current_room);
        printf("[DUNGEON] Moved to room %d/%d with %d monsters\n",
               d->current_room, d->total_rooms, d->monster_count);
    }

    /* Build response */
    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    response[offset++] = d->current_room;
    response[offset++] = d->total_rooms;

    uint8_t mon_count = (uint8_t)d->monster_count;
    response[offset++] = mon_count;

    /* Monster list */
    for (int i = 0; i < d->monster_count && offset < 240; i++) {
        MonsterInfo* m = &d->monsters[i];
        memcpy(response + offset, &m->monster_id, 4);
        offset += 4;
        memcpy(response + offset, &m->monster_type, 2);
        offset += 2;
        memcpy(response + offset, &m->x, 2);
        offset += 2;
        memcpy(response + offset, &m->y, 2);
        offset += 2;
        memcpy(response + offset, &m->hp, 4);
        offset += 4;
    }

    /* For party dungeons, broadcast room change to all members and sync their state */
    if (sd) {
        /* Broadcast to other members (msg_no=70 but with result=100 to indicate room change) */
        uint8_t room_notify[8];
        memset(room_notify, 0, sizeof(room_notify));
        room_notify[0] = d->current_room;
        room_notify[1] = d->total_rooms;
        room_notify[2] = mon_count;
        shared_dungeon_broadcast_except(sd, s->fd, 67, room_notify, 8);  /* msg_no=67: PARTY_DUNGEON_ROOM_CHANGE */

        /* Sync all party members */
        for (int i = 0; i < (int)sd->member_count; i++) {
            int member_fd = (int)sd->member_fds[i];
            if (member_fd == s->fd) continue;

            for (int j = 0; j < MAX_SESSIONS; j++) {
                if (g_sessions[j].active && g_sessions[j].fd == member_fd) {
                    sync_dungeon_from_shared(&g_sessions[j].dungeon, sd);
                    /* Send same response to party members */
                    send_response(&g_sessions[j], 66, response, offset);
                    break;
                }
            }
        }
    }

    return send_response(s, 66, response, offset);
}

/* msg_no=70: DUNGEON_CLEAR (clear final room) */
static int handle_dungeon_clear(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] DUNGEON_CLEAR from fd=%d\n", s->fd);

    if (s->state != STATE_IN_DUNGEON) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 70, error, 4);
    }

    DungeonInstance* d = &s->dungeon;

    /* Check if final room and cleared */
    if (d->current_room < d->total_rooms - 1) {
        uint8_t error[4] = {2, 0, 0, 0};  /* Not at final room */
        return send_response(s, 70, error, 4);
    }

    if (!dungeon_room_cleared(d)) {
        uint8_t error[4] = {3, 0, 0, 0};  /* Room not cleared */
        return send_response(s, 70, error, 4);
    }

    /* Calculate clear time */
    uint32_t clear_time = (uint32_t)time(NULL) - d->start_time;

    /* Calculate rank (S, A, B, C, D based on time and difficulty) */
    uint8_t rank;
    uint32_t base_time = 180 * (d->difficulty + 1);  /* Base time for S rank */
    if (clear_time <= base_time) {
        rank = 0;  /* S */
    } else if (clear_time <= base_time * 1.5) {
        rank = 1;  /* A */
    } else if (clear_time <= base_time * 2) {
        rank = 2;  /* B */
    } else if (clear_time <= base_time * 3) {
        rank = 3;  /* C */
    } else {
        rank = 4;  /* D */
    }

    printf("[DUNGEON] Cleared! Time: %u sec, Rank: %c, Kills: %d\n",
           clear_time, "SABCD"[rank], d->monsters_killed);

    /* Build response */
    uint8_t response[32];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &clear_time, 4);
    offset += 4;

    response[offset++] = rank;

    memcpy(response + offset, &d->monsters_killed, 4);
    offset += 4;

    /* Base rewards (scaled by rank and difficulty) */
    uint32_t exp_reward = 10000 * (d->difficulty + 1) * (5 - rank);
    uint32_t gold_reward = 5000 * (d->difficulty + 1) * (5 - rank);

    memcpy(response + offset, &exp_reward, 4);
    offset += 4;

    memcpy(response + offset, &gold_reward, 4);
    offset += 4;

    /* Persist rewards to database */
    printf("[DUNGEON] Persisting rewards: EXP=%u, Gold=%u\n", exp_reward, gold_reward);

    /* Add EXP to database */
    if (db_add_exp(s->character_id, exp_reward) >= 0) {
        printf("[DUNGEON] EXP persisted to DB\n");
        /* Update in-memory too */
        for (int i = 0; i < s->character_count; i++) {
            if (s->characters[i].charac_no == s->character_id) {
                s->characters[i].exp += exp_reward;
                break;
            }
        }
    }

    /* Add gold to database */
    if (db_add_gold(s->character_id, gold_reward) >= 0) {
        printf("[DUNGEON] Gold persisted to DB\n");
        /* Update in-memory too */
        s->inventory.money += gold_reward;
    }

    /* Drop a random item as reward (simple: item_id based on dungeon_id + difficulty) */
    uint32_t drop_item_id = 60000 + d->dungeon_id * 100 + d->difficulty * 10 + (rank < 3 ? 1 : 0);
    int drop_slot = db_find_free_slot(s->character_id, MAX_EQUIP_SLOTS);
    if (drop_slot > 0 && drop_slot < 200) {
        int new_id = db_add_item(s->character_id, drop_item_id, drop_slot, 0);
        if (new_id > 0) {
            printf("[DUNGEON] Item drop persisted: item_id=%u, slot=%d\n", drop_item_id, drop_slot);
        }
    }

    /* Mark dungeon as complete */
    d->active = 0;
    s->state = STATE_IN_GAME;

    return send_response(s, 70, response, offset);
}

/* msg_no=71: DUNGEON_REWARD (claim additional rewards) */
static int handle_dungeon_reward(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] DUNGEON_REWARD from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 71, error, 4);
    }

    uint32_t reward_type;
    memcpy(&reward_type, data, 4);

    printf("[DUNGEON] Claiming reward type: %u\n", reward_type);

    /* Generate reward item based on type */
    uint32_t item_id = 2000 + (reward_type % 10);
    uint16_t item_count = 1;

    /* Find free slot and add item to database */
    int slot = db_find_free_slot(s->character_id, MAX_EQUIP_SLOTS);
    if (slot < 0 || slot >= 200) {
        uint8_t error[4] = {4, 0, 0, 0};  /* Inventory full */
        return send_response(s, 71, error, 4);
    }

    int new_ui_id = db_add_item(s->character_id, item_id, slot, 0);
    if (new_ui_id <= 0) {
        uint8_t error[4] = {5, 0, 0, 0};  /* DB error */
        return send_response(s, 71, error, 4);
    }

    printf("[DUNGEON] Reward item persisted: item_id=%u, slot=%d, ui_id=%d\n",
           item_id, slot, new_ui_id);

    /* Build response */
    uint8_t response[32];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &item_id, 4);
    offset += 4;
    memcpy(response + offset, &item_count, 2);
    offset += 2;

    uint16_t slot16 = (uint16_t)slot;
    memcpy(response + offset, &slot16, 2);
    offset += 2;

    return send_response(s, 71, response, offset);
}

/* msg_no=220: DUNGEON_MONSTER_TICK - Client sends player position, server updates monsters */
static int handle_dungeon_monster_tick(ClientSession* s, const uint8_t* data, int len) {
    if (s->state != STATE_IN_DUNGEON || len < 4) {
        return 0;  /* Silent fail for tick messages */
    }
    
    /* Parse player position from client */
    uint16_t player_x, player_y;
    memcpy(&player_x, data, 2);
    memcpy(&player_y, data + 2, 2);
    
    DungeonInstance* d = &s->dungeon;
    if (!d->active) return 0;
    
    /* Update all monster AI */
    uint8_t attack_flags[8];
    memset(attack_flags, 0, sizeof(attack_flags));
    int attacks = dungeon_tick_monsters(d, player_x, player_y, s->character_id, attack_flags);
    
    /* Build response: monster positions + pending attacks */
    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;
    
    /* Result code */
    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;
    
    /* Monster count */
    response[offset++] = (uint8_t)d->monster_count;
    
    /* Monster positions: id(4) + x(2) + y(2) + state(1) + hp(4) = 13 bytes */
    for (int i = 0; i < d->monster_count && offset < 240; i++) {
        MonsterInfo* m = &d->monsters[i];
        memcpy(response + offset, &m->monster_id, 4);
        offset += 4;
        memcpy(response + offset, &m->x, 2);
        offset += 2;
        memcpy(response + offset, &m->y, 2);
        offset += 2;
        response[offset++] = m->state;
        memcpy(response + offset, &m->hp, 4);
        offset += 4;
    }
    
    /* Pending attacks count */
    response[offset++] = (uint8_t)attacks;
    
    /* Attack info: monster_index(1) + damage(4) = 5 bytes each */
    for (int i = 0; i < attacks && i < 8 && offset < 250; i++) {
        int mon_idx = attack_flags[i];
        MonsterInfo* m = &d->monsters[mon_idx];
        response[offset++] = (uint8_t)mon_idx;
        memcpy(response + offset, &m->attack_damage, 4);
        offset += 4;
    }
    
    return send_response(s, 214, response, offset);
}

/* msg_no=221: DUNGEON_PLAYER_HIT - Acknowledge player hit by monster */
static int handle_dungeon_player_hit(ClientSession* s, const uint8_t* data, int len) {
    if (s->state != STATE_IN_DUNGEON || len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 215, error, 4);
    }
    
    uint32_t monster_id;
    uint8_t monster_idx;
    memcpy(&monster_id, data, 4);
    monster_idx = data[4];
    
    DungeonInstance* d = &s->dungeon;
    if (!d->active || monster_idx >= d->monster_count) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 215, error, 4);
    }
    
    MonsterInfo* m = &d->monsters[monster_idx];
    if (!m->alive || m->monster_id != monster_id) {
        uint8_t error[4] = {3, 0, 0, 0};
        return send_response(s, 215, error, 4);
    }
    
    uint32_t damage = m->attack_damage;
    printf("[DUNGEON] Player hit by monster %u for %u damage\n", monster_id, damage);
    
    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 0;
    
    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;
    memcpy(response + offset, &monster_id, 4);
    offset += 4;
    memcpy(response + offset, &damage, 4);
    offset += 4;
    
    return send_response(s, 215, response, offset);
}

/*==============================================================================
 * Trade System (msg_no 75-83)
 *============================================================================*/

/* Trade state structure */
typedef struct {
    uint32_t partner_fd;          /* Trading partner's fd */
    uint32_t partner_id;          /* Partner character ID */
    uint32_t items[10];           /* Item ui_ids in trade */
    uint8_t  item_count;
    uint32_t gold;                /* Gold offered */
    uint8_t  locked;              /* Trade locked (ready to confirm) */
    uint8_t  confirmed;           /* Trade confirmed */
    uint8_t  active;
} TradeState;

static TradeState g_trades[MAX_SESSIONS];

/* PartyInfo struct and g_parties defined earlier at line ~165 for shared dungeon support */

/* Find party by ID (implementation of forward declaration) */
static PartyInfo* find_party(uint32_t party_id) {
    for (int i = 0; i < MAX_PARTIES; i++) {
        if (g_parties[i].active && g_parties[i].party_id == party_id) {
            return &g_parties[i];
        }
    }
    return NULL;
}

/* Create a new party */
static PartyInfo* create_party(uint32_t leader_id) {
    for (int i = 0; i < MAX_PARTIES; i++) {
        if (!g_parties[i].active) {
            memset(&g_parties[i], 0, sizeof(PartyInfo));
            g_parties[i].party_id = g_next_party_id++;
            g_parties[i].leader_id = leader_id;
            g_parties[i].members[0] = leader_id;
            g_parties[i].member_count = 1;
            g_parties[i].loot_mode = 0;
            g_parties[i].active = 1;
            printf("[PARTY] Created party %u with leader %u\n",
                   g_parties[i].party_id, leader_id);
            return &g_parties[i];
        }
    }
    return NULL;  /* No free slots */
}

/* Add member to party */
static int party_add_member(PartyInfo* party, uint32_t member_id) {
    if (!party || party->member_count >= MAX_PARTY_MEMBERS) return -1;

    /* Check not already in party */
    for (int i = 0; i < (int)party->member_count; i++) {
        if (party->members[i] == member_id) return -1;
    }

    party->members[party->member_count++] = member_id;
    printf("[PARTY] Added member %u to party %u (count=%u)\n",
           member_id, party->party_id, party->member_count);
    return 0;
}

/* Remove member from party */
static int party_remove_member(PartyInfo* party, uint32_t member_id) {
    if (!party) return -1;

    int found = -1;
    for (int i = 0; i < (int)party->member_count; i++) {
        if (party->members[i] == member_id) {
            found = i;
            break;
        }
    }

    if (found < 0) return -1;

    /* Shift remaining members */
    for (int i = found; i < (int)party->member_count - 1; i++) {
        party->members[i] = party->members[i + 1];
    }
    party->member_count--;

    printf("[PARTY] Removed member %u from party %u (count=%u)\n",
           member_id, party->party_id, party->member_count);

    /* If party empty or leader left, dissolve party */
    if (party->member_count == 0 || member_id == party->leader_id) {
        if (party->member_count > 0) {
            /* Transfer leadership to first remaining member */
            party->leader_id = party->members[0];
            printf("[PARTY] New leader: %u\n", party->leader_id);
        } else {
            party->active = 0;
            printf("[PARTY] Party %u dissolved\n", party->party_id);
        }
    }

    return 0;
}

/* Notify all party members */
static void party_notify_members(PartyInfo* party, uint8_t msg_no,
                                  const uint8_t* data, int len) {
    if (!party) return;

    for (int i = 0; i < (int)party->member_count; i++) {
        uint32_t member_id = party->members[i];
        for (int j = 0; j < MAX_SESSIONS; j++) {
            if (g_sessions[j].active && g_sessions[j].character_id == member_id) {
                send_response(&g_sessions[j], msg_no, data, len);
                break;
            }
        }
    }
}


/* Auction listing info */
#define MAX_AUCTION_LISTINGS 256
typedef struct {
    uint32_t listing_id;          /* Unique listing ID */
    uint32_t seller_id;           /* Seller character ID */
    char     seller_name[20];     /* Seller name */
    uint32_t item_ui_id;          /* Item unique ID from user_items */
    uint32_t item_id;             /* Item template ID */
    uint32_t price;               /* Asking price */
    uint32_t expire_time;         /* Unix timestamp when expires */
    uint8_t  active;
} AuctionListing;

static AuctionListing g_auction_listings[MAX_AUCTION_LISTINGS];
static uint32_t g_next_auction_id = 1;

/* Create auction listing */
static AuctionListing* create_auction_listing(uint32_t seller_id, const char* seller_name,
                                                uint32_t item_ui_id, uint32_t item_id,
                                                uint32_t price, uint32_t duration_hours) {
    for (int i = 0; i < MAX_AUCTION_LISTINGS; i++) {
        if (!g_auction_listings[i].active) {
            AuctionListing* a = &g_auction_listings[i];
            memset(a, 0, sizeof(AuctionListing));
            a->listing_id = g_next_auction_id++;
            a->seller_id = seller_id;
            strncpy(a->seller_name, seller_name, 19);
            a->item_ui_id = item_ui_id;
            a->item_id = item_id;
            a->price = price;
            a->expire_time = (uint32_t)time(NULL) + (duration_hours * 3600);
            a->active = 1;
            printf("[AUCTION] Created listing %u: seller=%u item=%u price=%u\n",
                   a->listing_id, seller_id, item_id, price);
            return a;
        }
    }
    return NULL;
}

/* Find auction listing by ID */
static AuctionListing* find_auction_listing(uint32_t listing_id) {
    for (int i = 0; i < MAX_AUCTION_LISTINGS; i++) {
        if (g_auction_listings[i].active && g_auction_listings[i].listing_id == listing_id) {
            return &g_auction_listings[i];
        }
    }
    return NULL;
}

/* Cancel auction listing */
static int cancel_auction_listing(uint32_t listing_id, uint32_t seller_id) {
    AuctionListing* a = find_auction_listing(listing_id);
    if (a && a->seller_id == seller_id) {
        printf("[AUCTION] Cancelled listing %u\n", listing_id);
        a->active = 0;
        return 0;
    }
    return -1;
}

/* Search auction listings by item_id */
static int search_auction_listings(uint32_t item_id, uint32_t min_price, uint32_t max_price,
                                    AuctionListing** results, int max_results) {
    int count = 0;
    uint32_t now = (uint32_t)time(NULL);

    for (int i = 0; i < MAX_AUCTION_LISTINGS && count < max_results; i++) {
        AuctionListing* a = &g_auction_listings[i];
        if (!a->active || a->expire_time < now) continue;

        /* Match by item_id (0 = all items) */
        if (item_id > 0 && a->item_id != item_id) continue;

        /* Match by price range */
        if (min_price > 0 && a->price < min_price) continue;
        if (max_price > 0 && a->price > max_price) continue;

        results[count++] = a;
    }

    return count;
}

/* Get seller's listings */
static int get_seller_listings(uint32_t seller_id, AuctionListing** results, int max_results) {
    int count = 0;
    uint32_t now = (uint32_t)time(NULL);

    for (int i = 0; i < MAX_AUCTION_LISTINGS && count < max_results; i++) {
        AuctionListing* a = &g_auction_listings[i];
        if (!a->active) continue;
        if (a->seller_id == seller_id) {
            /* Mark expired listings */
            if (a->expire_time < now) a->active = 0;
            else results[count++] = a;
        }
    }

    return count;
}


/* msg_no=75: TRADE_REQUEST */
static int handle_trade_request(ClientSession* s, const uint8_t* data, int len) {
    printf("[TRADE] TRADE_REQUEST from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 75, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);

    /* Find target session */
    ClientSession* target = NULL;
    int target_idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == target_id) {
            target = &g_sessions[i];
            target_idx = i;
            break;
        }
    }

    if (!target || target->state != STATE_IN_GAME) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Target not found */
        return send_response(s, 75, error, 4);
    }

    /* Find requester index */
    int req_idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) {
            req_idx = i;
            break;
        }
    }

    /* Initialize trade states */
    memset(&g_trades[req_idx], 0, sizeof(TradeState));
    memset(&g_trades[target_idx], 0, sizeof(TradeState));
    g_trades[req_idx].partner_fd = target->fd;
    g_trades[req_idx].partner_id = target_id;
    g_trades[req_idx].active = 1;
    g_trades[target_idx].partner_fd = s->fd;
    g_trades[target_idx].partner_id = s->character_id;
    g_trades[target_idx].active = 1;

    /* Notify target */
    uint8_t notify[64];
    memset(notify, 0, sizeof(notify));
    uint32_t result = 0;
    memcpy(notify, &result, 4);
    memcpy(notify + 4, &s->character_id, 4);
    memcpy(notify + 8, s->character_name, 20);
    send_response(target, 75, notify, 28);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response, &result, 4);
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 75, response, 8);
}

/* msg_no=76: TRADE_ACCEPT */
static int handle_trade_accept(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TRADE] TRADE_ACCEPT from fd=%d\n", s->fd);

    int idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) { idx = i; break; }
    }

    if (idx < 0 || !g_trades[idx].active) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 76, error, 4);
    }

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 76, response, 4);
}

/* msg_no=77: TRADE_DECLINE */
static int handle_trade_decline(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TRADE] TRADE_DECLINE from fd=%d\n", s->fd);

    int idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) { idx = i; break; }
    }

    if (idx >= 0 && g_trades[idx].active) {
        /* Find partner and notify */
        for (int i = 0; i < MAX_SESSIONS; i++) {
            if (g_sessions[i].fd == (int)g_trades[idx].partner_fd) {
                uint8_t notify[4] = {3, 0, 0, 0}; /* Trade declined */
                send_response(&g_sessions[i], 77, notify, 4);
                g_trades[i].active = 0;
                break;
            }
        }
        g_trades[idx].active = 0;
    }

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 77, response, 4);
}

/* msg_no=78: TRADE_ADD_ITEM */
static int handle_trade_add_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[TRADE] TRADE_ADD_ITEM from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 78, error, 4);
    }

    int idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) { idx = i; break; }
    }

    if (idx < 0 || !g_trades[idx].active || g_trades[idx].locked) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 78, error, 4);
    }

    uint32_t item_uid;
    memcpy(&item_uid, data, 4);

    if (g_trades[idx].item_count < 10) {
        g_trades[idx].items[g_trades[idx].item_count++] = item_uid;
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &item_uid, 4);
    return send_response(s, 78, response, 8);
}

/* msg_no=79: TRADE_REMOVE_ITEM */
static int handle_trade_remove_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[TRADE] TRADE_REMOVE_ITEM from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 79, error, 4);
    }

    int idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) { idx = i; break; }
    }

    if (idx < 0 || !g_trades[idx].active || g_trades[idx].locked) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 79, error, 4);
    }

    uint32_t item_uid;
    memcpy(&item_uid, data, 4);

    /* Remove item from trade */
    for (int i = 0; i < g_trades[idx].item_count; i++) {
        if (g_trades[idx].items[i] == item_uid) {
            for (int j = i; j < g_trades[idx].item_count - 1; j++) {
                g_trades[idx].items[j] = g_trades[idx].items[j + 1];
            }
            g_trades[idx].item_count--;
            break;
        }
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &item_uid, 4);
    return send_response(s, 79, response, 8);
}

/* msg_no=80: TRADE_SET_GOLD */
static int handle_trade_set_gold(ClientSession* s, const uint8_t* data, int len) {
    printf("[TRADE] TRADE_SET_GOLD from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 80, error, 4);
    }

    int idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) { idx = i; break; }
    }

    if (idx < 0 || !g_trades[idx].active || g_trades[idx].locked) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 80, error, 4);
    }

    uint32_t gold;
    memcpy(&gold, data, 4);

    if (gold > s->inventory.money) {
        gold = s->inventory.money;
    }
    g_trades[idx].gold = gold;

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &gold, 4);
    return send_response(s, 80, response, 8);
}

/* msg_no=81: TRADE_LOCK */
static int handle_trade_lock(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TRADE] TRADE_LOCK from fd=%d\n", s->fd);

    int idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) { idx = i; break; }
    }

    if (idx < 0 || !g_trades[idx].active) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 81, error, 4);
    }

    g_trades[idx].locked = 1;

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 81, response, 4);
}

/* msg_no=82: TRADE_CONFIRM */
static int handle_trade_confirm(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TRADE] TRADE_CONFIRM from fd=%d\n", s->fd);

    int idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) { idx = i; break; }
    }

    if (idx < 0 || !g_trades[idx].active || !g_trades[idx].locked) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 82, error, 4);
    }

    g_trades[idx].confirmed = 1;

    /* Check if both confirmed */
    int partner_idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].fd == (int)g_trades[idx].partner_fd) {
            partner_idx = i;
            break;
        }
    }

    if (partner_idx >= 0 && g_trades[partner_idx].confirmed) {
        ClientSession* partner = &g_sessions[partner_idx];

        /* Execute trade - swap gold in memory */
        uint32_t my_give = g_trades[idx].gold;
        uint32_t partner_give = g_trades[partner_idx].gold;

        s->inventory.money -= my_give;
        s->inventory.money += partner_give;
        partner->inventory.money -= partner_give;
        partner->inventory.money += my_give;

        printf("[TRADE] Gold swap: %s gives %u, %s gives %u\n",
               s->character_name, my_give, partner->character_name, partner_give);

        /* Persist gold to database */
        db_update_gold(s->character_id, s->inventory.money);
        db_update_gold(partner->character_id, partner->inventory.money);

        /* Transfer items from self to partner */
        for (int i = 0; i < g_trades[idx].item_count && i < 10; i++) {
            uint32_t item_uid = g_trades[idx].items[i];
            if (item_uid > 0) {
                int new_slot = db_transfer_item(item_uid, s->character_id, partner->character_id);
                if (new_slot >= 0) {
                    printf("[TRADE] Item %u transferred from %s to %s (slot=%d)\n",
                           item_uid, s->character_name, partner->character_name, new_slot);
                } else {
                    printf("[TRADE] Item %u transfer FAILED from %s to %s\n",
                           item_uid, s->character_name, partner->character_name);
                }
            }
        }

        /* Transfer items from partner to self */
        for (int i = 0; i < g_trades[partner_idx].item_count && i < 10; i++) {
            uint32_t item_uid = g_trades[partner_idx].items[i];
            if (item_uid > 0) {
                int new_slot = db_transfer_item(item_uid, partner->character_id, s->character_id);
                if (new_slot >= 0) {
                    printf("[TRADE] Item %u transferred from %s to %s (slot=%d)\n",
                           item_uid, partner->character_name, s->character_name, new_slot);
                } else {
                    printf("[TRADE] Item %u transfer FAILED from %s to %s\n",
                           item_uid, partner->character_name, s->character_name);
                }
            }
        }

        /* Clear trades */
        g_trades[idx].active = 0;
        g_trades[partner_idx].active = 0;

        printf("[TRADE] Trade completed between %s and %s\n",
               s->character_name, partner->character_name);

        /* Notify both */
        uint8_t complete[4] = {0, 0, 0, 0};
        send_response(partner, 82, complete, 4);
    }

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 82, response, 4);
}

/* msg_no=83: TRADE_CANCEL */
static int handle_trade_cancel(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TRADE] TRADE_CANCEL from fd=%d\n", s->fd);

    int idx = -1;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (&g_sessions[i] == s) { idx = i; break; }
    }

    if (idx >= 0 && g_trades[idx].active) {
        for (int i = 0; i < MAX_SESSIONS; i++) {
            if (g_sessions[i].fd == (int)g_trades[idx].partner_fd) {
                uint8_t notify[4] = {4, 0, 0, 0}; /* Trade cancelled */
                send_response(&g_sessions[i], 83, notify, 4);
                g_trades[i].active = 0;
                break;
            }
        }
        g_trades[idx].active = 0;
    }

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 83, response, 4);
}

/*==============================================================================
 * Mail System (msg_no 85-91)
 *============================================================================*/

/* msg_no=85: MAIL_GET_LIST */
static int handle_mail_get_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[MAIL] MAIL_GET_LIST from fd=%d\n", s->fd);

    uint8_t response[2048];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Get mail from database */
    DBMailInfo mails[20];
    int mail_count = db_get_mail_list(s->character_id, mails, 20);

    response[offset++] = (uint8_t)mail_count;

    for (int i = 0; i < mail_count && offset < 2000; i++) {
        /* mail_id */
        memcpy(response + offset, &mails[i].postal_id, 4);
        offset += 4;

        /* sender name */
        char sender[20];
        memset(sender, 0, 20);
        strncpy(sender, mails[i].send_name, 19);
        memcpy(response + offset, sender, 20);
        offset += 20;

        /* subject (use sender name as subject for now) */
        char subject[32];
        memset(subject, 0, 32);
        if (mails[i].gold > 0) {
            snprintf(subject, 31, "Gold: %u", mails[i].gold);
        } else if (mails[i].item_id > 0) {
            snprintf(subject, 31, "Item: %u", mails[i].item_id);
        } else {
            strncpy(subject, "Message", 31);
        }
        memcpy(response + offset, subject, 32);
        offset += 32;

        /* timestamp */
        memcpy(response + offset, &mails[i].occ_time, 4);
        offset += 4;

        /* has_attachment */
        uint8_t has_attach = (mails[i].item_id > 0 || mails[i].gold > 0) ? 1 : 0;
        response[offset++] = has_attach;

        /* is_read */
        response[offset++] = mails[i].read_flag;
    }

    printf("[MAIL] Returning %d mails for charac_no=%u\n", mail_count, s->character_id);
    return send_response(s, 85, response, offset);
}

/* msg_no=86: MAIL_SEND */
static int handle_mail_send(ClientSession* s, const uint8_t* data, int len) {
    printf("[MAIL] MAIL_SEND from fd=%d\n", s->fd);
    if (len < 56) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 86, error, 4);
    }

    char recipient[21];
    char subject[33];
    memset(recipient, 0, sizeof(recipient));
    memset(subject, 0, sizeof(subject));
    memcpy(recipient, data, 20);
    memcpy(subject, data + 20, 32);

    uint32_t item_id = 0;
    uint32_t gold = 0;
    if (len >= 56) memcpy(&item_id, data + 52, 4);
    if (len >= 60) memcpy(&gold, data + 56, 4);

    printf("[MAIL] Sending mail to '%s' subject='%s' item=%u gold=%u\n",
           recipient, subject, item_id, gold);

    /* Look up recipient charac_no by name */
    uint32_t recv_charac_no = db_get_charac_no_by_name(recipient);
    if (recv_charac_no == 0) {
        printf("[MAIL] Recipient '%s' not found\n", recipient);
        uint8_t error[4] = {2, 0, 0, 0}; /* Error: recipient not found */
        return send_response(s, 86, error, 4);
    }

    /* Get sender's character name */
    char send_name[21];
    memset(send_name, 0, sizeof(send_name));
    strncpy(send_name, s->character_name, 20);

    /* Deduct postage (100 gold) - check balance first */
    uint32_t postage = 100;
    uint32_t total_cost = postage + gold;
    if (s->inventory.money < total_cost) {
        printf("[MAIL] Insufficient gold: have %u, need %u\n", s->inventory.money, total_cost);
        uint8_t error[4] = {3, 0, 0, 0}; /* Error: insufficient gold */
        return send_response(s, 86, error, 4);
    }

    /* Insert mail into postal table */
    int mail_id = db_send_mail(s->character_id, send_name, recv_charac_no, item_id, gold);
    if (mail_id <= 0) {
        printf("[MAIL] Failed to insert mail into database\n");
        uint8_t error[4] = {4, 0, 0, 0}; /* Error: database error */
        return send_response(s, 86, error, 4);
    }

    /* Deduct gold from sender */
    s->inventory.money -= total_cost;
    db_update_gold(s->character_id, s->inventory.money);
    printf("[MAIL] Mail sent successfully, mail_id=%d, deducted %u gold\n", mail_id, total_cost);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    uint32_t result = 0; /* Success */
    memcpy(response, &result, 4);
    memcpy(response + 4, &mail_id, 4);
    return send_response(s, 86, response, 8);
}

/* msg_no=87: MAIL_READ */
static int handle_mail_read(ClientSession* s, const uint8_t* data, int len) {
    printf("[MAIL] MAIL_READ from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 87, error, 4);
    }

    uint32_t mail_id;
    memcpy(&mail_id, data, 4);

    /* Get mail from database */
    DBMailInfo mail;
    if (db_get_mail_by_id(mail_id, s->character_id, &mail) < 0) {
        printf("[MAIL] Mail %u not found for charac_no=%u\n", mail_id, s->character_id);
        uint8_t error[4] = {2, 0, 0, 0}; /* Mail not found */
        return send_response(s, 87, error, 4);
    }

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &mail_id, 4);
    offset += 4;

    /* Sender name from database */
    memcpy(response + offset, mail.send_name, 20);
    offset += 20;

    /* Subject - generate from mail info */
    char subject[32];
    memset(subject, 0, sizeof(subject));
    if (mail.gold > 0 && mail.item_id > 0) {
        snprintf(subject, sizeof(subject) - 1, "Gold & Item");
    } else if (mail.gold > 0) {
        snprintf(subject, sizeof(subject) - 1, "Gold: %u", mail.gold);
    } else if (mail.item_id > 0) {
        snprintf(subject, sizeof(subject) - 1, "Item: %u", mail.item_id);
    } else {
        strncpy(subject, "Message", sizeof(subject) - 1);
    }
    memcpy(response + offset, subject, 32);
    offset += 32;

    /* Body */
    char body[128];
    memset(body, 0, sizeof(body));
    snprintf(body, sizeof(body) - 1, "From: %s", mail.send_name);
    memcpy(response + offset, body, 128);
    offset += 128;

    /* Attachment gold and item */
    memcpy(response + offset, &mail.gold, 4);
    offset += 4;

    memcpy(response + offset, &mail.item_id, 4);
    offset += 4;

    printf("[MAIL] Read mail %u: from=%s gold=%u item=%u\n",
           mail_id, mail.send_name, mail.gold, mail.item_id);
    return send_response(s, 87, response, offset);
}

/* msg_no=88: MAIL_DELETE */
static int handle_mail_delete(ClientSession* s, const uint8_t* data, int len) {
    printf("[MAIL] MAIL_DELETE from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 88, error, 4);
    }

    uint32_t mail_id;
    memcpy(&mail_id, data, 4);
    printf("[MAIL] Deleting mail %u for charac_no=%u\n", mail_id, s->character_id);

    /* Delete mail from database */
    if (db_delete_mail(mail_id, s->character_id) < 0) {
        printf("[MAIL] Failed to delete mail %u\n", mail_id);
        uint8_t error[4] = {2, 0, 0, 0}; /* Mail not found or not owned */
        return send_response(s, 88, error, 4);
    }

    printf("[MAIL] Mail %u deleted successfully\n", mail_id);
    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &mail_id, 4);
    return send_response(s, 88, response, 8);
}

/* msg_no=89: MAIL_TAKE_ATTACHMENT */
static int handle_mail_take_attachment(ClientSession* s, const uint8_t* data, int len) {
    printf("[MAIL] MAIL_TAKE_ATTACHMENT from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 89, error, 4);
    }

    uint32_t mail_id;
    memcpy(&mail_id, data, 4);

    /* Get mail attachment from database */
    DBMailAttachment att;
    if (db_get_mail_attachment(mail_id, s->character_id, &att) < 0) {
        printf("[MAIL] Mail %u not found or not for this character\n", mail_id);
        uint8_t error[4] = {2, 0, 0, 0}; /* Mail not found */
        return send_response(s, 89, error, 4);
    }

    /* Check if already claimed */
    if (att.claimed) {
        printf("[MAIL] Mail %u already claimed\n", mail_id);
        uint8_t error[4] = {3, 0, 0, 0}; /* Already claimed */
        return send_response(s, 89, error, 4);
    }

    /* Give gold to player */
    if (att.gold > 0) {
        s->inventory.money += att.gold;
        db_update_gold(s->character_id, s->inventory.money);
        printf("[MAIL] Added %u gold from mail %u\n", att.gold, mail_id);
    }

    /* Add item to player inventory if present */
    if (att.item_id > 0) {
        int free_slot = db_find_free_slot(s->character_id, 11);  /* Bag slots start at 11 */
        if (free_slot >= 0 && free_slot <= 200) {
            int new_ui_id = db_add_item(s->character_id, att.item_id, free_slot, 0);
            if (new_ui_id > 0) {
                printf("[MAIL] Added item_id=%u to slot=%d (new ui_id=%d) from mail %u\n",
                       att.item_id, free_slot, new_ui_id, mail_id);
            } else {
                printf("[MAIL] Failed to add item_id=%u from mail %u\n", att.item_id, mail_id);
            }
        } else {
            printf("[MAIL] No free inventory slot for item from mail %u\n", mail_id);
        }
    }

    /* Mark mail as claimed */
    db_claim_mail_attachment(mail_id);

    printf("[MAIL] Took attachment from mail %u: gold=%u item=%u\n",
           mail_id, att.gold, att.item_id);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    uint32_t result = 0;
    memcpy(response, &result, 4);
    memcpy(response + 4, &mail_id, 4);
    memcpy(response + 8, &att.gold, 4);
    memcpy(response + 12, &att.item_id, 4);
    return send_response(s, 89, response, 16);
}

/* msg_no=90: MAIL_GET_UNREAD_COUNT */
static int handle_mail_get_unread_count(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[MAIL] MAIL_GET_UNREAD_COUNT from fd=%d\n", s->fd);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    uint32_t unread = db_get_unread_mail_count(s->character_id);
    memcpy(response + 4, &unread, 4);
    printf("[MAIL] Unread count=%u for charac_no=%u\n", unread, s->character_id);
    return send_response(s, 90, response, 8);
}

/* msg_no=91: MAIL_RETURN */
static int handle_mail_return(ClientSession* s, const uint8_t* data, int len) {
    printf("[MAIL] MAIL_RETURN from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 91, error, 4);
    }

    uint32_t mail_id;
    memcpy(&mail_id, data, 4);
    printf("[MAIL] Returning mail %u to sender\n", mail_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &mail_id, 4);
    return send_response(s, 91, response, 8);
}

/*==============================================================================
 * Friend System (msg_no 95-102)
 *============================================================================*/

/* msg_no=95: FRIEND_GET_LIST */
static int handle_friend_get_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[FRIEND] FRIEND_GET_LIST from fd=%d\n", s->fd);

    uint8_t response[1024];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Get friends from database */
    DBFriendInfo friends[30];
    int friend_count = db_get_friend_list(s->character_id, friends, 30);

    response[offset++] = (uint8_t)friend_count;

    for (int i = 0; i < friend_count && offset < 1000; i++) {
        /* friend_id */
        memcpy(response + offset, &friends[i].friend_charac_no, 4);
        offset += 4;

        /* name */
        char name[20];
        memset(name, 0, 20);
        strncpy(name, friends[i].friend_name, 19);
        memcpy(response + offset, name, 20);
        offset += 20;

        /* level (we don't have this from friend table, use 0) */
        uint8_t level = 0;
        response[offset++] = level;

        /* job (we don't have this from friend table, use 0) */
        uint8_t job = 0;
        response[offset++] = job;

        /* online status */
        response[offset++] = friends[i].online;
    }

    printf("[FRIEND] Returning %d friends for charac_no=%u\n", friend_count, s->character_id);
    return send_response(s, 95, response, offset);
}

/* msg_no=96: FRIEND_ADD */
static int handle_friend_add(ClientSession* s, const uint8_t* data, int len) {
    printf("[FRIEND] FRIEND_ADD from fd=%d\n", s->fd);
    if (len < 20) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 96, error, 4);
    }

    char target_name[21];
    memset(target_name, 0, sizeof(target_name));
    memcpy(target_name, data, 20);
    printf("[FRIEND] Adding friend: %s\n", target_name);

    /* Look up target's charac_no by name */
    uint32_t friend_charac_no = db_get_charac_no_by_name(target_name);
    if (friend_charac_no == 0) {
        printf("[FRIEND] Target '%s' not found\n", target_name);
        uint8_t error[4] = {2, 0, 0, 0}; /* Error: character not found */
        return send_response(s, 96, error, 4);
    }

    /* Cannot add yourself */
    if (friend_charac_no == s->character_id) {
        printf("[FRIEND] Cannot add yourself as friend\n");
        uint8_t error[4] = {3, 0, 0, 0}; /* Error: cannot add self */
        return send_response(s, 96, error, 4);
    }

    /* Add friend relationship to database (bidirectional) */
    int result = db_add_friend(s->character_id, friend_charac_no);
    if (result < 0) {
        printf("[FRIEND] Failed to add friend to database\n");
        uint8_t error[4] = {4, 0, 0, 0}; /* Error: database error or already friends */
        return send_response(s, 96, error, 4);
    }

    printf("[FRIEND] Friend added successfully: %s (charac_no=%u)\n", target_name, friend_charac_no);

    /* Notify target if online */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == friend_charac_no) {
            uint8_t notify[64];
            memset(notify, 0, sizeof(notify));
            uint32_t req_result = 0;
            memcpy(notify, &req_result, 4);
            memcpy(notify + 4, &s->character_id, 4);
            memcpy(notify + 8, s->character_name, 20);
            send_response(&g_sessions[i], 96, notify, 28);
            printf("[FRIEND] Notified online friend fd=%d\n", g_sessions[i].fd);
            break;
        }
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &friend_charac_no, 4);
    return send_response(s, 96, response, 8);
}

/* msg_no=97: FRIEND_REMOVE */
static int handle_friend_remove(ClientSession* s, const uint8_t* data, int len) {
    printf("[FRIEND] FRIEND_REMOVE from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 97, error, 4);
    }

    uint32_t friend_charac_no;
    memcpy(&friend_charac_no, data, 4);
    printf("[FRIEND] Removing friend charac_no: %u\n", friend_charac_no);

    /* Remove friend from database (bidirectional) */
    int result = db_remove_friend(s->character_id, friend_charac_no);
    if (result < 0) {
        printf("[FRIEND] Failed to remove friend from database\n");
        uint8_t error[4] = {2, 0, 0, 0}; /* Error: database error */
        return send_response(s, 97, error, 4);
    }

    printf("[FRIEND] Friend removed successfully: charac_no=%u\n", friend_charac_no);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &friend_charac_no, 4);
    return send_response(s, 97, response, 8);
}

/* msg_no=98: FRIEND_ACCEPT */
static int handle_friend_accept(ClientSession* s, const uint8_t* data, int len) {
    printf("[FRIEND] FRIEND_ACCEPT from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 98, error, 4);
    }

    uint32_t requester_id;
    memcpy(&requester_id, data, 4);
    printf("[FRIEND] Accepting friend request from ID: %u\n", requester_id);

    /* Add bidirectional friendship in database */
    if (db_add_friend(s->character_id, requester_id) < 0) {
        printf("[FRIEND] Failed to save friendship to database\n");
        uint8_t error[4] = {2, 0, 0, 0}; /* Database error */
        return send_response(s, 98, error, 4);
    }
    /* Also add reverse direction */
    db_add_friend(requester_id, s->character_id);

    printf("[FRIEND] Friendship saved: %u <-> %u\n", s->character_id, requester_id);

    /* Notify requester if online */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == requester_id) {
            uint8_t notify[64];
            memset(notify, 0, sizeof(notify));
            memcpy(notify + 4, &s->character_id, 4);
            memcpy(notify + 8, s->character_name, 20);
            send_response(&g_sessions[i], 98, notify, 28);
            break;
        }
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &requester_id, 4);
    return send_response(s, 98, response, 8);
}

/* msg_no=99: FRIEND_DECLINE */
static int handle_friend_decline(ClientSession* s, const uint8_t* data, int len) {
    printf("[FRIEND] FRIEND_DECLINE from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 99, error, 4);
    }

    uint32_t requester_id;
    memcpy(&requester_id, data, 4);
    printf("[FRIEND] Declining friend request from ID: %u\n", requester_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &requester_id, 4);
    return send_response(s, 99, response, 8);
}

/* msg_no=100: FRIEND_BLOCK */
static int handle_friend_block(ClientSession* s, const uint8_t* data, int len) {
    printf("[FRIEND] FRIEND_BLOCK from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 100, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);
    printf("[FRIEND] Blocking user ID: %u\n", target_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 100, response, 8);
}

/* msg_no=101: FRIEND_UNBLOCK */
static int handle_friend_unblock(ClientSession* s, const uint8_t* data, int len) {
    printf("[FRIEND] FRIEND_UNBLOCK from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 101, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);
    printf("[FRIEND] Unblocking user ID: %u\n", target_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 101, response, 8);
}

/* msg_no=102: FRIEND_GET_BLOCKED_LIST */
static int handle_friend_get_blocked_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[FRIEND] FRIEND_GET_BLOCKED_LIST from fd=%d\n", s->fd);

    uint8_t response[64];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t blocked_count = 0;
    response[offset++] = blocked_count;

    return send_response(s, 102, response, offset);
}

/*==============================================================================
 * Guild System (msg_no 105-117)
 *============================================================================*/

/* msg_no=105: GUILD_CREATE */
static int handle_guild_create(ClientSession* s, const uint8_t* data, int len) {
    printf("[GUILD] GUILD_CREATE from fd=%d\n", s->fd);
    if (len < 20) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 105, error, 4);
    }

    /* Already in a guild */
    if (s->guild_id > 0) {
        printf("[GUILD] Player already in guild %u\n", s->guild_id);
        uint8_t error[4] = {3, 0, 0, 0}; /* Already in guild */
        return send_response(s, 105, error, 4);
    }

    char guild_name[21];
    memset(guild_name, 0, sizeof(guild_name));
    memcpy(guild_name, data, 20);
    printf("[GUILD] Creating guild: %s\n", guild_name);

    /* Check gold (creation fee: 100000) */
    uint32_t creation_fee = 100000;
    if (s->inventory.money < creation_fee) {
        printf("[GUILD] Not enough gold: have %u, need %u\n", s->inventory.money, creation_fee);
        uint8_t error[4] = {2, 0, 0, 0}; /* Not enough gold */
        return send_response(s, 105, error, 4);
    }

    /* Create guild in database */
    int guild_id = db_create_guild(guild_name, s->character_id, s->character_name);
    if (guild_id <= 0) {
        printf("[GUILD] Failed to create guild in database\n");
        uint8_t error[4] = {4, 0, 0, 0}; /* Database error */
        return send_response(s, 105, error, 4);
    }

    /* Add creator as guild master (grade=1) */
    if (db_add_guild_member(guild_id, s->character_id, s->character_name, 1) < 0) {
        printf("[GUILD] Failed to add guild master to member table\n");
    }

    /* Update character's guild_id */
    db_update_character_guild(s->character_id, guild_id);

    /* Deduct gold and update in database */
    s->inventory.money -= creation_fee;
    db_update_gold(s->character_id, s->inventory.money);
    s->guild_id = (uint32_t)guild_id;

    printf("[GUILD] Guild created: id=%d name=%s master=%s\n",
           guild_id, guild_name, s->character_name);

    uint8_t response[28];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &s->guild_id, 4);
    memcpy(response + 8, guild_name, 20);
    return send_response(s, 105, response, 28);
}

/* msg_no=106: GUILD_DISBAND */
static int handle_guild_disband(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GUILD] GUILD_DISBAND from fd=%d\n", s->fd);

    if (s->guild_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 106, error, 4);
    }

    uint32_t old_guild = s->guild_id;

    /* Disband guild in database */
    if (db_disband_guild(old_guild) < 0) {
        printf("[GUILD] Failed to disband guild %u in database\n", old_guild);
        uint8_t error[4] = {2, 0, 0, 0}; /* Database error */
        return send_response(s, 106, error, 4);
    }

    /* Update character's guild_id */
    db_update_character_guild(s->character_id, 0);

    /* Update all online members */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].guild_id == old_guild) {
            g_sessions[i].guild_id = 0;
            /* Notify member if not self */
            if (g_sessions[i].character_id != s->character_id) {
                uint8_t notify[8] = {6, 0, 0, 0, 0, 0, 0, 0}; /* Disbanded */
                memcpy(notify + 4, &old_guild, 4);
                send_response(&g_sessions[i], 106, notify, 8);
            }
        }
    }

    s->guild_id = 0;
    printf("[GUILD] Disbanded guild %u\n", old_guild);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &old_guild, 4);
    return send_response(s, 106, response, 8);
}

/* msg_no=107: GUILD_GET_INFO */
static int handle_guild_get_info(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GUILD] GUILD_GET_INFO from fd=%d\n", s->fd);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Get guild info from database */
    DBGuildInfo guild;
    int has_guild = (db_get_guild_by_charac(s->character_id, &guild) == 0);

    uint32_t result = has_guild ? 0 : 1;  /* 1 = not in guild */
    memcpy(response + offset, &result, 4);
    offset += 4;

    if (has_guild) {
        /* guild_id */
        memcpy(response + offset, &guild.guild_id, 4);
        offset += 4;
        s->guild_id = guild.guild_id;

        /* guild_name */
        char guild_name[20];
        memset(guild_name, 0, 20);
        strncpy(guild_name, guild.guild_name, 19);
        memcpy(response + offset, guild_name, 20);
        offset += 20;

        /* guild_level */
        response[offset++] = guild.level;

        /* member_count */
        uint16_t member_count = (uint16_t)guild.member_count;
        memcpy(response + offset, &member_count, 2);
        offset += 2;

        /* max_members (based on guild level) */
        uint16_t max_members = 20 + guild.level * 10;
        memcpy(response + offset, &max_members, 2);
        offset += 2;

        /* master_name */
        char master_name[20];
        memset(master_name, 0, 20);
        strncpy(master_name, guild.master_name, 19);
        memcpy(response + offset, master_name, 20);
        offset += 20;

        /* guild_point */
        memcpy(response + offset, &guild.guild_point, 4);
        offset += 4;

        /* notice placeholder */
        char notice[64] = "Welcome to our guild!";
        memcpy(response + offset, notice, 64);
        offset += 64;

        printf("[GUILD] Returning guild %s (id=%u, lv=%u, members=%u)\n",
               guild.guild_name, guild.guild_id, guild.level, guild.member_count);
    } else {
        s->guild_id = 0;
        printf("[GUILD] Character %u is not in any guild\n", s->character_id);
    }

    return send_response(s, 107, response, offset);
}

/* msg_no=108: GUILD_INVITE */
static int handle_guild_invite(ClientSession* s, const uint8_t* data, int len) {
    printf("[GUILD] GUILD_INVITE from fd=%d\n", s->fd);
    if (len < 4 || s->guild_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 108, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);

    /* Find and notify target */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == target_id) {
            uint8_t notify[64];
            memset(notify, 0, sizeof(notify));
            memcpy(notify + 4, &s->guild_id, 4);
            memcpy(notify + 8, &s->character_id, 4);
            memcpy(notify + 12, s->character_name, 20);
            send_response(&g_sessions[i], 108, notify, 32);
            break;
        }
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 108, response, 8);
}

/* msg_no=109: GUILD_JOIN */
static int handle_guild_join(ClientSession* s, const uint8_t* data, int len) {
    printf("[GUILD] GUILD_JOIN from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 109, error, 4);
    }

    /* Already in a guild */
    if (s->guild_id > 0) {
        printf("[GUILD] Already in guild %u, cannot join another\n", s->guild_id);
        uint8_t error[4] = {2, 0, 0, 0}; /* Already in guild */
        return send_response(s, 109, error, 4);
    }

    uint32_t guild_id;
    memcpy(&guild_id, data, 4);

    /* Add to guild_member table (grade=5 = normal member) */
    if (db_add_guild_member(guild_id, s->character_id, s->character_name, 5) < 0) {
        printf("[GUILD] Failed to add member to database\n");
        uint8_t error[4] = {3, 0, 0, 0}; /* Database error */
        return send_response(s, 109, error, 4);
    }

    /* Update member count */
    db_update_guild_member_count(guild_id, 1);

    /* Update character's guild_id */
    db_update_character_guild(s->character_id, guild_id);

    s->guild_id = guild_id;
    printf("[GUILD] %s joined guild %u\n", s->character_name, guild_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &guild_id, 4);
    return send_response(s, 109, response, 8);
}

/* msg_no=110: GUILD_LEAVE */
static int handle_guild_leave(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GUILD] GUILD_LEAVE from fd=%d\n", s->fd);

    if (s->guild_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 110, error, 4);
    }

    uint32_t old_guild = s->guild_id;

    /* Remove from guild_member table */
    if (db_remove_guild_member(old_guild, s->character_id) < 0) {
        printf("[GUILD] Failed to remove member from database\n");
        /* Continue anyway */
    }

    /* Update member count */
    db_update_guild_member_count(old_guild, -1);

    /* Update character's guild_id */
    db_update_character_guild(s->character_id, 0);

    s->guild_id = 0;
    printf("[GUILD] Left guild %u\n", old_guild);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &old_guild, 4);
    return send_response(s, 110, response, 8);
}

/* msg_no=111: GUILD_KICK */
static int handle_guild_kick(ClientSession* s, const uint8_t* data, int len) {
    printf("[GUILD] GUILD_KICK from fd=%d\n", s->fd);
    if (len < 4 || s->guild_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 111, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);

    /* Remove from database */
    if (db_remove_guild_member(s->guild_id, target_id) < 0) {
        printf("[GUILD] Failed to remove kicked member from database\n");
    }

    /* Update member count */
    db_update_guild_member_count(s->guild_id, -1);

    /* Update kicked character's guild_id */
    db_update_character_guild(target_id, 0);

    /* Notify if online */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == target_id) {
            g_sessions[i].guild_id = 0;
            uint8_t notify[8] = {5, 0, 0, 0, 0, 0, 0, 0}; /* Kicked */
            send_response(&g_sessions[i], 111, notify, 8);
            break;
        }
    }

    printf("[GUILD] Kicked member %u from guild %u\n", target_id, s->guild_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 111, response, 8);
}

/* msg_no=112: GUILD_PROMOTE */
static int handle_guild_promote(ClientSession* s, const uint8_t* data, int len) {
    printf("[GUILD] GUILD_PROMOTE from fd=%d\n", s->fd);
    if (len < 4 || s->guild_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 112, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);
    printf("[GUILD] Promoted member %u\n", target_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 112, response, 8);
}

/* msg_no=113: GUILD_DEMOTE */
static int handle_guild_demote(ClientSession* s, const uint8_t* data, int len) {
    printf("[GUILD] GUILD_DEMOTE from fd=%d\n", s->fd);
    if (len < 4 || s->guild_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 113, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);
    printf("[GUILD] Demoted member %u\n", target_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 113, response, 8);
}

/* msg_no=114: GUILD_GET_MEMBERS */
static int handle_guild_get_members(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GUILD] GUILD_GET_MEMBERS from fd=%d\n", s->fd);

    uint8_t response[2048];
    memset(response, 0, sizeof(response));
    int offset = 0;

    /* Check if player has guild */
    if (s->guild_id == 0) {
        uint32_t result = 1;  /* Not in guild */
        memcpy(response, &result, 4);
        return send_response(s, 114, response, 4);
    }

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Get guild members from database */
    DBGuildMemberInfo members[50];
    int member_count = db_get_guild_members(s->guild_id, members, 50);

    response[offset++] = (uint8_t)member_count;

    for (int i = 0; i < member_count && offset < 2000; i++) {
        /* member_id */
        memcpy(response + offset, &members[i].charac_no, 4);
        offset += 4;

        /* name */
        char name[20];
        memset(name, 0, 20);
        strncpy(name, members[i].charac_name, 19);
        memcpy(response + offset, name, 20);
        offset += 20;

        /* level */
        response[offset++] = members[i].level;

        /* rank/grade */
        response[offset++] = members[i].grade;

        /* online status */
        response[offset++] = members[i].online;

        /* job */
        response[offset++] = members[i].job;
    }

    printf("[GUILD] Returning %d members for guild_id=%u\n", member_count, s->guild_id);
    return send_response(s, 114, response, offset);
}

/* msg_no=115: GUILD_SET_NOTICE */
static int handle_guild_set_notice(ClientSession* s, const uint8_t* data, int len) {
    printf("[GUILD] GUILD_SET_NOTICE from fd=%d\n", s->fd);
    if (len < 1 || s->guild_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 115, error, 4);
    }

    char notice[128];
    int notice_len = (len > 127) ? 127 : len;
    memcpy(notice, data, notice_len);
    notice[notice_len] = '\0';
    printf("[GUILD] Setting notice: %s\n", notice);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 115, response, 4);
}

/* msg_no=116: GUILD_GET_APPLICATIONS */
static int handle_guild_get_applications(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GUILD] GUILD_GET_APPLICATIONS from fd=%d\n", s->fd);

    uint8_t response[64];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t app_count = 0;
    response[offset++] = app_count;

    return send_response(s, 116, response, offset);
}

/* msg_no=117: GUILD_RESPOND_APPLICATION */
static int handle_guild_respond_application(ClientSession* s, const uint8_t* data, int len) {
    printf("[GUILD] GUILD_RESPOND_APPLICATION from fd=%d\n", s->fd);
    if (len < 5 || s->guild_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 117, error, 4);
    }

    uint32_t applicant_id;
    uint8_t accept;
    memcpy(&applicant_id, data, 4);
    accept = data[4];

    printf("[GUILD] %s application from %u\n", accept ? "Accepted" : "Rejected", applicant_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &applicant_id, 4);
    return send_response(s, 117, response, 8);
}

/*==============================================================================
 * Party System (msg_no 120-128)
 *============================================================================*/

/* msg_no=120: PARTY_CREATE */
static int handle_party_create(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PARTY] PARTY_CREATE from fd=%d\n", s->fd);

    if (s->party_id != 0) {
        uint8_t error[4] = {1, 0, 0, 0}; /* Already in party */
        return send_response(s, 120, error, 4);
    }

    PartyInfo* party = create_party(s->character_id);
    if (!party) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Cannot create party */
        return send_response(s, 120, error, 4);
    }

    s->party_id = party->party_id;

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &party->party_id, 4);
    return send_response(s, 120, response, 8);
}

/* msg_no=121: PARTY_INVITE */
static int handle_party_invite(ClientSession* s, const uint8_t* data, int len) {
    printf("[PARTY] PARTY_INVITE from fd=%d\n", s->fd);
    if (len < 4 || s->party_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 121, error, 4);
    }

    PartyInfo* party = find_party(s->party_id);
    if (!party || party->leader_id != s->character_id) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not leader */
        return send_response(s, 121, error, 4);
    }

    if (party->member_count >= MAX_PARTY_MEMBERS) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Party full */
        return send_response(s, 121, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);

    /* Find and notify target */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == target_id) {
            if (g_sessions[i].party_id != 0) {
                uint8_t error[4] = {4, 0, 0, 0}; /* Target in party */
                return send_response(s, 121, error, 4);
            }
            uint8_t notify[64];
            memset(notify, 0, sizeof(notify));
            memcpy(notify + 4, &s->party_id, 4);
            memcpy(notify + 8, &s->character_id, 4);
            memcpy(notify + 12, s->character_name, 20);
            send_response(&g_sessions[i], 121, notify, 32);
            printf("[PARTY] Invited %u to party %u\n", target_id, s->party_id);
            break;
        }
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 121, response, 8);
}

/* msg_no=122: PARTY_JOIN */
static int handle_party_join(ClientSession* s, const uint8_t* data, int len) {
    printf("[PARTY] PARTY_JOIN from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 122, error, 4);
    }

    if (s->party_id != 0) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Already in party */
        return send_response(s, 122, error, 4);
    }

    uint32_t party_id;
    memcpy(&party_id, data, 4);

    PartyInfo* party = find_party(party_id);
    if (!party) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Party not found */
        return send_response(s, 122, error, 4);
    }

    if (party_add_member(party, s->character_id) < 0) {
        uint8_t error[4] = {4, 0, 0, 0}; /* Cannot join */
        return send_response(s, 122, error, 4);
    }

    s->party_id = party_id;

    /* Notify all party members */
    uint8_t notify[32];
    memset(notify, 0, sizeof(notify));
    uint32_t action = 1; /* Member joined */
    memcpy(notify, &action, 4);
    memcpy(notify + 4, &s->character_id, 4);
    memcpy(notify + 8, s->character_name, 20);
    party_notify_members(party, 125, notify, 28);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &party_id, 4);
    return send_response(s, 122, response, 8);
}

/* msg_no=123: PARTY_LEAVE */
static int handle_party_leave(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PARTY] PARTY_LEAVE from fd=%d\n", s->fd);

    if (s->party_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 123, error, 4);
    }

    uint32_t old_party = s->party_id;
    PartyInfo* party = find_party(s->party_id);

    s->party_id = 0;

    if (party) {
        party_remove_member(party, s->character_id);

        /* Notify remaining members */
        if (party->active) {
            uint8_t notify[12];
            memset(notify, 0, sizeof(notify));
            uint32_t action = 2; /* Member left */
            memcpy(notify, &action, 4);
            memcpy(notify + 4, &s->character_id, 4);
            party_notify_members(party, 125, notify, 12);
        }
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &old_party, 4);
    return send_response(s, 123, response, 8);
}

/* msg_no=124: PARTY_KICK */
static int handle_party_kick(ClientSession* s, const uint8_t* data, int len) {
    printf("[PARTY] PARTY_KICK from fd=%d\n", s->fd);
    if (len < 4 || s->party_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 124, error, 4);
    }

    PartyInfo* party = find_party(s->party_id);
    if (!party || party->leader_id != s->character_id) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not leader */
        return send_response(s, 124, error, 4);
    }

    uint32_t target_id;
    memcpy(&target_id, data, 4);

    if (target_id == s->character_id) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Cannot kick self */
        return send_response(s, 124, error, 4);
    }

    /* Remove from party */
    party_remove_member(party, target_id);

    /* Find and notify kicked player */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == target_id) {
            g_sessions[i].party_id = 0;
            uint8_t notify[8] = {5, 0, 0, 0, 0, 0, 0, 0}; /* Kicked */
            send_response(&g_sessions[i], 124, notify, 8);
            break;
        }
    }

    /* Notify remaining members */
    uint8_t member_notify[12];
    memset(member_notify, 0, sizeof(member_notify));
    uint32_t action = 3; /* Member kicked */
    memcpy(member_notify, &action, 4);
    memcpy(member_notify + 4, &target_id, 4);
    party_notify_members(party, 125, member_notify, 12);

    printf("[PARTY] Kicked member %u\n", target_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    return send_response(s, 124, response, 8);
}

/* msg_no=125: PARTY_GET_INFO */
static int handle_party_get_info(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PARTY] PARTY_GET_INFO from fd=%d\n", s->fd);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    PartyInfo* party = find_party(s->party_id);

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &s->party_id, 4);
    offset += 4;

    if (!party || !party->active) {
        /* No party or inactive */
        response[offset++] = 0; /* member_count = 0 */
        return send_response(s, 125, response, offset);
    }

    response[offset++] = (uint8_t)party->member_count;

    for (int i = 0; i < (int)party->member_count && i < MAX_PARTY_MEMBERS; i++) {
        uint32_t member_id = party->members[i];
        memcpy(response + offset, &member_id, 4);
        offset += 4;

        /* Find member info */
        char name[20] = "Unknown";
        uint8_t level = 1;
        uint8_t job = 0;

        for (int j = 0; j < MAX_SESSIONS; j++) {
            if (g_sessions[j].active && g_sessions[j].character_id == member_id) {
                memcpy(name, g_sessions[j].character_name, 20);
                /* Find level/job from character data */
                for (int k = 0; k < g_sessions[j].character_count; k++) {
                    if (g_sessions[j].characters[k].charac_no == member_id) {
                        level = g_sessions[j].characters[k].level;
                        job = g_sessions[j].characters[k].job;
                        break;
                    }
                }
                break;
            }
        }

        memcpy(response + offset, name, 20);
        offset += 20;

        response[offset++] = level;
        response[offset++] = job;

        uint8_t is_leader = (member_id == party->leader_id) ? 1 : 0;
        response[offset++] = is_leader;
    }

    return send_response(s, 125, response, offset);
}

/* msg_no=126: PARTY_CHANGE_LEADER */
static int handle_party_change_leader(ClientSession* s, const uint8_t* data, int len) {
    printf("[PARTY] PARTY_CHANGE_LEADER from fd=%d\n", s->fd);
    if (len < 4 || s->party_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 126, error, 4);
    }

    PartyInfo* party = find_party(s->party_id);
    if (!party || party->leader_id != s->character_id) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not leader */
        return send_response(s, 126, error, 4);
    }

    uint32_t new_leader_id;
    memcpy(&new_leader_id, data, 4);

    /* Verify new leader is in party */
    int found = 0;
    for (int i = 0; i < (int)party->member_count; i++) {
        if (party->members[i] == new_leader_id) {
            found = 1;
            break;
        }
    }

    if (!found) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Not in party */
        return send_response(s, 126, error, 4);
    }

    party->leader_id = new_leader_id;
    printf("[PARTY] Changed leader to %u\n", new_leader_id);

    /* Notify all members */
    uint8_t notify[12];
    memset(notify, 0, sizeof(notify));
    uint32_t action = 4; /* Leader changed */
    memcpy(notify, &action, 4);
    memcpy(notify + 4, &new_leader_id, 4);
    party_notify_members(party, 125, notify, 12);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &new_leader_id, 4);
    return send_response(s, 126, response, 8);
}
/* msg_no=127: PARTY_SET_LOOT_MODE */
static int handle_party_set_loot_mode(ClientSession* s, const uint8_t* data, int len) {
    printf("[PARTY] PARTY_SET_LOOT_MODE from fd=%d\n", s->fd);
    if (len < 1 || s->party_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 127, error, 4);
    }

    uint8_t loot_mode = data[0]; /* 0=FFA, 1=Round-robin, 2=Leader */
    printf("[PARTY] Set loot mode to %d\n", loot_mode);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    response[4] = loot_mode;
    return send_response(s, 127, response, 8);
}

/* msg_no=128: PARTY_CHAT */
static int handle_party_chat(ClientSession* s, const uint8_t* data, int len) {
    printf("[PARTY] PARTY_CHAT from fd=%d\n", s->fd);
    if (len < 1 || s->party_id == 0) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 128, error, 4);
    }

    /* Broadcast to party members */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].party_id == s->party_id && &g_sessions[i] != s) {
            uint8_t msg[256];
            memset(msg, 0, sizeof(msg));
            memcpy(msg + 4, &s->character_id, 4);
            memcpy(msg + 8, s->character_name, 20);
            int msg_len = (len > 200) ? 200 : len;
            memcpy(msg + 28, data, msg_len);
            send_response(&g_sessions[i], 128, msg, 28 + msg_len);
        }
    }

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 128, response, 4);
}

/*==============================================================================
 * PVP/Arena System (msg_no 130-138)
 *============================================================================*/

/* msg_no=130: PVP_GET_INFO */
static int handle_pvp_get_info(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PVP] PVP_GET_INFO from fd=%d\n", s->fd);

    uint8_t response[32];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint32_t pvp_points = 1500;
    memcpy(response + offset, &pvp_points, 4);
    offset += 4;

    uint32_t wins = 42;
    memcpy(response + offset, &wins, 4);
    offset += 4;

    uint32_t losses = 18;
    memcpy(response + offset, &losses, 4);
    offset += 4;

    uint8_t rank = 3; /* Gold */
    response[offset++] = rank;

    return send_response(s, 130, response, offset);
}

/* msg_no=131: PVP_QUEUE */
static int handle_pvp_queue(ClientSession* s, const uint8_t* data, int len) {
    printf("[PVP] PVP_QUEUE from fd=%d\n", s->fd);

    uint8_t mode = 0; /* 1v1 */
    if (len >= 1) mode = data[0];

    printf("[PVP] Queued for mode %d\n", mode);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    response[4] = mode;
    uint16_t queue_pos = 5;
    memcpy(response + 5, &queue_pos, 2);
    return send_response(s, 131, response, 8);
}

/* msg_no=132: PVP_CANCEL_QUEUE */
static int handle_pvp_cancel_queue(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PVP] PVP_CANCEL_QUEUE from fd=%d\n", s->fd);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 132, response, 4);
}

/* msg_no=133: PVP_ACCEPT_MATCH */
static int handle_pvp_accept_match(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PVP] PVP_ACCEPT_MATCH from fd=%d\n", s->fd);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 133, response, 4);
}

/* msg_no=134: PVP_DECLINE_MATCH */
static int handle_pvp_decline_match(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PVP] PVP_DECLINE_MATCH from fd=%d\n", s->fd);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 134, response, 4);
}

/* msg_no=135: PVP_READY */
static int handle_pvp_ready(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PVP] PVP_READY from fd=%d\n", s->fd);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 135, response, 4);
}

/* msg_no=136: PVP_ATTACK */
static int handle_pvp_attack(ClientSession* s, const uint8_t* data, int len) {
    printf("[PVP] PVP_ATTACK from fd=%d\n", s->fd);
    if (len < 8) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 136, error, 4);
    }

    uint32_t target_id;
    uint32_t damage;
    memcpy(&target_id, data, 4);
    memcpy(&damage, data + 4, 4);

    printf("[PVP] Attack target %u for %u damage\n", target_id, damage);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &target_id, 4);
    memcpy(response + 8, &damage, 4);
    return send_response(s, 136, response, 16);
}

/* msg_no=137: PVP_LEAVE */
static int handle_pvp_leave(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PVP] PVP_LEAVE from fd=%d\n", s->fd);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 137, response, 4);
}

/* msg_no=138: PVP_GET_RANKING */
static int handle_pvp_get_ranking(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PVP] PVP_GET_RANKING from fd=%d\n", s->fd);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Mock: top 5 */
    uint8_t count = 5;
    response[offset++] = count;

    for (int i = 0; i < 5; i++) {
        uint8_t rank = i + 1;
        response[offset++] = rank;

        char name[20];
        sprintf(name, "PvPKing%d", i + 1);
        memcpy(response + offset, name, 20);
        offset += 20;

        uint32_t points = 2500 - i * 200;
        memcpy(response + offset, &points, 4);
        offset += 4;

        uint16_t wins = 100 - i * 15;
        memcpy(response + offset, &wins, 2);
        offset += 2;
    }

    return send_response(s, 138, response, offset);
}

/*==============================================================================
 * Ranking System (msg_no 140-145)
 *============================================================================*/

/* msg_no=140: RANKING_GET_LEVEL */
static int handle_ranking_get_level(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[RANK] RANKING_GET_LEVEL from fd=%d\n", s->fd);

    /* Query real data from database */
    DBRankingEntry entries[20];
    int db_count = db_get_level_ranking(entries, 20);

    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = (db_count > 0) ? (uint8_t)db_count : 0;
    response[offset++] = count;

    for (int i = 0; i < count && i < 20; i++) {
        uint8_t rank = i + 1;
        response[offset++] = rank;

        memcpy(response + offset, entries[i].charac_name, 20);
        offset += 20;

        response[offset++] = entries[i].lev;
        response[offset++] = entries[i].job;
    }

    printf("[RANK] Returning %d level ranking entries\n", count);
    return send_response(s, 140, response, offset);
}

/* msg_no=141: RANKING_GET_WEALTH */
static int handle_ranking_get_wealth(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[RANK] RANKING_GET_WEALTH from fd=%d\n", s->fd);

    /* Query real data from database */
    DBRankingEntry entries[20];
    int db_count = db_get_wealth_ranking(entries, 20);

    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = (db_count > 0) ? (uint8_t)db_count : 0;
    response[offset++] = count;

    for (int i = 0; i < count && i < 20; i++) {
        uint8_t rank = i + 1;
        response[offset++] = rank;

        memcpy(response + offset, entries[i].charac_name, 20);
        offset += 20;

        memcpy(response + offset, &entries[i].money, 4);
        offset += 4;
    }

    printf("[RANK] Returning %d wealth ranking entries\n", count);
    return send_response(s, 141, response, offset);
}

/* msg_no=142: RANKING_GET_DUNGEON */
static int handle_ranking_get_dungeon(ClientSession* s, const uint8_t* data, int len) {
    printf("[RANK] RANKING_GET_DUNGEON from fd=%d\n", s->fd);

    uint32_t dungeon_id = 1;
    if (len >= 4) memcpy(&dungeon_id, data, 4);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &dungeon_id, 4);
    offset += 4;

    uint8_t count = 10;
    response[offset++] = count;

    for (int i = 0; i < 10; i++) {
        uint8_t rank = i + 1;
        response[offset++] = rank;

        char name[20];
        sprintf(name, "Speedrunner%d", i + 1);
        memcpy(response + offset, name, 20);
        offset += 20;

        uint32_t clear_time = 60 + i * 5; /* seconds */
        memcpy(response + offset, &clear_time, 4);
        offset += 4;
    }

    return send_response(s, 142, response, offset);
}

/* msg_no=143: RANKING_GET_MY_RANK */
static int handle_ranking_get_my_rank(ClientSession* s, const uint8_t* data, int len) {
    printf("[RANK] RANKING_GET_MY_RANK from fd=%d\n", s->fd);

    uint8_t rank_type = 0; /* 0=level, 1=wealth, 2=pvp */
    if (len >= 1) rank_type = data[0];

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    response[offset++] = rank_type;

    uint32_t my_rank = 42 + rank_type * 10;
    memcpy(response + offset, &my_rank, 4);
    offset += 4;

    return send_response(s, 143, response, offset);
}

/* msg_no=144: RANKING_GET_GUILD */
static int handle_ranking_get_guild(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[RANK] RANKING_GET_GUILD from fd=%d\n", s->fd);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = 10;
    response[offset++] = count;

    for (int i = 0; i < 10; i++) {
        uint8_t rank = i + 1;
        response[offset++] = rank;

        char name[20];
        sprintf(name, "TopGuild%d", i + 1);
        memcpy(response + offset, name, 20);
        offset += 20;

        uint8_t level = 10 - i / 2;
        response[offset++] = level;

        uint16_t members = 50 - i * 3;
        memcpy(response + offset, &members, 2);
        offset += 2;
    }

    return send_response(s, 144, response, offset);
}

/* msg_no=145: RANKING_GET_ACHIEVEMENT */
static int handle_ranking_get_achievement(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[RANK] RANKING_GET_ACHIEVEMENT from fd=%d\n", s->fd);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = 10;
    response[offset++] = count;

    for (int i = 0; i < 10; i++) {
        uint8_t rank = i + 1;
        response[offset++] = rank;

        char name[20];
        sprintf(name, "Achiever%d", i + 1);
        memcpy(response + offset, name, 20);
        offset += 20;

        uint16_t points = 10000 - i * 500;
        memcpy(response + offset, &points, 2);
        offset += 2;
    }

    return send_response(s, 145, response, offset);
}

/*==============================================================================
 * Achievement System (msg_no 150-156)
 *============================================================================*/

/* msg_no=150: ACHIEVEMENT_GET_LIST */
static int handle_achievement_get_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[ACH] ACHIEVEMENT_GET_LIST from fd=%d\n", s->fd);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint16_t total_points = 2500;
    memcpy(response + offset, &total_points, 2);
    offset += 2;

    uint8_t count = 5;
    response[offset++] = count;

    for (int i = 0; i < 5; i++) {
        uint32_t ach_id = 1000 + i;
        memcpy(response + offset, &ach_id, 4);
        offset += 4;

        uint8_t completed = (i < 3) ? 1 : 0;
        response[offset++] = completed;

        uint8_t progress = completed ? 100 : (i * 20);
        response[offset++] = progress;

        uint16_t points = 100 * (i + 1);
        memcpy(response + offset, &points, 2);
        offset += 2;
    }

    return send_response(s, 150, response, offset);
}

/* msg_no=151: ACHIEVEMENT_GET_DETAIL */
static int handle_achievement_get_detail(ClientSession* s, const uint8_t* data, int len) {
    printf("[ACH] ACHIEVEMENT_GET_DETAIL from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 151, error, 4);
    }

    uint32_t ach_id;
    memcpy(&ach_id, data, 4);

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &ach_id, 4);
    offset += 4;

    char name[32] = "Dungeon Master";
    memcpy(response + offset, name, 32);
    offset += 32;

    char desc[64] = "Clear 100 dungeons";
    memcpy(response + offset, desc, 64);
    offset += 64;

    uint16_t points = 500;
    memcpy(response + offset, &points, 2);
    offset += 2;

    uint8_t progress = 75;
    response[offset++] = progress;

    return send_response(s, 151, response, offset);
}

/* msg_no=152: ACHIEVEMENT_CLAIM_REWARD */
static int handle_achievement_claim_reward(ClientSession* s, const uint8_t* data, int len) {
    printf("[ACH] ACHIEVEMENT_CLAIM_REWARD from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 152, error, 4);
    }

    uint32_t ach_id;
    memcpy(&ach_id, data, 4);

    /* Give reward */
    s->inventory.money += 10000;
    printf("[ACH] Claimed reward for achievement %u\n", ach_id);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &ach_id, 4);
    uint32_t gold = 10000;
    memcpy(response + 8, &gold, 4);
    return send_response(s, 152, response, 16);
}

/* msg_no=153: ACHIEVEMENT_GET_PROGRESS */
static int handle_achievement_get_progress(ClientSession* s, const uint8_t* data, int len) {
    printf("[ACH] ACHIEVEMENT_GET_PROGRESS from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 153, error, 4);
    }

    uint32_t ach_id;
    memcpy(&ach_id, data, 4);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &ach_id, 4);
    uint8_t progress = 65;
    response[8] = progress;
    return send_response(s, 153, response, 12);
}

/* msg_no=154: ACHIEVEMENT_GET_CATEGORIES */
static int handle_achievement_get_categories(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[ACH] ACHIEVEMENT_GET_CATEGORIES from fd=%d\n", s->fd);

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = 5;
    response[offset++] = count;

    const char* categories[] = {"Combat", "Exploration", "Social", "Collection", "Special"};
    for (int i = 0; i < 5; i++) {
        uint8_t cat_id = i;
        response[offset++] = cat_id;
        memcpy(response + offset, categories[i], 16);
        offset += 16;
    }

    return send_response(s, 154, response, offset);
}

/* msg_no=155: ACHIEVEMENT_GET_BY_CATEGORY */
static int handle_achievement_get_by_category(ClientSession* s, const uint8_t* data, int len) {
    printf("[ACH] ACHIEVEMENT_GET_BY_CATEGORY from fd=%d\n", s->fd);

    uint8_t category = 0;
    if (len >= 1) category = data[0];

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    response[offset++] = category;

    uint8_t count = 3;
    response[offset++] = count;

    for (int i = 0; i < 3; i++) {
        uint32_t ach_id = category * 100 + i;
        memcpy(response + offset, &ach_id, 4);
        offset += 4;

        uint8_t completed = (i == 0) ? 1 : 0;
        response[offset++] = completed;
    }

    return send_response(s, 155, response, offset);
}

/* msg_no=156: ACHIEVEMENT_GET_RECENT */
static int handle_achievement_get_recent(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[ACH] ACHIEVEMENT_GET_RECENT from fd=%d\n", s->fd);

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = 3;
    response[offset++] = count;

    for (int i = 0; i < 3; i++) {
        uint32_t ach_id = 1000 + i;
        memcpy(response + offset, &ach_id, 4);
        offset += 4;

        uint32_t timestamp = (uint32_t)time(NULL) - i * 3600;
        memcpy(response + offset, &timestamp, 4);
        offset += 4;
    }

    return send_response(s, 156, response, offset);
}

/*==============================================================================
 * Title System (msg_no 160-165)
 *============================================================================*/

/* msg_no=160: TITLE_GET_LIST */
static int handle_title_get_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TITLE] TITLE_GET_LIST from fd=%d\n", s->fd);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = 5;
    response[offset++] = count;

    const char* titles[] = {"Beginner", "Adventurer", "Veteran", "Champion", "Legend"};
    for (int i = 0; i < 5; i++) {
        uint32_t title_id = 100 + i;
        memcpy(response + offset, &title_id, 4);
        offset += 4;

        memcpy(response + offset, titles[i], 20);
        offset += 20;

        uint8_t unlocked = (i < 3) ? 1 : 0;
        response[offset++] = unlocked;
    }

    return send_response(s, 160, response, offset);
}

/* msg_no=161: TITLE_EQUIP */
static int handle_title_equip(ClientSession* s, const uint8_t* data, int len) {
    printf("[TITLE] TITLE_EQUIP from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 161, error, 4);
    }

    uint32_t title_id;
    memcpy(&title_id, data, 4);
    printf("[TITLE] Equipped title %u\n", title_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &title_id, 4);
    return send_response(s, 161, response, 8);
}

/* msg_no=162: TITLE_UNEQUIP */
static int handle_title_unequip(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TITLE] TITLE_UNEQUIP from fd=%d\n", s->fd);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 162, response, 4);
}

/* msg_no=163: TITLE_GET_DETAIL */
static int handle_title_get_detail(ClientSession* s, const uint8_t* data, int len) {
    printf("[TITLE] TITLE_GET_DETAIL from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 163, error, 4);
    }

    uint32_t title_id;
    memcpy(&title_id, data, 4);

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &title_id, 4);
    offset += 4;

    char name[20] = "Champion";
    memcpy(response + offset, name, 20);
    offset += 20;

    char desc[64] = "Win 100 PvP matches";
    memcpy(response + offset, desc, 64);
    offset += 64;

    /* Stats bonus */
    uint16_t str_bonus = 10;
    uint16_t int_bonus = 5;
    memcpy(response + offset, &str_bonus, 2);
    offset += 2;
    memcpy(response + offset, &int_bonus, 2);
    offset += 2;

    return send_response(s, 163, response, offset);
}

/* msg_no=164: TITLE_GET_CURRENT */
static int handle_title_get_current(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TITLE] TITLE_GET_CURRENT from fd=%d\n", s->fd);

    uint8_t response[28];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint32_t title_id = 102; /* Veteran */
    memcpy(response + offset, &title_id, 4);
    offset += 4;

    char name[20] = "Veteran";
    memcpy(response + offset, name, 20);
    offset += 20;

    return send_response(s, 164, response, offset);
}

/* msg_no=165: TITLE_GET_UNLOCKABLE */
static int handle_title_get_unlockable(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[TITLE] TITLE_GET_UNLOCKABLE from fd=%d\n", s->fd);

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = 2;
    response[offset++] = count;

    for (int i = 0; i < 2; i++) {
        uint32_t title_id = 103 + i;
        memcpy(response + offset, &title_id, 4);
        offset += 4;

        uint8_t progress = 60 + i * 20;
        response[offset++] = progress;
    }

    return send_response(s, 165, response, offset);
}


/*==============================================================================
 * Auction System (msg_no 170-178)
 *============================================================================*/

/* msg_no=170: AUCTION_SEARCH */
static int handle_auction_search(ClientSession* s, const uint8_t* data, int len) {
    printf("[AUCTION] AUCTION_SEARCH from fd=%d\n", s->fd);

    uint32_t item_id = 0;
    uint32_t min_price = 0;
    uint32_t max_price = 0;
    if (len >= 4) memcpy(&item_id, data, 4);
    if (len >= 8) memcpy(&min_price, data + 4, 4);
    if (len >= 12) memcpy(&max_price, data + 8, 4);

    AuctionListing* results[20];
    int count = search_auction_listings(item_id, min_price, max_price, results, 20);

    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    response[offset++] = (uint8_t)count;

    for (int i = 0; i < count && offset < 480; i++) {
        AuctionListing* a = results[i];
        memcpy(response + offset, &a->listing_id, 4);
        offset += 4;
        memcpy(response + offset, &a->item_id, 4);
        offset += 4;
        memcpy(response + offset, &a->price, 4);
        offset += 4;
        memcpy(response + offset, a->seller_name, 20);
        offset += 20;
        uint32_t remaining = a->expire_time - (uint32_t)time(NULL);
        memcpy(response + offset, &remaining, 4);
        offset += 4;
    }

    printf("[AUCTION] Search returned %d results\n", count);
    return send_response(s, 170, response, offset);
}

/* msg_no=171: AUCTION_LIST_ITEM */
static int handle_auction_list_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[AUCTION] AUCTION_LIST_ITEM from fd=%d\n", s->fd);
    if (len < 12) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 171, error, 4);
    }

    uint32_t item_uid;
    uint32_t price;
    uint32_t duration; /* hours */
    memcpy(&item_uid, data, 4);
    memcpy(&price, data + 4, 4);
    memcpy(&duration, data + 8, 4);

    /* Validate duration (1-48 hours) */
    if (duration < 1) duration = 1;
    if (duration > 48) duration = 48;

    /* Listing fee (5% of price, min 1000) */
    uint32_t fee = price / 20;
    if (fee < 1000) fee = 1000;

    if (s->inventory.money < fee) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not enough gold */
        return send_response(s, 171, error, 4);
    }

    /* Get item info */
    uint32_t item_id = 0;
    if (db_get_item_at_slot(s->character_id, item_uid, &item_id) < 0) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Item not found */
        return send_response(s, 171, error, 4);
    }

    /* Create listing */
    AuctionListing* listing = create_auction_listing(
        s->character_id, s->character_name, item_uid, item_id, price, duration);

    if (!listing) {
        uint8_t error[4] = {4, 0, 0, 0}; /* Cannot create listing */
        return send_response(s, 171, error, 4);
    }

    /* Deduct fee and persist gold */
    s->inventory.money -= fee;
    db_update_gold(s->character_id, s->inventory.money);

    /* Remove item from seller's inventory */
    db_delete_item(s->character_id, item_uid);

    printf("[AUCTION] Listed item %u (id=%u) for %u gold (fee: %u, listing_id=%u)\n",
           item_uid, item_id, price, fee, listing->listing_id);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &listing->listing_id, 4);
    memcpy(response + 8, &fee, 4);
    return send_response(s, 171, response, 12);
}

/* msg_no=172: AUCTION_CANCEL */
static int handle_auction_cancel(ClientSession* s, const uint8_t* data, int len) {
    printf("[AUCTION] AUCTION_CANCEL from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 172, error, 4);
    }

    uint32_t listing_id;
    memcpy(&listing_id, data, 4);

    AuctionListing* listing = find_auction_listing(listing_id);
    if (!listing || listing->seller_id != s->character_id) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not found or not owner */
        return send_response(s, 172, error, 4);
    }

    /* Return item to seller */
    int free_slot = db_find_free_slot(s->character_id, 11);
    if (free_slot >= 0 && free_slot <= 200) {
        db_add_item(s->character_id, listing->item_id, free_slot, 0);
    }

    /* Cancel listing */
    listing->active = 0;
    printf("[AUCTION] Cancelled listing %u, returned item %u\n", listing_id, listing->item_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &listing_id, 4);
    return send_response(s, 172, response, 8);
}

/* msg_no=173: AUCTION_BUY */
static int handle_auction_buy(ClientSession* s, const uint8_t* data, int len) {
    printf("[AUCTION] AUCTION_BUY from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 173, error, 4);
    }

    uint32_t listing_id;
    memcpy(&listing_id, data, 4);

    AuctionListing* listing = find_auction_listing(listing_id);
    if (!listing) {
        uint8_t error[4] = {2, 0, 0, 0}; /* Not found */
        return send_response(s, 173, error, 4);
    }

    if (listing->seller_id == s->character_id) {
        uint8_t error[4] = {3, 0, 0, 0}; /* Cannot buy own listing */
        return send_response(s, 173, error, 4);
    }

    if (s->inventory.money < listing->price) {
        uint8_t error[4] = {4, 0, 0, 0}; /* Not enough gold */
        return send_response(s, 173, error, 4);
    }

    /* Deduct gold from buyer */
    s->inventory.money -= listing->price;
    db_update_gold(s->character_id, s->inventory.money);

    /* Add item to buyer */
    int free_slot = db_find_free_slot(s->character_id, 11);
    if (free_slot >= 0 && free_slot <= 200) {
        db_add_item(s->character_id, listing->item_id, free_slot, 0);
    }

    /* Credit seller (seller collects later or via auction_collect_gold) */
    /* For simplicity, directly add to seller if online */
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].character_id == listing->seller_id) {
            g_sessions[i].inventory.money += listing->price;
            db_update_gold(listing->seller_id, g_sessions[i].inventory.money);
            break;
        }
    }

    uint32_t price = listing->price;
    uint32_t item_id = listing->item_id;

    /* Mark listing as sold */
    listing->active = 0;
    printf("[AUCTION] Bought listing %u (item=%u) for %u gold\n", listing_id, item_id, price);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &listing_id, 4);
    memcpy(response + 8, &price, 4);
    memcpy(response + 12, &item_id, 4);
    return send_response(s, 173, response, 16);
}

/* msg_no=174: AUCTION_GET_MY_LISTINGS */
static int handle_auction_get_my_listings(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[AUCTION] AUCTION_GET_MY_LISTINGS from fd=%d\n", s->fd);

    AuctionListing* results[20];
    int count = get_seller_listings(s->character_id, results, 20);

    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    response[offset++] = (uint8_t)count;

    for (int i = 0; i < count && offset < 480; i++) {
        AuctionListing* a = results[i];
        memcpy(response + offset, &a->listing_id, 4);
        offset += 4;
        memcpy(response + offset, &a->item_id, 4);
        offset += 4;
        memcpy(response + offset, &a->price, 4);
        offset += 4;
        uint32_t remaining = a->expire_time - (uint32_t)time(NULL);
        memcpy(response + offset, &remaining, 4);
        offset += 4;
    }

    printf("[AUCTION] Returned %d listings for seller %u\n", count, s->character_id);
    return send_response(s, 174, response, offset);
}
/* msg_no=175: AUCTION_GET_HISTORY */
static int handle_auction_get_history(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[AUCTION] AUCTION_GET_HISTORY from fd=%d\n", s->fd);

    uint8_t response[128];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    uint8_t count = 2;
    response[offset++] = count;

    for (int i = 0; i < 2; i++) {
        uint32_t item_id = 10000 + i * 50;
        memcpy(response + offset, &item_id, 4);
        offset += 4;

        uint32_t price = 75000 + i * 25000;
        memcpy(response + offset, &price, 4);
        offset += 4;

        uint8_t sold = (i == 0) ? 1 : 0;
        response[offset++] = sold;

        uint32_t timestamp = (uint32_t)time(NULL) - i * 86400;
        memcpy(response + offset, &timestamp, 4);
        offset += 4;
    }

    return send_response(s, 175, response, offset);
}

/* msg_no=176: AUCTION_BID */
static int handle_auction_bid(ClientSession* s, const uint8_t* data, int len) {
    printf("[AUCTION] AUCTION_BID from fd=%d\n", s->fd);
    if (len < 8) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 176, error, 4);
    }

    uint32_t listing_id;
    uint32_t bid_amount;
    memcpy(&listing_id, data, 4);
    memcpy(&bid_amount, data + 4, 4);

    if (s->inventory.money < bid_amount) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 176, error, 4);
    }

    printf("[AUCTION] Bid %u on listing %u\n", bid_amount, listing_id);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &listing_id, 4);
    memcpy(response + 8, &bid_amount, 4);
    return send_response(s, 176, response, 12);
}

/* msg_no=177: AUCTION_GET_PRICE_HISTORY */
static int handle_auction_get_price_history(ClientSession* s, const uint8_t* data, int len) {
    printf("[AUCTION] AUCTION_GET_PRICE_HISTORY from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 177, error, 4);
    }

    uint32_t item_id;
    memcpy(&item_id, data, 4);

    uint8_t response[64];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &item_id, 4);
    offset += 4;

    uint32_t avg_price = 85000;
    uint32_t min_price = 50000;
    uint32_t max_price = 120000;
    memcpy(response + offset, &avg_price, 4);
    offset += 4;
    memcpy(response + offset, &min_price, 4);
    offset += 4;
    memcpy(response + offset, &max_price, 4);
    offset += 4;

    return send_response(s, 177, response, offset);
}

/* msg_no=178: AUCTION_COLLECT_GOLD */
static int handle_auction_collect_gold(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[AUCTION] AUCTION_COLLECT_GOLD from fd=%d\n", s->fd);

    /* Mock: collect 100000 gold from sold items */
    uint32_t collected = 100000;
    s->inventory.money += collected;

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &collected, 4);
    return send_response(s, 178, response, 8);
}

/*==============================================================================
 * Storage/Warehouse System (msg_no 180-185)
 *============================================================================*/

/* msg_no=180: STORAGE_GET_LIST */
static int handle_storage_get_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[STORAGE] STORAGE_GET_LIST from fd=%d\n", s->fd);

    uint8_t response[256];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Get storage from database using account_id as m_id */
    DBStorageInfo storage;
    if (db_get_storage(s->account_id, &storage) < 0) {
        storage.money = 0;
        storage.capacity = 8;
    }

    memcpy(response + offset, &storage.money, 4);
    offset += 4;

    response[offset++] = storage.capacity;

    /* Item count - cargo blob needs special handling, return 0 for now */
    uint8_t item_count = 0;
    response[offset++] = item_count;

    printf("[STORAGE] Returning storage for m_id=%u: gold=%u, capacity=%u\n",
           s->account_id, storage.money, storage.capacity);
    return send_response(s, 180, response, offset);
}

/* msg_no=181: STORAGE_DEPOSIT_ITEM */
static int handle_storage_deposit_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[STORAGE] STORAGE_DEPOSIT_ITEM from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 181, error, 4);
    }

    uint32_t item_uid;
    memcpy(&item_uid, data, 4);
    printf("[STORAGE] Depositing item %u\n", item_uid);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &item_uid, 4);
    return send_response(s, 181, response, 8);
}

/* msg_no=182: STORAGE_WITHDRAW_ITEM */
static int handle_storage_withdraw_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[STORAGE] STORAGE_WITHDRAW_ITEM from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 182, error, 4);
    }

    uint32_t item_uid;
    memcpy(&item_uid, data, 4);
    printf("[STORAGE] Withdrawing item %u\n", item_uid);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &item_uid, 4);
    return send_response(s, 182, response, 8);
}

/* msg_no=183: STORAGE_DEPOSIT_GOLD */
static int handle_storage_deposit_gold(ClientSession* s, const uint8_t* data, int len) {
    printf("[STORAGE] STORAGE_DEPOSIT_GOLD from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 183, error, 4);
    }

    uint32_t amount;
    memcpy(&amount, data, 4);

    if (amount > s->inventory.money) {
        amount = s->inventory.money;
    }

    if (amount > 0) {
        /* Deduct from character inventory */
        s->inventory.money -= amount;
        db_update_gold(s->character_id, s->inventory.money);

        /* Add to storage */
        db_update_storage_gold(s->account_id, (int32_t)amount);
        printf("[STORAGE] Deposited %u gold to storage (m_id=%u)\n", amount, s->account_id);
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &amount, 4);
    return send_response(s, 183, response, 8);
}

/* msg_no=184: STORAGE_WITHDRAW_GOLD */
static int handle_storage_withdraw_gold(ClientSession* s, const uint8_t* data, int len) {
    printf("[STORAGE] STORAGE_WITHDRAW_GOLD from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 184, error, 4);
    }

    uint32_t amount;
    memcpy(&amount, data, 4);

    /* Get actual storage gold */
    uint32_t storage_gold = db_get_storage_gold(s->account_id);
    if (amount > storage_gold) {
        amount = storage_gold;
    }

    if (amount > 0) {
        /* Deduct from storage */
        db_update_storage_gold(s->account_id, -(int32_t)amount);

        /* Add to character inventory */
        s->inventory.money += amount;
        db_update_gold(s->character_id, s->inventory.money);
        printf("[STORAGE] Withdrew %u gold from storage (m_id=%u)\n", amount, s->account_id);
    }

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &amount, 4);
    return send_response(s, 184, response, 8);
}

/* msg_no=185: STORAGE_EXPAND */
static int handle_storage_expand(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[STORAGE] STORAGE_EXPAND from fd=%d\n", s->fd);

    /* Expansion cost: 100000 gold */
    if (s->inventory.money < 100000) {
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 185, error, 4);
    }

    s->inventory.money -= 100000;

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    uint8_t new_capacity = 120;
    response[4] = new_capacity;
    return send_response(s, 185, response, 8);
}

/*==============================================================================
 * Pet System (msg_no 190-198)
 *============================================================================*/

/* msg_no=190: PET_GET_LIST */
static int handle_pet_get_list(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PET] PET_GET_LIST from fd=%d\n", s->fd);

    uint8_t response[512];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    /* Get pets from database */
    DBPetInfo pets[MAX_PETS];
    int pet_count = db_get_pet_list(s->character_id, pets, MAX_PETS);

    response[offset++] = (uint8_t)pet_count;

    for (int i = 0; i < pet_count && offset < 500; i++) {
        /* pet_id (ui_id) */
        memcpy(response + offset, &pets[i].ui_id, 4);
        offset += 4;

        /* pet_type (it_id) */
        memcpy(response + offset, &pets[i].it_id, 4);
        offset += 4;

        /* name - 20 bytes */
        char name[20];
        memset(name, 0, 20);
        strncpy(name, pets[i].name, 12);
        memcpy(response + offset, name, 20);
        offset += 20;

        /* level (based on exp, simplified) */
        uint8_t level = (uint8_t)(pets[i].exp / 1000 + 1);
        if (level > 100) level = 100;
        response[offset++] = level;

        /* summoned (slot 0 = summoned) */
        uint8_t summoned = (pets[i].slot == 0) ? 1 : 0;
        response[offset++] = summoned;
    }

    printf("[PET] Returning %d pets for charac_no=%u\n", pet_count, s->character_id);
    return send_response(s, 190, response, offset);
}

/* msg_no=191: PET_SUMMON */
static int handle_pet_summon(ClientSession* s, const uint8_t* data, int len) {
    printf("[PET] PET_SUMMON from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 191, error, 4);
    }

    uint32_t pet_id;
    memcpy(&pet_id, data, 4);
    printf("[PET] Summoned pet %u\n", pet_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &pet_id, 4);
    return send_response(s, 191, response, 8);
}

/* msg_no=192: PET_UNSUMMON */
static int handle_pet_unsummon(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[PET] PET_UNSUMMON from fd=%d\n", s->fd);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 192, response, 4);
}

/* msg_no=193: PET_FEED */
static int handle_pet_feed(ClientSession* s, const uint8_t* data, int len) {
    printf("[PET] PET_FEED from fd=%d\n", s->fd);
    if (len < 8) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 193, error, 4);
    }

    uint32_t pet_id;
    uint32_t food_id;
    memcpy(&pet_id, data, 4);
    memcpy(&food_id, data + 4, 4);

    printf("[PET] Fed pet %u with food %u\n", pet_id, food_id);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &pet_id, 4);
    uint8_t new_hunger = 100;
    response[8] = new_hunger;
    return send_response(s, 193, response, 12);
}

/* msg_no=194: PET_RENAME */
static int handle_pet_rename(ClientSession* s, const uint8_t* data, int len) {
    printf("[PET] PET_RENAME from fd=%d\n", s->fd);
    if (len < 24) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 194, error, 4);
    }

    uint32_t pet_id;
    char new_name[20];
    memcpy(&pet_id, data, 4);
    memcpy(new_name, data + 4, 20);
    new_name[19] = '\0';

    printf("[PET] Renamed pet %u to '%s'\n", pet_id, new_name);

    uint8_t response[28];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &pet_id, 4);
    memcpy(response + 8, new_name, 20);
    return send_response(s, 194, response, 28);
}

/* msg_no=195: PET_GET_INFO */
static int handle_pet_get_info(ClientSession* s, const uint8_t* data, int len) {
    printf("[PET] PET_GET_INFO from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 195, error, 4);
    }

    uint32_t pet_id;
    memcpy(&pet_id, data, 4);

    uint8_t response[64];
    memset(response, 0, sizeof(response));
    int offset = 0;

    uint32_t result = 0;
    memcpy(response + offset, &result, 4);
    offset += 4;

    memcpy(response + offset, &pet_id, 4);
    offset += 4;

    uint32_t pet_type = 1000;
    memcpy(response + offset, &pet_type, 4);
    offset += 4;

    char name[20] = "Fluffy";
    memcpy(response + offset, name, 20);
    offset += 20;

    uint8_t level = 15;
    response[offset++] = level;

    uint32_t exp = 5000;
    memcpy(response + offset, &exp, 4);
    offset += 4;

    uint8_t hunger = 85;
    response[offset++] = hunger;

    uint8_t loyalty = 95;
    response[offset++] = loyalty;

    return send_response(s, 195, response, offset);
}

/* msg_no=196: PET_RELEASE */
static int handle_pet_release(ClientSession* s, const uint8_t* data, int len) {
    printf("[PET] PET_RELEASE from fd=%d\n", s->fd);
    if (len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 196, error, 4);
    }

    uint32_t pet_id;
    memcpy(&pet_id, data, 4);
    printf("[PET] Released pet %u\n", pet_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &pet_id, 4);
    return send_response(s, 196, response, 8);
}

/* msg_no=197: PET_EQUIP_ITEM */
static int handle_pet_equip_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[PET] PET_EQUIP_ITEM from fd=%d\n", s->fd);
    if (len < 8) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 197, error, 4);
    }

    uint32_t pet_id;
    uint32_t item_uid;
    memcpy(&pet_id, data, 4);
    memcpy(&item_uid, data + 4, 4);

    printf("[PET] Equipped item %u on pet %u\n", item_uid, pet_id);

    uint8_t response[12];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &pet_id, 4);
    memcpy(response + 8, &item_uid, 4);
    return send_response(s, 197, response, 12);
}

/* msg_no=198: PET_UNEQUIP_ITEM */
static int handle_pet_unequip_item(ClientSession* s, const uint8_t* data, int len) {
    printf("[PET] PET_UNEQUIP_ITEM from fd=%d\n", s->fd);
    if (len < 5) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 198, error, 4);
    }

    uint32_t pet_id;
    uint8_t slot;
    memcpy(&pet_id, data, 4);
    slot = data[4];

    printf("[PET] Unequipped slot %d from pet %u\n", slot, pet_id);

    uint8_t response[8];
    memset(response, 0, sizeof(response));
    memcpy(response + 4, &pet_id, 4);
    response[8] = slot;
    return send_response(s, 198, response, 9);
}

/* Default handler */
static int handle_default(ClientSession* s, uint8_t msg_no, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] Unknown msg_no=%d from fd=%d\n", msg_no, s->fd);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, msg_no, response, 4);
}

/*==============================================================================
 * Packet Dispatcher
 *============================================================================*/

static int handle_packet(ClientSession* s, const uint8_t* packet, int pkt_len) {
    if (pkt_len < PACKET_HEADER_SIZE) {
        return -1;
    }

    /* Parse header */
    uint8_t classification = packet[0];
    uint32_t length;
    memcpy(&length, packet + 1, 4);
    uint8_t msg_no = packet[5];
    uint16_t sequence;
    memcpy(&sequence, packet + 11, 2);

    const uint8_t* data = packet + PACKET_HEADER_SIZE;
    int data_len = pkt_len - PACKET_HEADER_SIZE;

    printf("[GAME] Packet: class=%d len=%u msg=%d seq=%u data_len=%d\n",
           classification, length, msg_no, sequence, data_len);

    s->last_activity = (uint32_t)time(NULL);

    /* Dispatch - Protocol numbers match production binary (df_game_r)
     * Extracted via: strings df_game_r | grep ENUM_CMDPACKET_ | nl -v 0
     */
    switch (msg_no) {
        /* ========== Core Connection (0-8) ========== */
        case 0:  return handle_check_connection(s, data, data_len);  /* CHECK_CONNECTION */
        case 1:  return handle_login(s, data, data_len);             /* LOGIN */
        case 2:  return handle_set_udp(s, data, data_len);           /* SET_UDP_IP_PORT */
        case 3:  return handle_exit(s, data, data_len);              /* EXIT */
        case 4:  return handle_select_character(s, data, data_len);  /* SELECT_CHARACTER */
        case 5:  return handle_create_character(s, data, data_len);  /* CREATE_CHARACTER */
        case 6:  return handle_delete_character(s, data, data_len);  /* DELETE_CHARACTER */
        case 7:  return handle_return_select(s, data, data_len);     /* RETURN_SELECT_CHARACTER */
        case 8:  return handle_get_userinfo(s, data, data_len);      /* GET_USERINFO */
        /* 9: RECOVER_STAMINA - not implemented */

        /* ========== P2P & Party (10-14) ========== */
        /* 10: REQUEST_PEER - P2P related */
        /* 11: RESPONSE_PEER - P2P related */
        case 12: return handle_party_create(s, data, data_len);      /* SET_PARTY_INFO - reuse as create */
        case 13: return handle_party_leave(s, data, data_len);       /* LEAVE_PARTY */
        case 14: return handle_party_kick(s, data, data_len);        /* WALKOUT_PARTY_MEMBER */

        /* ========== Game Start & Dungeon (15-16) ========== */
        case 15: return handle_start_game(s, data, data_len);        /* START_GAME */
        case 16: return handle_dungeon_enter(s, data, data_len);     /* SELECT_DUNGEON - enter dungeon */

        /* ========== Chat (17) ========== */
        case 17: return handle_chat(s, data, data_len);              /* SEND_MESSAGE */

        /* ========== Item Operations (18-29) ========== */
        case 18: return handle_delete_item(s, data, data_len);       /* DELETE_ITEM */
        case 19: return handle_move_item(s, data, data_len);         /* MOVE_ITEMSPACE */
        case 20: return handle_sort_item(s, data, data_len);         /* SORT_ITEM */
        case 21: return handle_npc_shop_buy(s, data, data_len);      /* BUY_ITEM (NPC shop) */
        /* 22: RENT_AVATAR - not implemented */
        /* 23: EXTEND_AVATAR - not implemented */
        case 24: return handle_npc_shop_sell(s, data, data_len);     /* SELL_ITEM */
        case 25: return handle_repair_equipment(s, data, data_len);  /* REPAIR_EQUIPMENT */
        case 26: return handle_trade_request(s, data, data_len);     /* SET_ITEMTRADE_STATE */
        case 27: return handle_compound_item(s, data, data_len);     /* COMPOUND_ITEM */
        case 28: return handle_disjoint_item(s, data, data_len);     /* DISJOINT_ITEM */
        /* 29: USE_LOTTERY_ITEM - not implemented */

        /* ========== Skills & Stats (30-36) ========== */
        case 30: return handle_change_skillslot(s, data, data_len);  /* CHANGE_SKILLSLOT */
        case 31: return handle_buy_skill(s, data, data_len);         /* BUY_SKILL */
        case 32: return handle_increase_status(s, data, data_len);   /* INCREASE_STATUS */
        case 33: return handle_accept_quest(s, data, data_len);      /* ACCEPT_QUEST */
        case 34: return handle_giveup_quest(s, data, data_len);      /* GIVEUP_QUEST */
        case 35: return handle_set_quest_trigger(s, data, data_len); /* SET_QUEST_TRIGGER */
        case 36: return handle_complete_quest(s, data, data_len);    /* FINISH_QUEST */

        /* ========== Movement & Area (37-40) ========== */
        case 37: return handle_move(s, data, data_len);              /* SET_USER_POSITION */
        case 38: return handle_move_area(s, data, data_len);         /* SET_USER_AREA */
        /* 39: FISHING - not implemented */
        case 40: return handle_finish_loading(s, data, data_len);    /* FINISH_LOADING */

        /* ========== Combat & Items (41-51) ========== */
        case 41: return handle_use_skill(s, data, data_len);         /* USE_SKILL */
        case 42: return handle_die_monster(s, data, data_len);       /* DIE_MONSTER */
        case 43: return handle_die_character(s, data, data_len);     /* DIE_CHARACTER */
        case 44: return handle_use_coin(s, data, data_len);          /* USE_COIN (resurrection) */
        case 45: return handle_giveup_game(s, data, data_len);       /* GIVEUP_GAME */
        case 46: return handle_get_item(s, data, data_len);          /* GET_ITEM (pickup) */
        case 47: return handle_use_item(s, data, data_len);          /* USE_STACKABLE */
        case 48: return handle_dungeon_next_room(s, data, data_len); /* MOVE_MAP (next room) */
        case 49: return handle_dungeon_clear(s, data, data_len);     /* SET_PLAY_RESULT */
        case 50: return handle_drop_item(s, data, data_len);         /* DROP_ITEM */
        /* 51: DECREASE_DURABILITY - handled server-side */

        /* ========== PVP (53-62) ========== */
        case 53: return handle_pvp_create_room(s, data, data_len);   /* MAKE_PVP_ROOM */
        case 54: return handle_pvp_enter_room(s, data, data_len);    /* ENTER_PVP_ROOM */
        case 55: return handle_pvp_set_seat(s, data, data_len);      /* SET_PVP_SEAT_STATE */
        case 56: return handle_pvp_ready(s, data, data_len);         /* SET_PVP_READY_STATE */
        /* 57-62: Additional PVP - not yet implemented */

        /* ========== Social - Friends (63-64) ========== */
        case 63: return handle_friend_add(s, data, data_len);        /* ADD_FRIEND */
        case 64: return handle_friend_remove(s, data, data_len);     /* REMOVE_FRIEND */

        /* ========== Debug & CERA (65-69) ========== */
        case 65: return handle_debug_command(s, data, data_len);     /* DEBUG_COMMAND */
        /* 66-69: CERA shop - not implemented */

        /* ========== Guild (70-82) ========== */
        case 70: return handle_guild_get_members(s, data, data_len); /* GUILD_MEMER_LIST */
        /* 71-76: Guild management - partial */
        case 77: return handle_guild_get_info(s, data, data_len);    /* GUILD_INFO */
        case 78: return handle_guild_join(s, data, data_len);        /* REQUEST_GUILD_ENTER */
        /* 79-82: Guild member management */
        case 81: return handle_guild_leave(s, data, data_len);       /* MEMBER_SECEDE */

        /* ========== Item Enhancement (83-91) ========== */
        case 83: return handle_upgrade_item(s, data, data_len);      /* UPGRADE_ITEM */
        /* 84-91: Private store, etc */

        /* ========== Mail (97-99) ========== */
        case 97: return handle_mail_send(s, data, data_len);         /* MAILBOX_SEND */
        case 98: return handle_mail_take_attachment(s, data, data_len); /* MAILBOX_EXTRACT_ITEM */
        case 99: return handle_mail_get_list(s, data, data_len);     /* MAILBOX_OPEN */

        /* ========== Storage/Cargo (100-101) ========== */
        case 100: return handle_storage_deposit(s, data, data_len);  /* DEPOSIT_MONEY */
        case 101: return handle_storage_withdraw(s, data, data_len); /* WITHDRAW_MONEY */

        /* ========== Extended protocols - keep old numbers for Python test compatibility ========== */
        /* Inventory (get) - custom extension */
        case 200: return handle_get_inventory(s, data, data_len);    /* Custom: GET_INVENTORY */
        case 201: return handle_get_skill_list(s, data, data_len);   /* Custom: GET_SKILL_LIST */
        case 202: return handle_get_equipment(s, data, data_len);    /* Custom: GET_EQUIPMENT */
        case 203: return handle_equip_item(s, data, data_len);       /* Custom: EQUIP_ITEM */
        case 204: return handle_unequip_item(s, data, data_len);     /* Custom: UNEQUIP_ITEM */
        case 205: return handle_get_character_stats(s, data, data_len); /* Custom: GET_STATS */

        /* Dungeon extensions */
        case 210: return handle_dungeon_info(s, data, data_len);     /* Custom: DUNGEON_INFO */
        case 211: return handle_dungeon_exit(s, data, data_len);     /* Custom: DUNGEON_EXIT */
        case 212: return handle_dungeon_attack(s, data, data_len);   /* Custom: DUNGEON_ATTACK */
        case 213: return handle_dungeon_reward(s, data, data_len);   /* Custom: DUNGEON_REWARD */
        case 214: return handle_dungeon_monster_tick(s, data, data_len); /* Custom: MONSTER_TICK */
        case 215: return handle_dungeon_player_hit(s, data, data_len);   /* Custom: PLAYER_HIT */

        /* Party extensions */
        case 220: return handle_party_invite(s, data, data_len);     /* Custom: PARTY_INVITE */
        case 221: return handle_party_join(s, data, data_len);       /* Custom: PARTY_JOIN */
        case 222: return handle_party_get_info(s, data, data_len);   /* Custom: PARTY_GET_INFO */
        case 223: return handle_party_change_leader(s, data, data_len); /* Custom: PARTY_CHANGE_LEADER */
        case 224: return handle_party_set_loot_mode(s, data, data_len); /* Custom: PARTY_SET_LOOT */
        case 225: return handle_party_chat(s, data, data_len);       /* Custom: PARTY_CHAT */

        /* Friend extensions */
        case 230: return handle_friend_get_list(s, data, data_len);  /* Custom: FRIEND_GET_LIST */
        case 231: return handle_friend_accept(s, data, data_len);    /* Custom: FRIEND_ACCEPT */
        case 232: return handle_friend_decline(s, data, data_len);   /* Custom: FRIEND_DECLINE */
        case 233: return handle_friend_block(s, data, data_len);     /* Custom: FRIEND_BLOCK */
        case 234: return handle_friend_unblock(s, data, data_len);   /* Custom: FRIEND_UNBLOCK */
        case 235: return handle_friend_get_blocked_list(s, data, data_len); /* Custom: FRIEND_BLOCKED_LIST */

        /* Guild extensions */
        case 240: return handle_guild_create(s, data, data_len);     /* Custom: GUILD_CREATE */
        case 241: return handle_guild_disband(s, data, data_len);    /* Custom: GUILD_DISBAND */
        case 242: return handle_guild_invite(s, data, data_len);     /* Custom: GUILD_INVITE */
        case 243: return handle_guild_kick(s, data, data_len);       /* Custom: GUILD_KICK */
        case 244: return handle_guild_promote(s, data, data_len);    /* Custom: GUILD_PROMOTE */
        case 245: return handle_guild_demote(s, data, data_len);     /* Custom: GUILD_DEMOTE */
        case 246: return handle_guild_set_notice(s, data, data_len); /* Custom: GUILD_SET_NOTICE */
        case 247: return handle_guild_get_applications(s, data, data_len); /* Custom: GUILD_GET_APPS */
        case 248: return handle_guild_respond_application(s, data, data_len); /* Custom: GUILD_RESPOND_APP */

        /* Mail extensions */
        case 250: return handle_mail_read(s, data, data_len);        /* Custom: MAIL_READ */
        case 251: return handle_mail_delete(s, data, data_len);      /* Custom: MAIL_DELETE */
        case 252: return handle_mail_get_unread_count(s, data, data_len); /* Custom: MAIL_UNREAD */
        case 253: return handle_mail_return(s, data, data_len);      /* Custom: MAIL_RETURN */

        /* Trade extensions */
        case 260: return handle_trade_accept(s, data, data_len);     /* Custom: TRADE_ACCEPT */
        case 261: return handle_trade_decline(s, data, data_len);    /* Custom: TRADE_DECLINE */
        case 262: return handle_trade_add_item(s, data, data_len);   /* Custom: TRADE_ADD_ITEM */
        case 263: return handle_trade_remove_item(s, data, data_len);/* Custom: TRADE_REMOVE_ITEM */
        case 264: return handle_trade_set_gold(s, data, data_len);   /* Custom: TRADE_SET_GOLD */
        case 265: return handle_trade_lock(s, data, data_len);       /* Custom: TRADE_LOCK */
        case 266: return handle_trade_confirm(s, data, data_len);    /* Custom: TRADE_CONFIRM */
        case 267: return handle_trade_cancel(s, data, data_len);     /* Custom: TRADE_CANCEL */

        /* PVP extensions */
        case 270: return handle_pvp_get_info(s, data, data_len);     /* Custom: PVP_GET_INFO */
        case 271: return handle_pvp_queue(s, data, data_len);        /* Custom: PVP_QUEUE */
        case 272: return handle_pvp_cancel_queue(s, data, data_len); /* Custom: PVP_CANCEL_QUEUE */
        case 273: return handle_pvp_accept_match(s, data, data_len); /* Custom: PVP_ACCEPT_MATCH */
        case 274: return handle_pvp_decline_match(s, data, data_len);/* Custom: PVP_DECLINE_MATCH */
        case 275: return handle_pvp_attack(s, data, data_len);       /* Custom: PVP_ATTACK */
        case 276: return handle_pvp_leave(s, data, data_len);        /* Custom: PVP_LEAVE */
        case 277: return handle_pvp_get_ranking(s, data, data_len);  /* Custom: PVP_GET_RANKING */

        /* Ranking extensions */
        case 280: return handle_ranking_get_level(s, data, data_len);
        case 281: return handle_ranking_get_wealth(s, data, data_len);
        case 282: return handle_ranking_get_dungeon(s, data, data_len);
        case 283: return handle_ranking_get_my_rank(s, data, data_len);
        case 284: return handle_ranking_get_guild(s, data, data_len);
        case 285: return handle_ranking_get_achievement(s, data, data_len);

        /* Achievement extensions */
        case 290: return handle_achievement_get_list(s, data, data_len);
        case 291: return handle_achievement_get_detail(s, data, data_len);
        case 292: return handle_achievement_claim_reward(s, data, data_len);
        case 293: return handle_achievement_get_progress(s, data, data_len);
        case 294: return handle_achievement_get_categories(s, data, data_len);
        case 295: return handle_achievement_get_by_category(s, data, data_len);
        case 296: return handle_achievement_get_recent(s, data, data_len);

        /* Title protocols */
        case 160: return handle_title_get_list(s, data, data_len);
        case 161: return handle_title_equip(s, data, data_len);
        case 162: return handle_title_unequip(s, data, data_len);
        case 163: return handle_title_get_detail(s, data, data_len);
        case 164: return handle_title_get_current(s, data, data_len);
        case 165: return handle_title_get_unlockable(s, data, data_len);

        /* Auction protocols */
        case 170: return handle_auction_search(s, data, data_len);
        case 171: return handle_auction_list_item(s, data, data_len);
        case 172: return handle_auction_cancel(s, data, data_len);
        case 173: return handle_auction_buy(s, data, data_len);
        case 174: return handle_auction_get_my_listings(s, data, data_len);
        case 175: return handle_auction_get_history(s, data, data_len);
        case 176: return handle_auction_bid(s, data, data_len);
        case 177: return handle_auction_get_price_history(s, data, data_len);
        case 178: return handle_auction_collect_gold(s, data, data_len);

        /* Storage protocols */
        case 180: return handle_storage_get_list(s, data, data_len);
        case 181: return handle_storage_deposit_item(s, data, data_len);
        case 182: return handle_storage_withdraw_item(s, data, data_len);
        case 183: return handle_storage_deposit_gold(s, data, data_len);
        case 184: return handle_storage_withdraw_gold(s, data, data_len);
        case 185: return handle_storage_expand(s, data, data_len);

        /* Pet protocols */
        case 190: return handle_pet_get_list(s, data, data_len);
        case 191: return handle_pet_summon(s, data, data_len);
        case 192: return handle_pet_unsummon(s, data, data_len);
        case 193: return handle_pet_feed(s, data, data_len);
        case 194: return handle_pet_rename(s, data, data_len);
        case 195: return handle_pet_get_info(s, data, data_len);
        case 196: return handle_pet_release(s, data, data_len);
        case 197: return handle_pet_equip_item(s, data, data_len);
        case 198: return handle_pet_unequip_item(s, data, data_len);

        default: return handle_default(s, msg_no, data, data_len);
    }
}

/*==============================================================================
 * Client Handler
 *============================================================================*/

static int handle_client(ClientSession* s) {
    int n = recv(s->fd, s->recv_buffer + s->recv_len, BUFFER_SIZE - s->recv_len, 0);
    printf("[DEBUG] recv fd=%d n=%d errno=%d\n", s->fd, n, errno);
    if (n <= 0) {
        if (n == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
            printf("[GAME] Client disconnected: fd=%d\n", s->fd);
            return -1;
        }
        return 0;
    }

    s->recv_len += n;

    /* Process complete packets */
    int max_iterations = 100;  /* Prevent infinite loop */
    while (s->recv_len >= PACKET_HEADER_SIZE && max_iterations-- > 0) {
        uint8_t pkt_class = s->recv_buffer[0];

        /* Validate class byte - must be 0 or 1 */
        if (pkt_class > 1) {
            /* Invalid packet start, scan for valid header */
            int skip = 1;
            while (skip < s->recv_len && (unsigned char)s->recv_buffer[skip] > 1) {
                skip++;
            }
            if (skip > 0) {
                memmove(s->recv_buffer, s->recv_buffer + skip, s->recv_len - skip);
                s->recv_len -= skip;
            }
            continue;
        }

        uint32_t pkt_len;
        memcpy(&pkt_len, s->recv_buffer + 1, 4);

        /* Validate packet length - reasonable bounds */
        if (pkt_len > 8192 || pkt_len < PACKET_HEADER_SIZE) {
            /* Skip one byte and try to resync */
            memmove(s->recv_buffer, s->recv_buffer + 1, s->recv_len - 1);
            s->recv_len--;
            continue;
        }

        if ((int)pkt_len > s->recv_len) {
            break;  /* Wait for more data */
        }

        int result = handle_packet(s, s->recv_buffer, pkt_len);

        int remaining = s->recv_len - pkt_len;
        if (remaining > 0) {
            memmove(s->recv_buffer, s->recv_buffer + pkt_len, remaining);
        }
        s->recv_len = remaining;

        if (result < 0) {
            return -1;
        }
    }

    return 0;
}

/*==============================================================================
 * Main
 *============================================================================*/

int main(int argc, char* argv[]) {
    setlinebuf(stdout);
    setlinebuf(stderr);

    int port = PORT;
    if (argc > 1) {
        port = atoi(argv[1]);
    }

    printf("=== DNF Game Server (Database Integrated) ===\n");
    printf("Port: %d\n", port);
    printf("Database: %s@%s\n", DB_USER, DB_HOST);
    printf("============================================\n");

    /* Initialize database */
    if (db_init() < 0) {
        printf("[GAME] Database initialization failed!\n");
        printf("[GAME] Continuing without database (mock mode)\n");
    }

    /* Initialize sessions */
    memset(g_sessions, 0, sizeof(g_sessions));

    /* Create listen socket */
    g_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_listen_fd < 0) {
        perror("socket");
        db_cleanup();
        return 1;
    }

    int opt = 1;
    setsockopt(g_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    set_nonblocking(g_listen_fd);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(g_listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(g_listen_fd);
        db_cleanup();
        return 1;
    }

    if (listen(g_listen_fd, 128) < 0) {
        perror("listen");
        close(g_listen_fd);
        db_cleanup();
        return 1;
    }

    printf("[GAME] Listening on 0.0.0.0:%d\n", port);

    /* Create epoll */
    g_epoll_fd = epoll_create(MAX_EVENTS);
    if (g_epoll_fd < 0) {
        perror("epoll_create");
        close(g_listen_fd);
        db_cleanup();
        return 1;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = g_listen_fd;
    epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, g_listen_fd, &ev);

    struct epoll_event events[MAX_EVENTS];

    /* Main loop */
    while (g_running) {
        int nfds = epoll_wait(g_epoll_fd, events, MAX_EVENTS, 1000);

        if (nfds < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;

            if (fd == g_listen_fd) {
                /* New connection */
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(g_listen_fd, (struct sockaddr*)&client_addr, &client_len);

                if (client_fd >= 0) {
                    set_nonblocking(client_fd);

                    ClientSession* s = create_session(client_fd,
                                                      client_addr.sin_addr.s_addr,
                                                      ntohs(client_addr.sin_port));
                    if (s) {
                        ev.events = EPOLLIN;
                        ev.data.fd = client_fd;
                        epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);

                        printf("[GAME] New connection: fd=%d from %s:%d (sessions=%d)\n",
                               client_fd,
                               inet_ntoa(client_addr.sin_addr),
                               ntohs(client_addr.sin_port),
                               g_session_count);
                    } else {
                        printf("[GAME] Max sessions reached\n");
                        close(client_fd);
                    }
                }
            } else {
                /* Client data */
                ClientSession* s = find_session(fd);
                if (!s) {
                    epoll_ctl(g_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    continue;
                }

                if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                    printf("[GAME] Client error/hup: fd=%d\n", fd);
                    epoll_ctl(g_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    destroy_session(s);
                } else if (events[i].events & EPOLLIN) {
                    if (handle_client(s) < 0) {
                        epoll_ctl(g_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                        close(fd);
                        destroy_session(s);
                    }
                }
            }
        }
    }

    close(g_epoll_fd);
    close(g_listen_fd);
    db_cleanup();
    printf("[GAME] Server stopped\n");

    return 0;
}
