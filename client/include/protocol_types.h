/**
 * @file protocol_types.h
 * @brief Protocol data structures for client-server communication
 */

#ifndef PROTOCOL_TYPES_H
#define PROTOCOL_TYPES_H

#include "client.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ============================================================================
 * Basic Types
 * ============================================================================
 */

/* Max limits */
#define MAX_CHARACTERS      16
#define MAX_INVENTORY_SLOTS 128
#define MAX_EQUIPMENT_SLOTS 16
#define MAX_SKILL_SLOTS     48
#define MAX_BUFF_SLOTS      32
#define MAX_QUEST_SLOTS     64
#define MAX_FRIENDS         100
#define MAX_GUILD_MEMBERS   100
#define MAX_PARTY_MEMBERS   4
#define MAX_CHAT_LENGTH     256
#define MAX_NAME_LENGTH     32
#define MAX_ENTITIES        256

/* Item types */
typedef enum {
    ITEM_TYPE_NONE = 0,
    ITEM_TYPE_WEAPON,
    ITEM_TYPE_ARMOR,
    ITEM_TYPE_ACCESSORY,
    ITEM_TYPE_CONSUMABLE,
    ITEM_TYPE_MATERIAL,
    ITEM_TYPE_QUEST,
    ITEM_TYPE_CURRENCY,
    ITEM_TYPE_PET,
    ITEM_TYPE_AVATAR
} ItemType;

/* Equipment slots */
typedef enum {
    EQUIP_SLOT_WEAPON = 0,
    EQUIP_SLOT_TITLE,
    EQUIP_SLOT_HEAD,
    EQUIP_SLOT_CHEST,
    EQUIP_SLOT_SHOULDER,
    EQUIP_SLOT_WAIST,
    EQUIP_SLOT_LEGS,
    EQUIP_SLOT_SHOES,
    EQUIP_SLOT_BRACELET,
    EQUIP_SLOT_NECKLACE,
    EQUIP_SLOT_RING,
    EQUIP_SLOT_SUPPORT,
    EQUIP_SLOT_MAGIC_STONE,
    EQUIP_SLOT_EARRING,
    EQUIP_SLOT_AURA,
    EQUIP_SLOT_COUNT
} EquipSlot;

/* Character classes */
typedef enum {
    CLASS_SLAYER = 0,
    CLASS_FIGHTER,
    CLASS_GUNNER,
    CLASS_MAGE,
    CLASS_PRIEST,
    CLASS_THIEF,
    CLASS_KNIGHT,
    CLASS_DEMONIC_LANCER,
    CLASS_AGENT,
    CLASS_COUNT
} CharacterClass;

/* Quest states */
typedef enum {
    QUEST_STATE_NONE = 0,
    QUEST_STATE_AVAILABLE,
    QUEST_STATE_ACCEPTED,
    QUEST_STATE_COMPLETED,
    QUEST_STATE_REWARDED
} QuestState;

/*
 * ============================================================================
 * Item Structures
 * ============================================================================
 */

/* Item data */
typedef struct {
    u32 item_id;            /* Item template ID */
    u32 unique_id;          /* Unique instance ID */
    u16 slot;               /* Inventory slot */
    u16 count;              /* Stack count */
    u8  type;               /* ItemType */
    u8  rarity;             /* 0=common, 1=magic, 2=rare, 3=epic, 4=legendary */
    u8  bound;              /* Is bound to character */
    u8  enhance_level;      /* +0 to +15 */
    u16 durability;         /* Current durability */
    u16 max_durability;     /* Max durability */
    u32 expire_time;        /* Expiration timestamp (0=permanent) */
    /* Random attributes */
    u8  attr_count;
    struct {
        u8  type;           /* Attribute type */
        i16 value;          /* Attribute value */
    } attrs[8];
} ItemData;

/* Equipment info (extends ItemData) */
typedef struct {
    ItemData base;
    u8  equip_slot;         /* EquipSlot */
    u8  req_level;          /* Required level */
    u8  req_class;          /* Required class (bitmask) */
    i16 phys_atk;
    i16 mag_atk;
    i16 phys_def;
    i16 mag_def;
    i16 str_bonus;
    i16 int_bonus;
    i16 vit_bonus;
    i16 spr_bonus;
} EquipmentData;

/*
 * ============================================================================
 * Character Structures
 * ============================================================================
 */

