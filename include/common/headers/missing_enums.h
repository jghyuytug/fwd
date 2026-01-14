#ifndef MISSING_ENUMS_H
#define MISSING_ENUMS_H

/* Missing enum definitions that cannot be forward-declared */
/* Auto-generated from error analysis - 2025-10-29 */
/* These enums have 11,620 total references */

// Database handle enumeration (3,300 refs)
// REMOVED - Duplicate definition, see database/db_enums.h:9
/* enum ENUM_DB_HANDLE_IDX {
    DB_HANDLE_ACCOUNT = 0,
    DB_HANDLE_CHARACTER = 1,
    DB_HANDLE_GAME = 2,
    DB_HANDLE_LOG = 3,
    DB_HANDLE_EVENT = 4,
    DB_HANDLE_BACKUP = 5,
    DB_HANDLE_MAX
}; */

// Global effect types for ARAD system (2,220 refs)
// REMOVED - Duplicate definition, see network/packet_enums.h:152
/* enum ENUM_GLOBAL_EFFECT_ARAD {
    GLOBAL_EFFECT_NONE = 0,
    GLOBAL_EFFECT_EXP_BONUS = 1,
    GLOBAL_EFFECT_DROP_BONUS = 2,
    GLOBAL_EFFECT_GOLD_BONUS = 3,
    GLOBAL_EFFECT_EVENT_ACTIVE = 4,
    GLOBAL_EFFECT_MAX
}; */

// Server group enumeration (2,028 refs)
// REMOVED - Duplicate definition, see database/db_enums.h:20
/* enum ENUM_SERVER_GROUP {
    SERVER_GROUP_NONE = 0,
    SERVER_GROUP_CAIN = 1,
    SERVER_GROUP_DIREGIE = 2,
    SERVER_GROUP_SIROCO = 3,
    SERVER_GROUP_CASILLAS = 4,
    SERVER_GROUP_MAX
}; */

// Command packet types (1,740 refs)
// REMOVED - Duplicate definition, see network/packet_enums.h:8
/* enum ENUM_CMDPACKET {
    CMD_PACKET_NONE = 0,
    CMD_PACKET_LOGIN = 1,
    CMD_PACKET_LOGOUT = 2,
    CMD_PACKET_MOVE = 3,
    CMD_PACKET_ATTACK = 4,
    CMD_PACKET_SKILL = 5,
    CMD_PACKET_ITEM = 6,
    CMD_PACKET_CHAT = 7,
    CMD_PACKET_TRADE = 8,
    CMD_PACKET_PARTY = 9,
    CMD_PACKET_GUILD = 10,
    CMD_PACKET_MAX
}; */

// Module type enumeration (540 refs)
// REMOVED - Duplicate definition, see inventory/inventory_enums.h:28
/* enum ENUM_MODULE_TYPE {
    MODULE_TYPE_NONE = 0,
    MODULE_TYPE_CHARACTER = 1,
    MODULE_TYPE_INVENTORY = 2,
    MODULE_TYPE_SKILL = 3,
    MODULE_TYPE_QUEST = 4,
    MODULE_TYPE_DUNGEON = 5,
    MODULE_TYPE_PVP = 6,
    MODULE_TYPE_MAX
}; */

// Amplify type enumeration (492 refs)
enum ENUM_AMPLIFY_TYPE {
    AMPLIFY_TYPE_NONE = 0,
    AMPLIFY_TYPE_PHYSICAL = 1,
    AMPLIFY_TYPE_MAGICAL = 2,
    AMPLIFY_TYPE_INDEPENDENT = 3,
    AMPLIFY_TYPE_ALL = 4,
    AMPLIFY_TYPE_MAX
};

#endif // MISSING_ENUMS_H
