/**
 * @file event_types.h
 * @brief Event type definitions
 *
 * Defines all game event types for the event bus system.
 */

#ifndef ENGINE_CORE_EVENT_TYPES_H
#define ENGINE_CORE_EVENT_TYPES_H

#include "../types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Event categories
 */
typedef enum {
    EVENT_CATEGORY_NONE     = 0x0000,
    EVENT_CATEGORY_SESSION  = 0x0100,
    EVENT_CATEGORY_PLAYER   = 0x0200,
    EVENT_CATEGORY_COMBAT   = 0x0300,
    EVENT_CATEGORY_DUNGEON  = 0x0400,
    EVENT_CATEGORY_SOCIAL   = 0x0500,
    EVENT_CATEGORY_ECONOMY  = 0x0600,
    EVENT_CATEGORY_SYSTEM   = 0x0700,
    EVENT_CATEGORY_SCRIPT   = 0x0800,
    EVENT_CATEGORY_CUSTOM   = 0x1000
} EventCategory;

/**
 * Event types
 */
typedef enum {
    /* None */
    EVENT_NONE = 0,

    /* Session events (0x01xx) */
    EVENT_SESSION_CREATED       = 0x0101,
    EVENT_SESSION_DESTROYED     = 0x0102,
    EVENT_SESSION_AUTHENTICATED = 0x0103,
    EVENT_SESSION_TIMEOUT       = 0x0104,
    EVENT_SESSION_KICKED        = 0x0105,

    /* Player events (0x02xx) */
    EVENT_PLAYER_LOGIN          = 0x0201,
    EVENT_PLAYER_LOGOUT         = 0x0202,
    EVENT_PLAYER_CHAR_SELECT    = 0x0203,
    EVENT_PLAYER_ENTER_GAME     = 0x0204,
    EVENT_PLAYER_LEAVE_GAME     = 0x0205,
    EVENT_PLAYER_LEVEL_UP       = 0x0206,
    EVENT_PLAYER_DIED           = 0x0207,
    EVENT_PLAYER_RESURRECTED    = 0x0208,
    EVENT_PLAYER_MOVE           = 0x0209,
    EVENT_PLAYER_TELEPORT       = 0x020A,

    /* Combat events (0x03xx) */
    EVENT_COMBAT_START          = 0x0301,
    EVENT_COMBAT_END            = 0x0302,
    EVENT_DAMAGE_DEALT          = 0x0303,
    EVENT_DAMAGE_TAKEN          = 0x0304,
    EVENT_HEAL_RECEIVED         = 0x0305,
    EVENT_SKILL_USED            = 0x0306,
    EVENT_SKILL_HIT             = 0x0307,
    EVENT_SKILL_MISS            = 0x0308,
    EVENT_BUFF_APPLIED          = 0x0309,
    EVENT_BUFF_REMOVED          = 0x030A,
    EVENT_BUFF_EXPIRED          = 0x030B,
    EVENT_MONSTER_KILLED        = 0x030C,
    EVENT_BOSS_KILLED           = 0x030D,

    /* Dungeon events (0x04xx) */
    EVENT_DUNGEON_ENTER         = 0x0401,
    EVENT_DUNGEON_EXIT          = 0x0402,
    EVENT_DUNGEON_ROOM_ENTER    = 0x0403,
    EVENT_DUNGEON_ROOM_CLEARED  = 0x0404,
    EVENT_DUNGEON_BOSS_SPAWN    = 0x0405,
    EVENT_DUNGEON_BOSS_KILLED   = 0x0406,
    EVENT_DUNGEON_COMPLETED     = 0x0407,
    EVENT_DUNGEON_FAILED        = 0x0408,

    /* Social events (0x05xx) */
    EVENT_GUILD_CREATED         = 0x0501,
    EVENT_GUILD_DISBANDED       = 0x0502,
    EVENT_GUILD_JOINED          = 0x0503,
    EVENT_GUILD_LEFT            = 0x0504,
    EVENT_GUILD_KICKED          = 0x0505,
    EVENT_GUILD_PROMOTED        = 0x0506,
    EVENT_GUILD_DEMOTED         = 0x0507,
    EVENT_PARTY_CREATED         = 0x0510,
    EVENT_PARTY_DISBANDED       = 0x0511,
    EVENT_PARTY_JOINED          = 0x0512,
    EVENT_PARTY_LEFT            = 0x0513,
    EVENT_PARTY_KICKED          = 0x0514,
    EVENT_FRIEND_ADDED          = 0x0520,
    EVENT_FRIEND_REMOVED        = 0x0521,
    EVENT_CHAT_MESSAGE          = 0x0530,
    EVENT_MAIL_RECEIVED         = 0x0540,

    /* Economy events (0x06xx) */
    EVENT_GOLD_GAINED           = 0x0601,
    EVENT_GOLD_SPENT            = 0x0602,
    EVENT_ITEM_ACQUIRED         = 0x0603,
    EVENT_ITEM_LOST             = 0x0604,
    EVENT_ITEM_EQUIPPED         = 0x0605,
    EVENT_ITEM_UNEQUIPPED       = 0x0606,
    EVENT_ITEM_UPGRADED         = 0x0607,
    EVENT_ITEM_DESTROYED        = 0x0608,
    EVENT_TRADE_STARTED         = 0x0610,
    EVENT_TRADE_COMPLETED       = 0x0611,
    EVENT_TRADE_CANCELLED       = 0x0612,
    EVENT_AUCTION_LISTED        = 0x0620,
    EVENT_AUCTION_SOLD          = 0x0621,
    EVENT_AUCTION_CANCELLED     = 0x0622,

    /* System events (0x07xx) */
    EVENT_SERVER_STARTING       = 0x0701,
    EVENT_SERVER_STARTED        = 0x0702,
    EVENT_SERVER_STOPPING       = 0x0703,
    EVENT_SERVER_STOPPED        = 0x0704,
    EVENT_TICK                  = 0x0710,
    EVENT_TIMER_FIRED           = 0x0711,
    EVENT_CONFIG_RELOAD         = 0x0720,
    EVENT_DB_CONNECTED          = 0x0730,
    EVENT_DB_DISCONNECTED       = 0x0731,
    EVENT_IPC_CONNECTED         = 0x0740,
    EVENT_IPC_DISCONNECTED      = 0x0741,

    /* Script events (0x08xx) */
    EVENT_SCRIPT_LOADED         = 0x0801,
    EVENT_SCRIPT_UNLOADED       = 0x0802,
    EVENT_SCRIPT_ERROR          = 0x0803,
    EVENT_SCRIPT_RELOADED       = 0x0804,

    /* Custom events start at 0x1000 */
    EVENT_CUSTOM_BASE           = 0x1000,
    EVENT_MAX                   = 0xFFFF
} EventType;

