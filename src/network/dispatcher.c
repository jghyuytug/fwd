/**
 * @file dispatcher.c
 * @brief DNF Packet Dispatcher Implementation
 *
 * Implements the 41 DisPatcher classes extracted from production server.
 * Each dispatcher handles a specific category of packets.
 *
 * Source: vm131:/home/dxf/game/df_game_r
 * Date: 2025-11-26
 *
 * @version 1.0
 */

#include "game_server.h"
#include "db_service.h"
#include "../../include/network/headers/packet_enums.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*==============================================================================
 * Constants and Types
 *============================================================================*/

#define MAX_HANDLERS        700
#define MAX_ACCOUNT_NAME    32
#define MAX_CHARAC_NAME     16
#define MAX_CHARACTERS      16

/* Character job classes */
typedef enum {
    JOB_SLAYER = 0,
    JOB_FIGHTER = 1,
    JOB_GUNNER = 2,
    JOB_MAGE = 3,
    JOB_PRIEST = 4,
    JOB_THIEF = 5,
    JOB_KNIGHT = 6,
    JOB_DEMONIC_LANCER = 7,
    JOB_MAX
} CharacterJob;

/* Character grow types (subclass) */
typedef enum {
    GROW_DEFAULT = 0,
    GROW_FIRST = 1,
    GROW_SECOND = 2,
    GROW_THIRD = 3,
    GROW_FOURTH = 4
} GrowType;

/* Simple character data for login flow */
typedef struct {
    uint32_t charac_no;
    char     charac_name[MAX_CHARAC_NAME];
    uint8_t  job;
    uint8_t  grow_type;
    uint8_t  level;
    uint32_t exp;
    uint16_t hp;
    uint16_t max_hp;
    uint16_t mp;
    uint16_t max_mp;
    int32_t  village_id;
    uint32_t gold;
    uint32_t fatigue;
    uint32_t create_time;
    uint32_t last_play_time;
    uint8_t  slot;
    uint8_t  deleted;       /* 0=active, 1=deleted */
    uint32_t delete_time;   /* Time when deletion was requested */
} CharacterInfo;

/* Account data for authentication */
typedef struct {
    uint32_t account_uid;
    char     account_name[MAX_ACCOUNT_NAME];
    uint8_t  account_grade;     /* 0=normal, 1=GM, etc */
    uint8_t  character_count;
    CharacterInfo characters[MAX_CHARACTERS];
    uint32_t last_login_time;
    uint32_t premium_expire;
    uint32_t cera_point;
    uint32_t cera_point_gift;
} AccountInfo;

/*==============================================================================
 * Handler Registry
 *============================================================================*/

typedef int (*DispatcherHandler)(ClientSession* session, const uint8_t* data, int len);

typedef struct {
    DispatcherHandler handler;
    const char*       name;
    int               enabled;
} HandlerEntry;

static HandlerEntry g_handlers[MAX_HANDLERS] = {0};
static int g_handlers_initialized = 0;

/* Statistics */
static struct {
    unsigned long packets_handled;
    unsigned long unknown_packets;
    unsigned long handler_errors;
} g_stats = {0};

/*==============================================================================
 * Forward Declarations - Dispatcher Handlers
 *============================================================================*/