/* Character info (for character select) */
typedef struct {
    u32 charac_no;          /* Character ID */
    char name[MAX_NAME_LENGTH];
    u8  job;                /* CharacterClass */
    u8  grow_type;          /* Sub-class / advancement */
    u16 level;
    u32 exp;
    u32 gold;
    u16 fatigue;            /* Current fatigue */
    u16 max_fatigue;        /* Max fatigue (156) */
    u32 last_play;          /* Last play timestamp */
    u8  delete_flag;        /* Is marked for deletion */
    u32 delete_time;        /* Deletion timestamp */
    /* Appearance */
    u8  hair_style;
    u8  hair_color;
    u8  face_style;
    u8  skin_color;
    /* Equipment preview (for display in char select) */
    u32 weapon_id;
    u32 avatar_weapon_id;
    u32 avatar_top_id;
    u32 avatar_bottom_id;
} CharacterInfo;

/* Full character data (after entering game) */
typedef struct {
    CharacterInfo info;
    /* Stats */
    i32 hp;
    i32 max_hp;
    i32 mp;
    i32 max_mp;
    i16 str;
    i16 intel;
    i16 vit;
    i16 spr;
    i16 phys_atk;
    i16 mag_atk;
    i16 phys_def;
    i16 mag_def;
    i16 crit_rate;
    i16 crit_damage;
    i16 move_speed;
    i16 attack_speed;
    i16 cast_speed;
    /* Position */
    float x;
    float y;
    u32 map_id;
    u8  channel;
    /* Status */
    u8  pk_mode;
    u32 guild_id;
    u32 party_id;
    /* SP/TP */
    u32 sp;
    u32 tp;
    /* Quest points */
    u16 qp;
} CharacterData;

/*
 * ============================================================================
 * Skill Structures
 * ============================================================================
 */

typedef struct {
    u32 skill_id;
    u8  level;
    u8  slot;               /* Skill bar slot */
    u32 cooldown_end;       /* Timestamp when cooldown ends */
} SkillData;

typedef struct {
    u32 skill_count;
    SkillData skills[MAX_SKILL_SLOTS];
} SkillList;

/*
 * ============================================================================
 * Buff Structures
 * ============================================================================
 */

typedef struct {
    u32 buff_id;
    u8  level;
    u32 source_id;          /* Who applied this buff */
    u32 end_time;           /* When buff expires */
    u8  stack_count;
} BuffData;

typedef struct {
    u8 count;
    BuffData buffs[MAX_BUFF_SLOTS];
} BuffList;

/*
 * ============================================================================
 * Entity Structures (for synchronization)
 * ============================================================================
 */

/* Entity types in the game world */
typedef enum {
    SYNC_ENTITY_PLAYER = 0,
    SYNC_ENTITY_MONSTER,
    SYNC_ENTITY_NPC,
    SYNC_ENTITY_ITEM_DROP,
    SYNC_ENTITY_PROJECTILE,
    SYNC_ENTITY_PET
} SyncEntityType;

/* Spawn data sent by server */
typedef struct {
    u32 entity_id;
    u8  entity_type;        /* SyncEntityType */
    u32 template_id;        /* Monster/NPC ID */
    char name[MAX_NAME_LENGTH];
    float x;
    float y;
    float dir;              /* Direction in radians */
    i32 hp;
    i32 max_hp;
    u8  level;
    u8  state;              /* EntityState */
    /* For players only */
    u8  job;
    u8  grow_type;
    u32 guild_id;
    /* Visual */
    u32 weapon_id;
    u32 avatar_ids[4];      /* Top, bottom, hair, face */
} SpawnData;

/* Position update */
typedef struct {
    u32 entity_id;
    float x;
    float y;
    float vx;               /* Velocity X */
    float vy;               /* Velocity Y */
    float dir;
    u8  state;              /* Walking, running, etc */
    u32 timestamp;          /* Server timestamp */
} PositionSync;

/* Damage event */
typedef struct {
    u32 attacker_id;
    u32 target_id;
    u32 skill_id;
    i32 damage;
    u8  damage_type;        /* 0=normal, 1=crit, 2=miss, 3=block */
    i32 target_hp;          /* Target's HP after damage */
    float knockback_x;
    float knockback_y;
} DamageEvent;

/* Death event */
typedef struct {
    u32 entity_id;
    u32 killer_id;
    float x;
    float y;
} DeathEvent;

/*
 * ============================================================================
 * Dungeon Structures
 * ============================================================================
 */

typedef struct {
    u32 dungeon_id;
    u8  difficulty;         /* 0-4 */
    u8  room_count;
    u8  current_room;
    u8  clear_rank;         /* S/A/B/C/D after clear */
    u32 time_limit;         /* Remaining time in ms */
    u32 monster_count;
    u32 monster_killed;
} DungeonInfo;