/**
 * Event data structures
 */

/* Base event data (all events have this) */
typedef struct {
    EventType type;
    u32       timestamp;
    u32       source_id;    /* Source entity ID (player, monster, etc.) */
    u32       target_id;    /* Target entity ID */
} EventBase;

/* Damage event data */
typedef struct {
    EventBase base;
    u32       damage;
    u16       skill_id;
    u8        damage_type;  /* Physical, magical, etc. */
    u8        is_critical;
    u8        is_blocked;
    u8        element;
} EventDamage;

/* Buff event data */
typedef struct {
    EventBase base;
    u16       buff_id;
    u16       duration;
    u8        stacks;
    u8        is_debuff;
} EventBuff;

/* Level up event data */
typedef struct {
    EventBase base;
    u8        old_level;
    u8        new_level;
} EventLevelUp;

/* Item event data */
typedef struct {
    EventBase base;
    u32       item_id;
    u32       item_uid;     /* Unique instance ID */
    u16       quantity;
    u8        slot;
} EventItem;

/* Gold event data */
typedef struct {
    EventBase base;
    u32       amount;
    u32       new_total;
    u8        reason;       /* Shop, trade, quest, drop, etc. */
} EventGold;

/* Dungeon event data */
typedef struct {
    EventBase base;
    u32       dungeon_id;
    u8        difficulty;
    u8        room_index;
    u8        rating;       /* S/A/B/C/D */
} EventDungeon;

/* Guild event data */
typedef struct {
    EventBase base;
    u32       guild_id;
    u32       member_id;
    u8        old_rank;
    u8        new_rank;
} EventGuild;

/* Generic custom event data */
typedef struct {
    EventBase base;
    u32       param1;
    u32       param2;
    u32       param3;
    u32       param4;
    void*     custom_data;
    u32       custom_data_size;
} EventCustom;

/**
 * Get event category from event type
 * @param type Event type
 * @return Event category
 */
INLINE EventCategory event_get_category(EventType type)
{
    return (EventCategory)(type & 0xFF00);
}

/**
 * Check if event type is in category
 * @param type Event type
 * @param category Event category
 * @return 1 if in category, 0 otherwise
 */
INLINE int event_is_category(EventType type, EventCategory category)
{
    return (type & 0xFF00) == category;
}

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_CORE_EVENT_TYPES_H */