/* Connection & Authentication */
static int Dispatcher_CheckConnection(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_Login(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_Exit(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_GetUserInfo(ClientSession* session, const uint8_t* data, int len);

/* Character Management */
static int Dispatcher_SelectCharacter(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_CreateCharacter(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_DeleteCharacter(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_ReturnSelectCharacter(ClientSession* session, const uint8_t* data, int len);

/* Game Start & Dungeon */
static int Dispatcher_StartGame(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_SelectDungeon(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_FinishLoading(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_SetPlayResult(ClientSession* session, const uint8_t* data, int len);

/* Party & Social */
static int Dispatcher_SendMessage(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_SetPartyInfo(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_LeaveParty(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_RequestPeer(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_ResponsePeer(ClientSession* session, const uint8_t* data, int len);

/* PVP */
static int Dispatcher_MakePvpRoom(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_EnterPvpRoom(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_PvpHeartBeat(ClientSession* session, const uint8_t* data, int len);

/* Phase 3: Item Operations */
static int Dispatcher_BuyItem(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_SellItem(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_UseItem(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_MoveItem(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_EquipItem(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_UnequipItem(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_DropItem(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_PickupItem(ClientSession* session, const uint8_t* data, int len);

/* Phase 3: Dungeon Operations */
static int Dispatcher_EnterDungeon(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_LeaveDungeon(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_ClearDungeon(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_DungeonReward(ClientSession* session, const uint8_t* data, int len);

/* Phase 3: Skill Operations */
static int Dispatcher_UseSkill(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_UpgradeSkill(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_ResetSkill(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_GetSkillList(ClientSession* session, const uint8_t* data, int len);

/* Phase 3: Guild Operations */
static int Dispatcher_CreateGuild(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_JoinGuild(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_LeaveGuild(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_GuildInfo(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_GuildMemberList(ClientSession* session, const uint8_t* data, int len);

/* Default/Unknown */
static int Dispatcher_Default(ClientSession* session, const uint8_t* data, int len);

/*==============================================================================
 * Mock Data for Testing
 *============================================================================*/

static AccountInfo g_mock_account = {
    .account_uid = 10001,
    .account_name = "test_user",
    .account_grade = 0,
    .character_count = 2,
    .characters = {
        {
            .charac_no = 100001,
            .charac_name = "TestSlayer",
            .job = JOB_SLAYER,
            .grow_type = GROW_FIRST,
            .level = 85,
            .exp = 1234567,
            .hp = 50000,
            .max_hp = 50000,
            .mp = 30000,
            .max_mp = 30000,
            .village_id = 1,
            .gold = 999999999,
            .fatigue = 156,
            .create_time = 1700000000,
            .last_play_time = 1732550400,
            .slot = 0,
            .deleted = 0
        },
        {
            .charac_no = 100002,
            .charac_name = "TestMage",
            .job = JOB_MAGE,
            .grow_type = GROW_SECOND,
            .level = 70,
            .exp = 567890,
            .hp = 35000,
            .max_hp = 35000,
            .mp = 30000,
            .max_mp = 30000,
            .village_id = 2,
            .gold = 50000000,
            .fatigue = 100,
            .create_time = 1710000000,
            .last_play_time = 1732500000,
            .slot = 1,
            .deleted = 0
        }
    },
    .last_login_time = 1732550400,
    .premium_expire = 0,
    .cera_point = 0,
    .cera_point_gift = 0
};

/*==============================================================================
 * Packet Building Helpers
 *============================================================================*/

/**
 * Build USERINFO notification packet (NOTIPACKET_USERINFO)
 * Sent to client after successful login with character list
 */
static int BuildUserInfoPacket(uint8_t* buffer, int max_len, const AccountInfo* account)
{
    if (!buffer || !account || max_len < 256) return -1;

    int offset = 0;

    /* Result code (0 = success) */
    buffer[offset++] = 0;

    /* Account UID (4 bytes, little-endian) */
    memcpy(buffer + offset, &account->account_uid, 4);
    offset += 4;

    /* Account grade */
    buffer[offset++] = account->account_grade;

    /* Character count */
    buffer[offset++] = account->character_count;

    /* Character list */
    for (int i = 0; i < account->character_count && i < MAX_CHARACTERS; i++) {
        const CharacterInfo* ch = &account->characters[i];

        /* Character number (4 bytes) */
        memcpy(buffer + offset, &ch->charac_no, 4);
        offset += 4;

        /* Character name (16 bytes, null-terminated) */
        memset(buffer + offset, 0, MAX_CHARAC_NAME);
        strncpy((char*)(buffer + offset), ch->charac_name, MAX_CHARAC_NAME - 1);
        offset += MAX_CHARAC_NAME;

        /* Job class (1 byte) */
        buffer[offset++] = ch->job;

        /* Grow type (1 byte) */
        buffer[offset++] = ch->grow_type;

        /* Level (1 byte) */
        buffer[offset++] = ch->level;

        /* Slot (1 byte) */
        buffer[offset++] = ch->slot;

        /* HP (2 bytes) */
        memcpy(buffer + offset, &ch->hp, 2);
        offset += 2;

        /* Max HP (2 bytes) */
        memcpy(buffer + offset, &ch->max_hp, 2);
        offset += 2;

        /* Gold (4 bytes) */
        memcpy(buffer + offset, &ch->gold, 4);
        offset += 4;

        /* Village ID (4 bytes) */
        memcpy(buffer + offset, &ch->village_id, 4);
        offset += 4;

        /* Delete status (1 byte) */
        buffer[offset++] = ch->deleted;
    }

    /* Premium expire time */
    memcpy(buffer + offset, &account->premium_expire, 4);
    offset += 4;

    /* CERA point */
    memcpy(buffer + offset, &account->cera_point, 4);
    offset += 4;

    return offset;
}

/**
 * Build character creation success response
 */
static int BuildCharacterCreatedPacket(uint8_t* buffer, int max_len, const CharacterInfo* ch)
{
    if (!buffer || !ch || max_len < 64) return -1;

    int offset = 0;

    /* Result code (0 = success) */
    buffer[offset++] = 0;

    /* Character number */
    memcpy(buffer + offset, &ch->charac_no, 4);
    offset += 4;

    /* Character name */
    memset(buffer + offset, 0, MAX_CHARAC_NAME);
    strncpy((char*)(buffer + offset), ch->charac_name, MAX_CHARAC_NAME - 1);
    offset += MAX_CHARAC_NAME;

    /* Job */
    buffer[offset++] = ch->job;

    /* Grow type */
    buffer[offset++] = ch->grow_type;

    /* Slot */
    buffer[offset++] = ch->slot;

    return offset;
}

/*==============================================================================
 * Dispatcher Handler Implementations
 *============================================================================*/

static int Dispatcher_CheckConnection(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    printf("[Dispatcher] CHECK_CONNECTION from fd=%d\n", session->fd);

    /* Echo back to confirm connection */
    uint8_t response[4] = {0};
    return GameServer_SendPacket(session, ENUM_NOTIPACKET_CHECK_CONNECTION, response, sizeof(response));
}

static int Dispatcher_Login(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] LOGIN from fd=%d, len=%d\n", session->fd, len);

    if (len < 4) {
        printf("[Dispatcher] LOGIN packet too short\n");
        uint8_t response[4] = {1, 0, 0, 0};  /* Error code 1: invalid packet */
        return GameServer_SendPacket(session, ENUM_NOTIPACKET_USERINFO, response, sizeof(response));
    }

    /* In production: Parse account_name, password from packet
     * Validate against database
     * For now: accept any login and return mock user data
     */

    /* Parse account name if present */
    char account_name[MAX_ACCOUNT_NAME] = {0};
    int name_len = data[0];
    if (name_len > 0 && name_len < MAX_ACCOUNT_NAME && name_len + 1 <= len) {
        memcpy(account_name, data + 1, name_len);
        account_name[name_len] = '\0';
        printf("[Dispatcher] LOGIN account: %s\n", account_name);
    }

    /* Update session state */
    session->state = SESSION_STATE_AUTHENTICATED;
    session->account_id = g_mock_account.account_uid;
    strncpy(session->account_name, g_mock_account.account_name, sizeof(session->account_name) - 1);

    /* Build and send USERINFO response */
    uint8_t response[512];
    int response_len = BuildUserInfoPacket(response, sizeof(response), &g_mock_account);

    if (response_len < 0) {
        printf("[Dispatcher] Failed to build USERINFO packet\n");
        uint8_t error[4] = {2, 0, 0, 0};  /* Error code 2: internal error */
        return GameServer_SendPacket(session, ENUM_NOTIPACKET_USERINFO, error, sizeof(error));
    }

    printf("[Dispatcher] LOGIN success, sending USERINFO (%d bytes)\n", response_len);
    return GameServer_SendPacket(session, ENUM_NOTIPACKET_USERINFO, response, response_len);
}

static int Dispatcher_Exit(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    printf("[Dispatcher] EXIT from fd=%d\n", session->fd);

    session->state = SESSION_STATE_DISCONNECTING;

    /* Send exit acknowledgment */
    uint8_t response[4] = {0};
    GameServer_SendPacket(session, ENUM_NOTIPACKET_REQUEST_EXIT, response, sizeof(response));

    return -1;  /* Signal to close connection */
}

static int Dispatcher_GetUserInfo(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    printf("[Dispatcher] GET_USERINFO from fd=%d\n", session->fd);

    if (session->state < SESSION_STATE_AUTHENTICATED) {
        printf("[Dispatcher] GET_USERINFO: not authenticated\n");
        uint8_t error[4] = {3, 0, 0, 0};  /* Error code 3: not authenticated */
        return GameServer_SendPacket(session, ENUM_NOTIPACKET_USERINFO, error, sizeof(error));
    }

    /* Send current user info */
    uint8_t response[512];
    int response_len = BuildUserInfoPacket(response, sizeof(response), &g_mock_account);

    if (response_len > 0) {
        return GameServer_SendPacket(session, ENUM_NOTIPACKET_USERINFO, response, response_len);
    }

    return 0;
}

static int Dispatcher_SelectCharacter(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] SELECT_CHARACTER from fd=%d\n", session->fd);

    if (session->state < SESSION_STATE_AUTHENTICATED) {
        printf("[Dispatcher] SELECT_CHARACTER: not authenticated\n");
        return 0;
    }

    if (len < 4) {
        printf("[Dispatcher] SELECT_CHARACTER: packet too short\n");
        return 0;
    }

    /* Parse character number */
    uint32_t charac_no;
    memcpy(&charac_no, data, 4);

    printf("[Dispatcher] SELECT_CHARACTER: charac_no=%u\n", charac_no);

    /* Find character in account */
    const CharacterInfo* selected = NULL;
    for (int i = 0; i < g_mock_account.character_count; i++) {
        if (g_mock_account.characters[i].charac_no == charac_no) {
            selected = &g_mock_account.characters[i];
            break;
        }
    }

    if (!selected) {
        printf("[Dispatcher] SELECT_CHARACTER: character not found\n");
        uint8_t error[4] = {4, 0, 0, 0};  /* Error code 4: character not found */
        return GameServer_SendPacket(session, ENUM_NOTIPACKET_CHARACTER_STATUS, error, sizeof(error));
    }

    /* Update session */
    session->state = SESSION_STATE_IN_GAME;
    session->character_id = charac_no;
    strncpy(session->character_name, selected->charac_name, sizeof(session->character_name) - 1);

    /* Build CHARACTER_STATUS response */
    uint8_t response[256];
    int offset = 0;

    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &selected->charac_no, 4);
    offset += 4;
    memset(response + offset, 0, MAX_CHARAC_NAME);
    strncpy((char*)(response + offset), selected->charac_name, MAX_CHARAC_NAME - 1);
    offset += MAX_CHARAC_NAME;
    response[offset++] = selected->job;
    response[offset++] = selected->grow_type;
    response[offset++] = selected->level;
    memcpy(response + offset, &selected->exp, 4);
    offset += 4;
    memcpy(response + offset, &selected->hp, 2);
    offset += 2;
    memcpy(response + offset, &selected->max_hp, 2);
    offset += 2;
    memcpy(response + offset, &selected->mp, 2);
    offset += 2;
    memcpy(response + offset, &selected->max_mp, 2);
    offset += 2;
    memcpy(response + offset, &selected->gold, 4);
    offset += 4;
    memcpy(response + offset, &selected->village_id, 4);
    offset += 4;
    memcpy(response + offset, &selected->fatigue, 4);
    offset += 4;

    printf("[Dispatcher] SELECT_CHARACTER success: %s (Lv.%d)\n",
           selected->charac_name, selected->level);

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_CHARACTER_STATUS, response, offset);
}

static int Dispatcher_CreateCharacter(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] CREATE_CHARACTER from fd=%d\n", session->fd);

    if (session->state < SESSION_STATE_AUTHENTICATED) {
        printf("[Dispatcher] CREATE_CHARACTER: not authenticated\n");
        return 0;
    }

    if (len < 20) {  /* name(16) + job(1) + grow(1) + slot(1) + padding */
        printf("[Dispatcher] CREATE_CHARACTER: packet too short\n");
        uint8_t error[4] = {1, 0, 0, 0};
        return GameServer_SendPacket(session, ENUM_NOTIPACKET_USERINFO, error, sizeof(error));
    }

    /* Parse creation data */
    char charac_name[MAX_CHARAC_NAME];
    memset(charac_name, 0, sizeof(charac_name));
    memcpy(charac_name, data, MAX_CHARAC_NAME - 1);
    uint8_t job = data[16];
    uint8_t grow_type = data[17];
    uint8_t slot = data[18];

    printf("[Dispatcher] CREATE_CHARACTER: name=%s, job=%d, grow=%d, slot=%d\n",
           charac_name, job, grow_type, slot);

    /* Validate */
    if (strlen(charac_name) < 2) {
        printf("[Dispatcher] CREATE_CHARACTER: name too short\n");
        uint8_t error[4] = {5, 0, 0, 0};  /* Error code 5: invalid name */
        return GameServer_SendPacket(session, ENUM_NOTIPACKET_USERINFO, error, sizeof(error));
    }

    if (job >= JOB_MAX) {
        printf("[Dispatcher] CREATE_CHARACTER: invalid job\n");
        uint8_t error[4] = {6, 0, 0, 0};  /* Error code 6: invalid job */
        return GameServer_SendPacket(session, ENUM_NOTIPACKET_USERINFO, error, sizeof(error));
    }

    if (g_mock_account.character_count >= MAX_CHARACTERS) {
        printf("[Dispatcher] CREATE_CHARACTER: max characters reached\n");
        uint8_t error[4] = {7, 0, 0, 0};  /* Error code 7: max characters */
        return GameServer_SendPacket(session, ENUM_NOTIPACKET_USERINFO, error, sizeof(error));
    }

    /* Create new character (mock) */
    CharacterInfo* new_char = &g_mock_account.characters[g_mock_account.character_count];
    memset(new_char, 0, sizeof(CharacterInfo));
    new_char->charac_no = 100000 + g_mock_account.character_count + 3;
    strncpy(new_char->charac_name, charac_name, MAX_CHARAC_NAME - 1);
    new_char->job = job;
    new_char->grow_type = grow_type;
    new_char->level = 1;
    new_char->exp = 0;
    new_char->hp = 1000;
    new_char->max_hp = 1000;
    new_char->mp = 500;
    new_char->max_mp = 500;
    new_char->village_id = 1;  /* Start village */
    new_char->gold = 10000;
    new_char->fatigue = 156;
    new_char->create_time = (uint32_t)time(NULL);
    new_char->last_play_time = new_char->create_time;
    new_char->slot = slot;
    new_char->deleted = 0;

    g_mock_account.character_count++;

    printf("[Dispatcher] CREATE_CHARACTER success: %s (charac_no=%u)\n",
           new_char->charac_name, new_char->charac_no);

    /* Send updated user info */
    uint8_t response[512];
    int response_len = BuildUserInfoPacket(response, sizeof(response), &g_mock_account);
    return GameServer_SendPacket(session, ENUM_NOTIPACKET_USERINFO, response, response_len);
}

static int Dispatcher_DeleteCharacter(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] DELETE_CHARACTER from fd=%d\n", session->fd);

    if (session->state < SESSION_STATE_AUTHENTICATED) {
        return 0;
    }

    if (len < 4) {
        return 0;
    }

    uint32_t charac_no;
    memcpy(&charac_no, data, 4);

    printf("[Dispatcher] DELETE_CHARACTER: charac_no=%u\n", charac_no);

    /* Find and mark character as deleted */
    for (int i = 0; i < g_mock_account.character_count; i++) {
        if (g_mock_account.characters[i].charac_no == charac_no) {
            g_mock_account.characters[i].deleted = 1;
            g_mock_account.characters[i].delete_time = (uint32_t)time(NULL);
            printf("[Dispatcher] DELETE_CHARACTER success\n");
            break;
        }
    }

    /* Send updated user info */
    uint8_t response[512];
    int response_len = BuildUserInfoPacket(response, sizeof(response), &g_mock_account);
    return GameServer_SendPacket(session, ENUM_NOTIPACKET_USERINFO, response, response_len);
}

static int Dispatcher_ReturnSelectCharacter(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    printf("[Dispatcher] RETURN_SELECT_CHARACTER from fd=%d\n", session->fd);

    /* Return to character selection screen */
    session->state = SESSION_STATE_AUTHENTICATED;
    session->character_id = 0;
    memset(session->character_name, 0, sizeof(session->character_name));

    /* Send user info (character list) */
    uint8_t response[512];
    int response_len = BuildUserInfoPacket(response, sizeof(response), &g_mock_account);
    return GameServer_SendPacket(session, ENUM_NOTIPACKET_USERINFO, response, response_len);
}

static int Dispatcher_StartGame(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    printf("[Dispatcher] START_GAME from fd=%d (character=%s)\n",
           session->fd, session->character_name);

    if (session->state < SESSION_STATE_IN_GAME) {
        printf("[Dispatcher] START_GAME: not in game state\n");
        return 0;
    }

    /* Send START_GAME notification */
    uint8_t response[64];
    int offset = 0;

    response[offset++] = 0;  /* Success */

    /* Dungeon/map info would go here in production */
    memset(response + offset, 0, 16);
    offset += 16;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_START_GAME, response, offset);
}

static int Dispatcher_SelectDungeon(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] SELECT_DUNGEON from fd=%d\n", session->fd);

    if (len < 4) {
        return 0;
    }

    uint32_t dungeon_id;
    memcpy(&dungeon_id, data, 4);

    printf("[Dispatcher] SELECT_DUNGEON: dungeon_id=%u\n", dungeon_id);

    /* Send dungeon info */
    uint8_t response[64];
    int offset = 0;

    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &dungeon_id, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_DUNGEON_INFO, response, offset);
}

static int Dispatcher_FinishLoading(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    printf("[Dispatcher] FINISH_LOADING from fd=%d\n", session->fd);

    uint8_t response[4] = {0};
    return GameServer_SendPacket(session, ENUM_NOTIPACKET_FINISH_LOADING, response, sizeof(response));
}

static int Dispatcher_SetPlayResult(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] SET_PLAY_RESULT from fd=%d, len=%d\n", session->fd, len);

    /* Parse result (simplified) */
    uint8_t result_type = len > 0 ? data[0] : 0;

    printf("[Dispatcher] SET_PLAY_RESULT: type=%d\n", result_type);

    /* Send play result notification */
    uint8_t response[64];
    int offset = 0;

    response[offset++] = 0;  /* Success */
    response[offset++] = result_type;

    /* Rewards, exp gain, etc. would go here */

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_PLAY_RESULT, response, offset);
}

static int Dispatcher_SendMessage(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] SEND_MESSAGE from fd=%d\n", session->fd);

    if (len < 2) {
        return 0;
    }

    uint8_t msg_type = data[0];
    uint8_t msg_len = data[1];

    if (msg_len + 2 > len) {
        return 0;
    }

    char message[256];
    memset(message, 0, sizeof(message));
    memcpy(message, data + 2, msg_len < sizeof(message) - 1 ? msg_len : sizeof(message) - 1);

    printf("[Dispatcher] SEND_MESSAGE: type=%d, msg=\"%s\"\n", msg_type, message);

    /* Broadcast message to other players (simplified) */
    /* In production: use proper message routing based on type */

    uint8_t response[300];
    int offset = 0;

    response[offset++] = msg_type;
    response[offset++] = (uint8_t)strlen(session->character_name);
    memcpy(response + offset, session->character_name, strlen(session->character_name));
    offset += strlen(session->character_name);
    response[offset++] = msg_len;
    memcpy(response + offset, message, msg_len);
    offset += msg_len;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_MESSAGE, response, offset);
}

static int Dispatcher_SetPartyInfo(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    printf("[Dispatcher] SET_PARTY_INFO from fd=%d\n", session->fd);

    uint8_t response[64];
    memset(response, 0, sizeof(response));
    return GameServer_SendPacket(session, ENUM_NOTIPACKET_PARTY_INFO, response, 16);
}

static int Dispatcher_LeaveParty(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    printf("[Dispatcher] LEAVE_PARTY from fd=%d\n", session->fd);

    uint8_t response[4] = {0};
    return GameServer_SendPacket(session, ENUM_NOTIPACKET_WALKOUT, response, sizeof(response));
}

static int Dispatcher_RequestPeer(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    printf("[Dispatcher] REQUEST_PEER from fd=%d\n", session->fd);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    return GameServer_SendPacket(session, ENUM_NOTIPACKET_REQUEST_PEER, response, sizeof(response));
}

static int Dispatcher_ResponsePeer(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    printf("[Dispatcher] RESPONSE_PEER from fd=%d\n", session->fd);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    return GameServer_SendPacket(session, ENUM_NOTIPACKET_RESPONSE_PEER, response, sizeof(response));
}

static int Dispatcher_MakePvpRoom(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    printf("[Dispatcher] MAKE_PVP_ROOM from fd=%d\n", session->fd);

    uint8_t response[64];
    memset(response, 0, sizeof(response));
    return GameServer_SendPacket(session, ENUM_NOTIPACKET_PVP_ROOM_INFO, response, 32);
}

static int Dispatcher_EnterPvpRoom(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    printf("[Dispatcher] ENTER_PVP_ROOM from fd=%d\n", session->fd);

    uint8_t response[64];
    memset(response, 0, sizeof(response));
    return GameServer_SendPacket(session, ENUM_NOTIPACKET_PVP_SEAT_STATE, response, 16);
}

static int Dispatcher_PvpHeartBeat(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    /* Silent heartbeat - no response needed */
    session->last_activity = (uint32_t)time(NULL);
    return 0;
}

static int Dispatcher_Default(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    printf("[Dispatcher] Unhandled packet from fd=%d\n", session->fd);
    g_stats.unknown_packets++;
    return 0;
}

/*==============================================================================
 * Phase 3: Item Operation Handlers
 *============================================================================*/

static int Dispatcher_BuyItem(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] BUY_ITEM from fd=%d\n", session->fd);

    if (session->state < SESSION_STATE_IN_GAME || len < 8) {
        return 0;
    }

    /* Parse item purchase request */
    uint32_t item_id, quantity;
    memcpy(&item_id, data, 4);
    memcpy(&quantity, data + 4, 4);

    printf("[Dispatcher] BUY_ITEM: item=%u, qty=%u\n", item_id, quantity);

    /* TODO: Check gold, add item via DBService */
    /* For now: simulate purchase success */

    uint8_t response[32];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &item_id, 4);
    offset += 4;
    memcpy(response + offset, &quantity, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_BUY_ITEM, response, offset);
}

static int Dispatcher_SellItem(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] SELL_ITEM from fd=%d\n", session->fd);

    if (session->state < SESSION_STATE_IN_GAME || len < 8) {
        return 0;
    }

    uint32_t ui_id, quantity;
    memcpy(&ui_id, data, 4);
    memcpy(&quantity, data + 4, 4);

    printf("[Dispatcher] SELL_ITEM: ui_id=%u, qty=%u\n", ui_id, quantity);

    /* TODO: Remove item, add gold via DBService */

    uint8_t response[32];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &ui_id, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_SELL_ITEM, response, offset);
}

static int Dispatcher_UseItem(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] USE_ITEM from fd=%d\n", session->fd);

    if (session->state < SESSION_STATE_IN_GAME || len < 4) {
        return 0;
    }

    uint32_t ui_id;
    memcpy(&ui_id, data, 4);

    printf("[Dispatcher] USE_ITEM: ui_id=%u\n", ui_id);

    uint8_t response[16];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &ui_id, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_USE_ITEM, response, offset);
}

static int Dispatcher_MoveItem(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] MOVE_ITEM from fd=%d\n", session->fd);

    if (session->state < SESSION_STATE_IN_GAME || len < 8) {
        return 0;
    }

    uint32_t ui_id, new_slot;
    memcpy(&ui_id, data, 4);
    memcpy(&new_slot, data + 4, 4);

    printf("[Dispatcher] MOVE_ITEM: ui_id=%u, slot=%u\n", ui_id, new_slot);

    /* Update item slot in database */
    DBService_UpdateItemSlot(ui_id, new_slot);

    uint8_t response[16];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &ui_id, 4);
    offset += 4;
    memcpy(response + offset, &new_slot, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_MOVE_ITEM, response, offset);
}

static int Dispatcher_EquipItem(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] EQUIP_ITEM from fd=%d\n", session->fd);

    if (session->state < SESSION_STATE_IN_GAME || len < 8) {
        return 0;
    }

    uint32_t ui_id, equip_slot;
    memcpy(&ui_id, data, 4);
    memcpy(&equip_slot, data + 4, 4);

    printf("[Dispatcher] EQUIP_ITEM: ui_id=%u, equip_slot=%u\n", ui_id, equip_slot);

    uint8_t response[16];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &ui_id, 4);
    offset += 4;
    memcpy(response + offset, &equip_slot, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_EQUIP_ITEM, response, offset);
}

static int Dispatcher_UnequipItem(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] UNEQUIP_ITEM from fd=%d\n", session->fd);

    if (session->state < SESSION_STATE_IN_GAME || len < 4) {
        return 0;
    }

    uint32_t equip_slot;
    memcpy(&equip_slot, data, 4);

    printf("[Dispatcher] UNEQUIP_ITEM: equip_slot=%u\n", equip_slot);

    uint8_t response[8];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &equip_slot, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_UNEQUIP_ITEM, response, offset);
}

static int Dispatcher_DropItem(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] DROP_ITEM from fd=%d\n", session->fd);

    if (session->state < SESSION_STATE_IN_GAME || len < 4) {
        return 0;
    }

    uint32_t ui_id;
    memcpy(&ui_id, data, 4);

    printf("[Dispatcher] DROP_ITEM: ui_id=%u\n", ui_id);

    /* Remove item from inventory */
    DBService_RemoveItem(ui_id);

    uint8_t response[8];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &ui_id, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_DROP_ITEM, response, offset);
}

static int Dispatcher_PickupItem(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] PICKUP_ITEM from fd=%d\n", session->fd);

    if (session->state < SESSION_STATE_IN_GAME || len < 4) {
        return 0;
    }

    uint32_t drop_id;
    memcpy(&drop_id, data, 4);

    printf("[Dispatcher] PICKUP_ITEM: drop_id=%u\n", drop_id);

    /* TODO: Add item to inventory via DBService */

    uint8_t response[16];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &drop_id, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_PICKUP_ITEM, response, offset);
}

/*==============================================================================
 * Phase 3: Dungeon Operation Handlers
 *============================================================================*/

static int Dispatcher_EnterDungeon(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] ENTER_DUNGEON from fd=%d\n", session->fd);

    if (session->state < SESSION_STATE_IN_GAME || len < 8) {
        return 0;
    }

    uint32_t dungeon_id, difficulty;
    memcpy(&dungeon_id, data, 4);
    memcpy(&difficulty, data + 4, 4);

    printf("[Dispatcher] ENTER_DUNGEON: dungeon=%u, difficulty=%u\n", dungeon_id, difficulty);

    /* Check and consume fatigue */
    int16_t fatigue_cost = 8;  /* Base fatigue cost */
    if (DBService_UseFatigue(session->character_id, fatigue_cost) != 0) {
        uint8_t error[4] = {10, 0, 0, 0};  /* Error: insufficient fatigue */
        return GameServer_SendPacket(session, ENUM_NOTIPACKET_DUNGEON_INFO, error, sizeof(error));
    }

    /* Increment dungeon play count */
    DBService_IncrementDungeonPlayCount(session->character_id);

    uint8_t response[32];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &dungeon_id, 4);
    offset += 4;
    memcpy(response + offset, &difficulty, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_DUNGEON_INFO, response, offset);
}

static int Dispatcher_LeaveDungeon(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    printf("[Dispatcher] LEAVE_DUNGEON from fd=%d\n", session->fd);

    uint8_t response[4] = {0};
    return GameServer_SendPacket(session, ENUM_NOTIPACKET_DUNGEON_INFO, response, sizeof(response));
}

static int Dispatcher_ClearDungeon(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] CLEAR_DUNGEON from fd=%d\n", session->fd);

    if (len < 8) {
        return 0;
    }

    uint32_t dungeon_id, clear_time;
    memcpy(&dungeon_id, data, 4);
    memcpy(&clear_time, data + 4, 4);

    printf("[Dispatcher] CLEAR_DUNGEON: dungeon=%u, time=%us\n", dungeon_id, clear_time);

    /* Update dungeon clear record */
    DBService_UpdateDungeonClear(session->character_id, dungeon_id, clear_time);

    uint8_t response[32];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &dungeon_id, 4);
    offset += 4;
    memcpy(response + offset, &clear_time, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_PLAY_RESULT, response, offset);
}