typedef struct {
    u32 exp_gained;
    u32 gold_gained;
    u8  item_count;
    ItemData items[16];     /* Dropped items */
    u8  rank;               /* Clear rank */
    u32 clear_time;         /* Time taken in ms */
} DungeonResult;

/*
 * ============================================================================
 * Social Structures
 * ============================================================================
 */

typedef struct {
    u32 charac_no;
    char name[MAX_NAME_LENGTH];
    u8  level;
    u8  job;
    u8  online;
    u32 last_login;
    u8  relation;           /* 0=friend, 1=blocked */
} FriendData;

typedef struct {
    u32 guild_id;
    char name[MAX_NAME_LENGTH];
    u8  level;
    u32 exp;
    u32 gold;
    u32 member_count;
    u32 max_members;
    u32 master_id;
    char master_name[MAX_NAME_LENGTH];
    char notice[256];
} GuildInfo;

typedef struct {
    u32 charac_no;
    char name[MAX_NAME_LENGTH];
    u8  rank;               /* 0=master, 1=officer, 2=member */
    u8  level;
    u8  job;
    u8  online;
    u32 contribution;
    u32 last_login;
} GuildMemberData;

typedef struct {
    u32 party_id;
    u32 leader_id;
    u8  member_count;
    struct {
        u32 charac_no;
        char name[MAX_NAME_LENGTH];
        u8  level;
        u8  job;
        i32 hp;
        i32 max_hp;
    } members[MAX_PARTY_MEMBERS];
} PartyInfo;

/*
 * ============================================================================
 * Chat Structures
 * ============================================================================
 */

typedef enum {
    CHAT_CHANNEL_NORMAL = 0,
    CHAT_CHANNEL_PARTY,
    CHAT_CHANNEL_GUILD,
    CHAT_CHANNEL_WORLD,
    CHAT_CHANNEL_WHISPER,
    CHAT_CHANNEL_SYSTEM,
    CHAT_CHANNEL_MEGAPHONE
} ChatChannel;

typedef struct {
    u8  channel;
    u32 sender_id;
    char sender_name[MAX_NAME_LENGTH];
    char message[MAX_CHAT_LENGTH];
    u32 timestamp;
} ChatMessage;

/*
 * ============================================================================
 * Mail Structures
 * ============================================================================
 */

typedef struct {
    u32 mail_id;
    u32 sender_id;
    char sender_name[MAX_NAME_LENGTH];
    char title[64];
    char content[512];
    u32 gold;
    u8  has_item;
    ItemData item;
    u32 send_time;
    u32 expire_time;
    u8  read;
    u8  claimed;
} MailData;

/*
 * ============================================================================
 * Trade Structures
 * ============================================================================
 */

typedef struct {
    u32 trade_id;
    u32 partner_id;
    char partner_name[MAX_NAME_LENGTH];
    u8  my_confirmed;
    u8  partner_confirmed;
    u32 my_gold;
    u32 partner_gold;
    u8  my_item_count;
    u8  partner_item_count;
    ItemData my_items[12];
    ItemData partner_items[12];
} TradeInfo;

/*
 * ============================================================================
 * Auction Structures
 * ============================================================================
 */

typedef struct {
    u32 auction_id;
    u32 seller_id;
    char seller_name[MAX_NAME_LENGTH];
    ItemData item;
    u32 price;
    u32 buyout_price;
    u32 current_bid;
    u32 bidder_id;
    u32 start_time;
    u32 end_time;
} AuctionItem;

/*
 * ============================================================================
 * Response Packet Structures
 * ============================================================================
 */

/* Login response */
typedef struct {
    u8  result;             /* 0=success, other=error code */
    u32 account_id;
    u8  character_count;
    CharacterInfo characters[MAX_CHARACTERS];
} LoginResponse;

/* Enter world response */
typedef struct {
    u8  result;
    CharacterData character;
    /* Initial inventory */
    u8  inventory_count;
    ItemData inventory[MAX_INVENTORY_SLOTS];
    /* Equipment */
    EquipmentData equipment[MAX_EQUIPMENT_SLOTS];
    /* Skills */
    SkillList skills;
    /* Buffs */
    BuffList buffs;
} EnterWorldResponse;

/* Inventory update */
typedef struct {
    u8  operation;          /* 0=add, 1=remove, 2=update, 3=move */
    u8  slot;
    ItemData item;
} InventoryUpdate;

/* Stat update */
typedef struct {
    u8  stat_type;          /* Which stat changed */
    i32 value;              /* New value */
} StatUpdate;

#ifdef __cplusplus
}
#endif

#endif /* PROTOCOL_TYPES_H */