static int Dispatcher_DungeonReward(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] DUNGEON_REWARD from fd=%d\n", session->fd);

    if (len < 4) {
        return 0;
    }

    uint32_t reward_type;
    memcpy(&reward_type, data, 4);

    printf("[Dispatcher] DUNGEON_REWARD: type=%u\n", reward_type);

    /* TODO: Process rewards - exp, gold, items */
    uint32_t exp_reward = 1000;
    uint32_t gold_reward = 5000;

    /* Add gold to character */
    DBService_AddGold(session->character_id, gold_reward);

    uint8_t response[32];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &exp_reward, 4);
    offset += 4;
    memcpy(response + offset, &gold_reward, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_PLAY_RESULT, response, offset);
}

/*==============================================================================
 * Phase 3: Skill Operation Handlers
 *============================================================================*/

static int Dispatcher_UseSkill(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] USE_SKILL from fd=%d\n", session->fd);

    if (session->state < SESSION_STATE_IN_GAME || len < 8) {
        return 0;
    }

    uint32_t skill_id;
    uint16_t target_x, target_y;
    memcpy(&skill_id, data, 4);
    memcpy(&target_x, data + 4, 2);
    memcpy(&target_y, data + 6, 2);

    printf("[Dispatcher] USE_SKILL: skill=%u, pos=(%u,%u)\n", skill_id, target_x, target_y);

    /* TODO: Check MP, apply skill effects */

    uint8_t response[16];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &skill_id, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_USE_SKILL, response, offset);
}

static int Dispatcher_UpgradeSkill(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] UPGRADE_SKILL from fd=%d\n", session->fd);

    if (session->state < SESSION_STATE_IN_GAME || len < 4) {
        return 0;
    }

    uint32_t skill_id;
    memcpy(&skill_id, data, 4);

    printf("[Dispatcher] UPGRADE_SKILL: skill=%u\n", skill_id);

    /* TODO: Check SP, upgrade skill via DBService */
    int new_level = 1;  /* Placeholder */
    DBService_UpdateSkill(session->character_id, skill_id, new_level);

    uint8_t response[16];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &skill_id, 4);
    offset += 4;
    response[offset++] = (uint8_t)new_level;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_SKILL_LEVEL_UP, response, offset);
}

static int Dispatcher_ResetSkill(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    printf("[Dispatcher] RESET_SKILL from fd=%d\n", session->fd);

    /* TODO: Reset all skills, refund SP */

    uint8_t response[4] = {0};  /* Success */
    return GameServer_SendPacket(session, ENUM_NOTIPACKET_SKILL_LEVEL_UP, response, sizeof(response));
}

static int Dispatcher_GetSkillList(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    printf("[Dispatcher] GET_SKILL_LIST from fd=%d\n", session->fd);

    /* Get skills from database */
    int skill_ids[100];
    int skill_levels[100];
    int count = DBService_GetSkills(session->character_id, skill_ids, skill_levels, 100);

    uint8_t response[512];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    response[offset++] = (uint8_t)(count > 0 ? count : 0);

    for (int i = 0; i < count && offset < 500; i++) {
        memcpy(response + offset, &skill_ids[i], 4);
        offset += 4;
        response[offset++] = (uint8_t)skill_levels[i];
    }

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_SKILL_LIST, response, offset);
}

/*==============================================================================
 * Phase 3: Guild Operation Handlers
 *============================================================================*/

static int Dispatcher_CreateGuild(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] CREATE_GUILD from fd=%d\n", session->fd);

    if (session->state < SESSION_STATE_IN_GAME || len < 32) {
        return 0;
    }

    char guild_name[32];
    memset(guild_name, 0, sizeof(guild_name));
    memcpy(guild_name, data, 31);

    printf("[Dispatcher] CREATE_GUILD: name=%s\n", guild_name);

    /* TODO: Check requirements (gold, level, no current guild) */

    DBGuild guild;
    memset(&guild, 0, sizeof(DBGuild));
    strncpy(guild.guild_name, guild_name, sizeof(guild.guild_name) - 1);
    guild.lev = 1;
    guild.master_id = session->character_id;
    strncpy(guild.master_name, session->character_name, sizeof(guild.master_name) - 1);
    guild.member_count = 1;
    guild.member_max_count = 50;

    int guild_id = DBService_CreateGuild(&guild);

    if (guild_id > 0) {
        /* Add creator as guild master */
        DBService_AddGuildMember(guild_id, session->character_id, 1);  /* 1 = master */
    }

    uint8_t response[64];
    int offset = 0;
    response[offset++] = (guild_id > 0) ? 0 : 1;  /* 0=success, 1=failed */
    memcpy(response + offset, &guild_id, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_GUILD_INFO, response, offset);
}

static int Dispatcher_JoinGuild(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] JOIN_GUILD from fd=%d\n", session->fd);

    if (session->state < SESSION_STATE_IN_GAME || len < 4) {
        return 0;
    }

    uint32_t guild_id;
    memcpy(&guild_id, data, 4);

    printf("[Dispatcher] JOIN_GUILD: guild_id=%u\n", guild_id);

    /* Add member to guild */
    int result = DBService_AddGuildMember(guild_id, session->character_id, 0);  /* 0 = normal member */

    uint8_t response[8];
    int offset = 0;
    response[offset++] = (result == 0) ? 0 : 1;
    memcpy(response + offset, &guild_id, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_GUILD_INFO, response, offset);
}

static int Dispatcher_LeaveGuild(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    printf("[Dispatcher] LEAVE_GUILD from fd=%d\n", session->fd);

    /* Get character's guild */
    DBCharacter ch;
    if (DBService_GetCharacter(session->character_id, &ch) == 0 && ch.guild_id > 0) {
        DBService_RemoveGuildMember(ch.guild_id, session->character_id);
    }

    uint8_t response[4] = {0};  /* Success */
    return GameServer_SendPacket(session, ENUM_NOTIPACKET_GUILD_INFO, response, sizeof(response));
}

static int Dispatcher_GuildInfo(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] GUILD_INFO from fd=%d\n", session->fd);

    uint32_t guild_id = 0;
    if (len >= 4) {
        memcpy(&guild_id, data, 4);
    }

    printf("[Dispatcher] GUILD_INFO: guild_id=%u\n", guild_id);

    DBGuild guild;
    int result = DBService_GetGuild(guild_id, &guild);

    uint8_t response[128];
    int offset = 0;

    if (result == 0) {
        response[offset++] = 0;  /* Success */
        memcpy(response + offset, &guild.guild_id, 4);
        offset += 4;
        memset(response + offset, 0, 32);
        strncpy((char*)(response + offset), guild.guild_name, 31);
        offset += 32;
        response[offset++] = (uint8_t)guild.lev;
        memcpy(response + offset, &guild.member_count, 4);
        offset += 4;
        memcpy(response + offset, &guild.member_max_count, 4);
        offset += 4;
    } else {
        response[offset++] = 1;  /* Guild not found */
    }

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_GUILD_INFO, response, offset);
}

static int Dispatcher_GuildMemberList(ClientSession* session, const uint8_t* data, int len)
{
    printf("[Dispatcher] GUILD_MEMBER_LIST from fd=%d\n", session->fd);

    uint32_t guild_id = 0;
    if (len >= 4) {
        memcpy(&guild_id, data, 4);
    }

    DBCharacter members[100];
    int count = DBService_GetGuildMembers(guild_id, members, 100);

    uint8_t response[2048];
    int offset = 0;

    response[offset++] = 0;  /* Success */
    response[offset++] = (uint8_t)(count > 0 ? count : 0);

    for (int i = 0; i < count && offset < 2000; i++) {
        memcpy(response + offset, &members[i].charac_no, 4);
        offset += 4;
        memset(response + offset, 0, 16);
        strncpy((char*)(response + offset), members[i].charac_name, 15);
        offset += 16;
        response[offset++] = members[i].level;
        response[offset++] = members[i].job;
    }

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_GUILD_MEMBER_LIST, response, offset);
}

/*==============================================================================
 * Phase 4: Trade Handlers
 *============================================================================*/

/* Forward declarations for Phase 4 handlers */
static int Dispatcher_RequestTrade(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_ConfirmTrade(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_CancelTrade(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_AddTradeItem(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_SendMail(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_OpenMailbox(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_ExtractMailItem(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_UpgradeItem(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_TryEnchant(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_AmplifyItem(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_AchievementTrigger(ClientSession* session, const uint8_t* data, int len);
static int Dispatcher_ClaimAchievementReward(ClientSession* session, const uint8_t* data, int len);

/**
 * Request trade with another player
 * Packet: [target_charac_no:4]
 */
static int Dispatcher_RequestTrade(ClientSession* session, const uint8_t* data, int len)
{
    if (len < 4) {
        return -1;
    }

    int target_charac_no = 0;
    memcpy(&target_charac_no, data, 4);

    /* Start direct trade session */
    int session_id = DBService_StartDirectTrade(session->character_id, target_charac_no);
    if (session_id < 0) {
        uint8_t error[4] = {1, 0, 0, 0};  /* Error */
        return GameServer_SendPacket(session, ENUM_NOTIPACKET_TRADE_REQUEST, error, sizeof(error));
    }

    uint8_t response[16];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &session_id, 4);
    offset += 4;
    memcpy(response + offset, &target_charac_no, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_TRADE_REQUEST, response, offset);
}

/**
 * Confirm trade
 * Packet: [trade_session_id:4]
 */
static int Dispatcher_ConfirmTrade(ClientSession* session, const uint8_t* data, int len)
{
    if (len < 4) {
        return -1;
    }

    int trade_session_id = 0;
    memcpy(&trade_session_id, data, 4);

    int result = DBService_ConfirmDirectTrade(trade_session_id, session->character_id);

    uint8_t response[8];
    int offset = 0;
    response[offset++] = (result >= 0) ? 0 : 1;  /* 0=success, 1=error */
    response[offset++] = (result == 1) ? 1 : 0;  /* 1=both confirmed, trade complete */
    memcpy(response + offset, &trade_session_id, 4);
    offset += 4;

    if (result == 1) {
        /* Trade complete - notify both parties */
        return GameServer_SendPacket(session, ENUM_NOTIPACKET_TRADE_COMPLETE, response, offset);
    }
    return GameServer_SendPacket(session, ENUM_NOTIPACKET_TRADE_CONFIRM, response, offset);
}

/**
 * Cancel trade
 * Packet: [trade_session_id:4]
 */
static int Dispatcher_CancelTrade(ClientSession* session, const uint8_t* data, int len)
{
    (void)session;
    if (len < 4) {
        return -1;
    }

    int trade_session_id = 0;
    memcpy(&trade_session_id, data, 4);

    uint8_t response[8];
    int offset = 0;
    response[offset++] = 0;  /* Success - trade cancelled */
    memcpy(response + offset, &trade_session_id, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_TRADE_CANCEL, response, offset);
}

/**
 * Add item to trade
 * Packet: [trade_session_id:4][item_ui_id:4][slot:1]
 */
static int Dispatcher_AddTradeItem(ClientSession* session, const uint8_t* data, int len)
{
    if (len < 9) {
        return -1;
    }

    int trade_session_id = 0;
    int item_ui_id = 0;
    uint8_t slot = 0;

    memcpy(&trade_session_id, data, 4);
    memcpy(&item_ui_id, data + 4, 4);
    slot = data[8];

    uint8_t response[16];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &trade_session_id, 4);
    offset += 4;
    memcpy(response + offset, &item_ui_id, 4);
    offset += 4;
    response[offset++] = slot;
    memcpy(response + offset, &session->character_id, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_TRADE_ADD_ITEM, response, offset);
}

/*==============================================================================
 * Phase 4: Mail Handlers
 *============================================================================*/

/**
 * Send mail
 * Packet: [receiver_name:16][title:64][content:512][attached_item_id:4][attached_gold:4]
 */
static int Dispatcher_SendMail(ClientSession* session, const uint8_t* data, int len)
{
    if (len < 600) {
        return -1;
    }

    DBMail mail = {0};
    mail.sender_id = session->character_id;
    strncpy(mail.sender_name, session->character_name, sizeof(mail.sender_name) - 1);

    /* Parse receiver name and look up ID (simplified - in real impl, query DB) */
    char receiver_name[16] = {0};
    strncpy(receiver_name, (const char*)data, 15);
    mail.receiver_id = 100001;  /* Mock: fixed receiver */

    strncpy(mail.title, (const char*)(data + 16), sizeof(mail.title) - 1);
    strncpy(mail.content, (const char*)(data + 80), sizeof(mail.content) - 1);
    memcpy(&mail.attached_item_id, data + 592, 4);
    memcpy(&mail.attached_gold, data + 596, 4);

    int mail_id = DBService_SendMail(&mail);

    uint8_t response[8];
    int offset = 0;
    response[offset++] = (mail_id > 0) ? 0 : 1;  /* 0=success */
    memcpy(response + offset, &mail_id, 4);
    offset += 4;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_MAIL_SEND, response, offset);
}

/**
 * Open mailbox (get mail list)
 * Packet: empty
 */
static int Dispatcher_OpenMailbox(ClientSession* session, const uint8_t* data, int len)
{
    (void)data;
    (void)len;

    DBMail mails[50];
    int count = DBService_GetMailbox(session->character_id, mails, 50);

    uint8_t response[4096];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    response[offset++] = (uint8_t)(count > 0 ? count : 0);

    for (int i = 0; i < count && offset < 4000; i++) {
        memcpy(response + offset, &mails[i].mail_id, 4);
        offset += 4;
        memset(response + offset, 0, 16);
        strncpy((char*)(response + offset), mails[i].sender_name, 15);
        offset += 16;
        memset(response + offset, 0, 64);
        strncpy((char*)(response + offset), mails[i].title, 63);
        offset += 64;
        response[offset++] = mails[i].is_read;
        response[offset++] = (mails[i].attached_item_id > 0 || mails[i].attached_gold > 0) ? 1 : 0;
    }

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_MAIL_LIST, response, offset);
}

/**
 * Extract mail attachment
 * Packet: [mail_id:4]
 */
static int Dispatcher_ExtractMailItem(ClientSession* session, const uint8_t* data, int len)
{
    if (len < 4) {
        return -1;
    }

    int mail_id = 0;
    memcpy(&mail_id, data, 4);

    int result = DBService_ExtractMailAttachment(mail_id, session->character_id);

    uint8_t response[8];
    int offset = 0;
    response[offset++] = (result == 0) ? 0 : 1;  /* 0=success */
    memcpy(response + offset, &mail_id, 4);
    offset += 4;

    /* Trigger achievement: extract mail */
    DBService_TriggerAchievement(session->character_id, 101);  /* Mail achievement */

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_MAIL_EXTRACT, response, offset);
}

/*==============================================================================
 * Phase 4: Enhancement Handlers
 *============================================================================*/

/**
 * Upgrade item (reinforce)
 * Packet: [item_ui_id:4][catalyst_ui_id:4]
 */
static int Dispatcher_UpgradeItem(ClientSession* session, const uint8_t* data, int len)
{
    if (len < 8) {
        return -1;
    }

    int item_ui_id = 0;
    int catalyst_ui_id = 0;
    memcpy(&item_ui_id, data, 4);
    memcpy(&catalyst_ui_id, data + 4, 4);

    /* Get current upgrade level to determine success rate */
    int8_t current_upgrade = 0;
    DBService_GetItemUpgradeInfo(item_ui_id, &current_upgrade, NULL, NULL);

    /* Success rate decreases with upgrade level */
    int success_rate = 95 - (current_upgrade * 5);
    if (success_rate < 10) success_rate = 10;

    /* Destroy on fail at +10 and above */
    int destroy_on_fail = (current_upgrade >= 10) ? 1 : 0;

    int result = DBService_UpgradeItem(item_ui_id, success_rate, destroy_on_fail);

    uint8_t response[16];
    int offset = 0;
    if (result == -2) {
        response[offset++] = 2;  /* Item destroyed */
    } else if (result >= 0) {
        response[offset++] = 0;  /* Success or maintained */
        if (result > current_upgrade) {
            /* Trigger achievement: upgrade success */
            DBService_TriggerAchievement(session->character_id, 201);
        }
    } else {
        response[offset++] = 1;  /* Error */
    }
    memcpy(response + offset, &item_ui_id, 4);
    offset += 4;
    response[offset++] = (result >= 0) ? (uint8_t)result : 0;  /* New level */
    response[offset++] = (uint8_t)success_rate;

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_UPGRADE_RESULT, response, offset);
}

/**
 * Enchant item
 * Packet: [item_ui_id:4][catalyst_ui_id:4][enchant_type:1]
 */
static int Dispatcher_TryEnchant(ClientSession* session, const uint8_t* data, int len)
{
    if (len < 9) {
        return -1;
    }

    int item_ui_id = 0;
    int catalyst_ui_id = 0;
    uint8_t enchant_type = 0;

    memcpy(&item_ui_id, data, 4);
    memcpy(&catalyst_ui_id, data + 4, 4);
    enchant_type = data[8];

    /* Random enchant value 1-10 */
    int enchant_value = (rand() % 10) + 1;

    int result = DBService_EnchantItem(item_ui_id, enchant_type, enchant_value);

    uint8_t response[16];
    int offset = 0;
    response[offset++] = (result == 0) ? 0 : 1;
    memcpy(response + offset, &item_ui_id, 4);
    offset += 4;
    response[offset++] = enchant_type;
    response[offset++] = (uint8_t)enchant_value;

    if (result == 0) {
        /* Trigger achievement: enchant success */
        DBService_TriggerAchievement(session->character_id, 202);
    }

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_ENCHANT_RESULT, response, offset);
}

/**
 * Amplify item
 * Packet: [item_ui_id:4][catalyst_ui_id:4]
 */
static int Dispatcher_AmplifyItem(ClientSession* session, const uint8_t* data, int len)
{
    if (len < 8) {
        return -1;
    }

    int item_ui_id = 0;
    int catalyst_ui_id = 0;
    memcpy(&item_ui_id, data, 4);
    memcpy(&catalyst_ui_id, data + 4, 4);

    /* Random amplify value 1-5 */
    int amplify_value = (rand() % 5) + 1;

    int result = DBService_AmplifyItem(item_ui_id, amplify_value);

    uint8_t response[12];
    int offset = 0;
    response[offset++] = (result == 0) ? 0 : 1;
    memcpy(response + offset, &item_ui_id, 4);
    offset += 4;
    response[offset++] = (uint8_t)amplify_value;

    if (result == 0) {
        /* Trigger achievement: amplify success */
        DBService_TriggerAchievement(session->character_id, 203);
    }

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_AMPLIFY_RESULT, response, offset);
}

/*==============================================================================
 * Phase 4: Achievement Handlers
 *============================================================================*/

/**
 * Achievement trigger (manual trigger from client)
 * Packet: [achievement_id:4]
 */
static int Dispatcher_AchievementTrigger(ClientSession* session, const uint8_t* data, int len)
{
    if (len < 4) {
        return -1;
    }

    int achievement_id = 0;
    memcpy(&achievement_id, data, 4);

    int result = DBService_TriggerAchievement(session->character_id, achievement_id);

    /* Get current progress */
    DBAchievement achievements[1];
    int count = DBService_GetAchievements(session->character_id, achievements, 1);

    uint8_t response[16];
    int offset = 0;
    response[offset++] = (result >= 0) ? 0 : 1;
    response[offset++] = (result == 1) ? 1 : 0;  /* 1 = completed */
    memcpy(response + offset, &achievement_id, 4);
    offset += 4;
    if (count > 0) {
        memcpy(response + offset, &achievements[0].progress, 4);
        offset += 4;
        memcpy(response + offset, &achievements[0].target, 4);
        offset += 4;
    }

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_ACHIEVEMENT_PROGRESS, response, offset);
}

/**
 * Claim achievement reward
 * Packet: [achievement_id:4]
 */
static int Dispatcher_ClaimAchievementReward(ClientSession* session, const uint8_t* data, int len)
{
    if (len < 4) {
        return -1;
    }

    int achievement_id = 0;
    memcpy(&achievement_id, data, 4);

    int result = DBService_ClaimAchievementReward(session->character_id, achievement_id);

    uint8_t response[12];
    int offset = 0;
    response[offset++] = (result == 0) ? 0 : 1;
    memcpy(response + offset, &achievement_id, 4);
    offset += 4;

    /* In real implementation, give reward items/gold here */
    if (result == 0) {
        DBService_AddGold(session->character_id, 10000);  /* Mock reward: 10000 gold */
    }

    return GameServer_SendPacket(session, ENUM_NOTIPACKET_ACHIEVEMENT_REWARD, response, offset);
}

/*==============================================================================
 * Public API
 *============================================================================*/

int Dispatcher_Initialize(void)
{
    if (g_handlers_initialized) {
        return 0;
    }

    memset(g_handlers, 0, sizeof(g_handlers));

    /* Register all handlers */
    #define REG(pkt_type, fn, fn_name) do { \
        g_handlers[pkt_type].handler = fn; \
        g_handlers[pkt_type].name = fn_name; \
        g_handlers[pkt_type].enabled = 1; \
    } while(0)

    /* Connection & Authentication */
    REG(ENUM_CMDPACKET_CHECK_CONNECTION, Dispatcher_CheckConnection, "CheckConnection");
    REG(ENUM_CMDPACKET_LOGIN, Dispatcher_Login, "Login");
    REG(ENUM_CMDPACKET_EXIT, Dispatcher_Exit, "Exit");
    REG(ENUM_CMDPACKET_GET_USERINFO, Dispatcher_GetUserInfo, "GetUserInfo");

    /* Character Management */
    REG(ENUM_CMDPACKET_SELECT_CHARACTER, Dispatcher_SelectCharacter, "SelectCharacter");
    REG(ENUM_CMDPACKET_CREATE_CHARACTER, Dispatcher_CreateCharacter, "CreateCharacter");
    REG(ENUM_CMDPACKET_DELETE_CHARACTER, Dispatcher_DeleteCharacter, "DeleteCharacter");
    REG(ENUM_CMDPACKET_RETURN_SELECT_CHARACTER, Dispatcher_ReturnSelectCharacter, "ReturnSelectCharacter");

    /* Game Start & Dungeon */
    REG(ENUM_CMDPACKET_START_GAME, Dispatcher_StartGame, "StartGame");
    REG(ENUM_CMDPACKET_SELECT_DUNGEON, Dispatcher_SelectDungeon, "SelectDungeon");
    REG(ENUM_CMDPACKET_FINISH_LOADING, Dispatcher_FinishLoading, "FinishLoading");
    REG(ENUM_CMDPACKET_SET_PLAY_RESULT, Dispatcher_SetPlayResult, "SetPlayResult");

    /* Party & Social */
    REG(ENUM_CMDPACKET_SEND_MESSAGE, Dispatcher_SendMessage, "SendMessage");
    REG(ENUM_CMDPACKET_SET_PARTY_INFO, Dispatcher_SetPartyInfo, "SetPartyInfo");
    REG(ENUM_CMDPACKET_LEAVE_PARTY, Dispatcher_LeaveParty, "LeaveParty");
    REG(ENUM_CMDPACKET_REQUEST_PEER, Dispatcher_RequestPeer, "RequestPeer");
    REG(ENUM_CMDPACKET_RESPONSE_PEER, Dispatcher_ResponsePeer, "ResponsePeer");

    /* PVP */
    REG(ENUM_CMDPACKET_MAKE_PVP_ROOM, Dispatcher_MakePvpRoom, "MakePvpRoom");
    REG(ENUM_CMDPACKET_ENTER_PVP_ROOM, Dispatcher_EnterPvpRoom, "EnterPvpRoom");
    REG(ENUM_CMDPACKET_PVP_HEART_BEAT, Dispatcher_PvpHeartBeat, "PvpHeartBeat");

    /* Phase 3: Item Operations */
    REG(ENUM_CMDPACKET_BUY_ITEM, Dispatcher_BuyItem, "BuyItem");
    REG(ENUM_CMDPACKET_SELL_ITEM, Dispatcher_SellItem, "SellItem");
    REG(ENUM_CMDPACKET_USE_STACKABLE, Dispatcher_UseItem, "UseItem");
    REG(ENUM_CMDPACKET_MOVE_ITEMSPACE, Dispatcher_MoveItem, "MoveItem");
    REG(ENUM_CMDPACKET_COMPOUND_ITEM, Dispatcher_EquipItem, "EquipItem");
    REG(ENUM_CMDPACKET_DISJOINT_ITEM, Dispatcher_UnequipItem, "UnequipItem");
    REG(ENUM_CMDPACKET_DROP_ITEM, Dispatcher_DropItem, "DropItem");
    REG(ENUM_CMDPACKET_GET_ITEM, Dispatcher_PickupItem, "PickupItem");

    /* Phase 3: Dungeon Operations */
    REG(ENUM_CMDPACKET_SELECT_DUNGEON, Dispatcher_EnterDungeon, "EnterDungeon");
    REG(ENUM_CMDPACKET_GIVEUP_GAME, Dispatcher_LeaveDungeon, "LeaveDungeon");
    REG(ENUM_CMDPACKET_SET_PLAY_RESULT, Dispatcher_ClearDungeon, "ClearDungeon");

    /* Phase 3: Skill Operations */
    REG(ENUM_CMDPACKET_USE_SKILL, Dispatcher_UseSkill, "UseSkill");
    REG(ENUM_CMDPACKET_BUY_SKILL, Dispatcher_UpgradeSkill, "UpgradeSkill");
    REG(ENUM_CMDPACKET_CHANGE_SKILLSLOT, Dispatcher_ResetSkill, "ResetSkill");
    REG(ENUM_CMDPACKET_INCREASE_STATUS, Dispatcher_GetSkillList, "GetSkillList");

    /* Phase 3: Guild Operations */
    REG(ENUM_CMDPACKET_CALL_GUILD_CREATE_RIGHT, Dispatcher_CreateGuild, "CreateGuild");
    REG(ENUM_CMDPACKET_REQUEST_GUILD_ENTER, Dispatcher_JoinGuild, "JoinGuild");
    REG(ENUM_CMDPACKET_MEMBER_SECEDE, Dispatcher_LeaveGuild, "LeaveGuild");
    REG(ENUM_CMDPACKET_GUILD_INFO, Dispatcher_GuildInfo, "GuildInfo");
    REG(ENUM_CMDPACKET_GUILD_MEMER_LIST, Dispatcher_GuildMemberList, "GuildMemberList");

    /* Phase 4: Trade Operations */
    REG(ENUM_CMDPACKET_SET_ITEMTRADE_STATE, Dispatcher_RequestTrade, "RequestTrade");
    REG(ENUM_CMDPACKET_AGREE_ENCHANT, Dispatcher_ConfirmTrade, "ConfirmTrade");
    REG(ENUM_CMDPACKET_CHANGE_CREATURE_TRADE_ATTR, Dispatcher_CancelTrade, "CancelTrade");
    REG(ENUM_CMDPACKET_BUY_PRIVATE_STORE_ITEM, Dispatcher_AddTradeItem, "AddTradeItem");

    /* Phase 4: Mail Operations */
    REG(ENUM_CMDPACKET_MAILBOX_SEND, Dispatcher_SendMail, "SendMail");
    REG(ENUM_CMDPACKET_MAILBOX_OPEN, Dispatcher_OpenMailbox, "OpenMailbox");
    REG(ENUM_CMDPACKET_MAILBOX_EXTRACT_ITEM, Dispatcher_ExtractMailItem, "ExtractMailItem");

    /* Phase 4: Enhancement Operations */
    REG(ENUM_CMDPACKET_UPGRADE_ITEM, Dispatcher_UpgradeItem, "UpgradeItem");
    REG(ENUM_CMDPACKET_TRY_ENCHANT, Dispatcher_TryEnchant, "TryEnchant");
    REG(ENUM_CMDPACKET_INVEST_ITEM_AMPLIFY_OPTION, Dispatcher_AmplifyItem, "AmplifyItem");

    /* Phase 4: Achievement Operations */
    REG(ENUM_CMDPACKET_ACHIEVEMENT_TRIGGER, Dispatcher_AchievementTrigger, "AchievementTrigger");
    REG(ENUM_CMDPACKET_ADVANCE_ALTAR_GET_ACHIEVEMENT_REWARD, Dispatcher_ClaimAchievementReward, "ClaimAchievementReward");

    #undef REG

    g_handlers_initialized = 1;
    printf("[Dispatcher] Initialized with %d handlers\n", MAX_HANDLERS);

    /* Initialize database service */
    if (DBService_Initialize(NULL) != 0) {
        printf("[Dispatcher] Warning: DBService initialization failed, using mock data\n");
    }

    return 0;
}

void Dispatcher_Cleanup(void)
{
    /* Cleanup database service */
    DBService_Cleanup();

    memset(g_handlers, 0, sizeof(g_handlers));
    memset(&g_stats, 0, sizeof(g_stats));
    g_handlers_initialized = 0;
    printf("[Dispatcher] Cleanup complete\n");
}

int Dispatcher_Dispatch(ClientSession* session, uint16_t packet_type, const uint8_t* data, int len)
{
    if (!g_handlers_initialized) {
        Dispatcher_Initialize();
    }

    if (packet_type >= MAX_HANDLERS) {
        printf("[Dispatcher] Packet type %d out of range\n", packet_type);
        return Dispatcher_Default(session, data, len);
    }

    HandlerEntry* entry = &g_handlers[packet_type];

    if (!entry->handler || !entry->enabled) {
        return Dispatcher_Default(session, data, len);
    }

    g_stats.packets_handled++;

    int result = entry->handler(session, data, len);
    if (result < 0) {
        g_stats.handler_errors++;
    }

    return result;
}

void Dispatcher_GetStatistics(unsigned long* handled, unsigned long* unknown, unsigned long* errors)
{
    if (handled) *handled = g_stats.packets_handled;
    if (unknown) *unknown = g_stats.unknown_packets;
    if (errors)  *errors = g_stats.handler_errors;
}

void Dispatcher_PrintRegistry(void)
{
    printf("\n=== Dispatcher Registry ===\n");
    int count = 0;
    for (int i = 0; i < MAX_HANDLERS; i++) {
        if (g_handlers[i].handler) {
            printf("  [%3d] %s %s\n",
                   i,
                   g_handlers[i].name ? g_handlers[i].name : "unnamed",
                   g_handlers[i].enabled ? "(enabled)" : "(disabled)");
            count++;
        }
    }
    printf("Total: %d handlers registered\n", count);
    printf("===========================\n\n");
}
