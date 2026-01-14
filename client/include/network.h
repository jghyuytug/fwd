/**
 * @file network.h
 * @brief Network client for server communication
 */

#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H

#include "client.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NET_BUFFER_SIZE     8192
#define NET_PACKET_MAX      4096
#define NET_CONNECT_TIMEOUT 5000
#define NET_RECV_TIMEOUT    100

/* Connection state */
typedef enum {
    NET_STATE_DISCONNECTED = 0,
    NET_STATE_CONNECTING,
    NET_STATE_CONNECTED,
    NET_STATE_ERROR
} NetState;

/* Packet header (13 bytes) */
typedef struct {
    u8      classification;
    u32     length;
    u16     msg_no;
    u32     checksum;
    u16     sequence;
} PacketHeader;

/* Network client */
typedef struct NetworkClient {
    int         socket;
    NetState    state;
    char        host[64];
    u16         port;

    /* Receive buffer */
    u8          recv_buffer[NET_BUFFER_SIZE];
    int         recv_len;

    /* Send buffer */
    u8          send_buffer[NET_BUFFER_SIZE];
    int         send_len;

    /* Sequence */
    u16         sequence;

    /* Statistics */
    u64         packets_sent;
    u64         packets_recv;
    u64         bytes_sent;
    u64         bytes_recv;

    /* Callbacks */
    void (*on_connect)(struct NetworkClient* client);
    void (*on_disconnect)(struct NetworkClient* client);
    void (*on_packet)(struct NetworkClient* client, u16 msg_no, u8* data, int len);
} NetworkClient;

/**
 * Initialize network client
 * @param client Network client instance
 * @return CLIENT_OK on success
 */
int network_init(NetworkClient* client);

/**
 * Cleanup network client
 * @param client Network client instance
 */
void network_cleanup(NetworkClient* client);

/**
 * Connect to server
 * @param client     Network client instance
 * @param host       Server host
 * @param port       Server port
 * @param timeout_ms Connection timeout
 * @return CLIENT_OK on success
 */
int network_connect(NetworkClient* client, const char* host, u16 port, u32 timeout_ms);

/**
 * Disconnect from server
 * @param client Network client instance
 */
void network_disconnect(NetworkClient* client);

/**
 * Check if connected
 * @param client Network client instance
 * @return true if connected
 */
bool network_is_connected(NetworkClient* client);

/**
 * Update network (poll for packets)
 * @param client Network client instance
 * @return Number of packets received
 */
int network_update(NetworkClient* client);

/**
 * Send raw packet
 * @param client Network client instance
 * @param msg_no Message number
 * @param data   Packet data
 * @param len    Data length
 * @return CLIENT_OK on success
 */
int network_send(NetworkClient* client, u16 msg_no, const u8* data, int len);

/**
 * Send packet with builder
 * @param client Network client instance
 * @param msg_no Message number
 * @return Packet builder context
 */
typedef struct PacketBuilder PacketBuilder;

PacketBuilder* network_begin_packet(NetworkClient* client, u16 msg_no);
void packet_write_u8(PacketBuilder* builder, u8 value);
void packet_write_u16(PacketBuilder* builder, u16 value);
void packet_write_u32(PacketBuilder* builder, u32 value);
void packet_write_i32(PacketBuilder* builder, i32 value);
void packet_write_string(PacketBuilder* builder, const char* str, int max_len);
void packet_write_bytes(PacketBuilder* builder, const u8* data, int len);
int network_end_packet(NetworkClient* client, PacketBuilder* builder);

/*
 * ============================================================================
 * Protocol Message Numbers (Client -> Server)
 * ============================================================================
 */

/* Core (0-20) */
#define MSG_CHECK_CONNECTION    0
#define MSG_LOGIN               1
#define MSG_SET_UDP_IP_PORT     2
#define MSG_EXIT                3
#define MSG_SELECT_CHARACTER    4
#define MSG_CREATE_CHARACTER    5
#define MSG_DELETE_CHARACTER    6
#define MSG_RETURN_SELECT       7
#define MSG_GET_USERINFO        8
#define MSG_PARTY_CREATE        12
#define MSG_PARTY_LEAVE         13
#define MSG_PARTY_KICK          14
#define MSG_START_GAME          15
#define MSG_DUNGEON_ENTER       16
#define MSG_CHAT                17
#define MSG_DELETE_ITEM         18
#define MSG_MOVE_ITEM           19
#define MSG_SORT_ITEM           20

/* NPC/Trade (21-36) */
#define MSG_NPC_SHOP_BUY        21
#define MSG_NPC_SHOP_SELL       24
#define MSG_REPAIR_EQUIPMENT    25
#define MSG_TRADE_REQUEST       26
#define MSG_COMPOUND_ITEM       27
#define MSG_DISJOINT_ITEM       28
#define MSG_CHANGE_SKILLSLOT    30
#define MSG_BUY_SKILL           31
#define MSG_INCREASE_STATUS     32
#define MSG_ACCEPT_QUEST        33
#define MSG_GIVEUP_QUEST        34
#define MSG_SET_QUEST_TRIGGER   35
#define MSG_FINISH_QUEST        36

/* Movement (37-39) */
#define MSG_SET_USER_POSITION   37
#define MSG_SET_USER_AREA       38

/* Combat (40-52) */
#define MSG_FINISH_LOADING      40
#define MSG_USE_SKILL           41
#define MSG_DIE_MONSTER         42
#define MSG_DIE_CHARACTER       43
#define MSG_USE_COIN            44
#define MSG_GIVEUP_GAME         45
#define MSG_GET_ITEM            46
#define MSG_USE_STACKABLE       47
#define MSG_MOVE_MAP            48
#define MSG_SET_PLAY_RESULT     49
#define MSG_DROP_ITEM           50

/* PVP (53-62) */
#define MSG_PVP_CREATE_ROOM     53
#define MSG_PVP_JOIN_ROOM       54
#define MSG_PVP_LEAVE_ROOM      55
#define MSG_PVP_START           56

/* Social (63-81) */
#define MSG_FRIEND_ADD          63
#define MSG_FRIEND_REMOVE       64
#define MSG_GUILD_MEMBER_LIST   70
#define MSG_GUILD_INFO          77
#define MSG_GUILD_JOIN          78
#define MSG_GUILD_LEAVE         81

/* Enhancement (83-96) */
#define MSG_UPGRADE_ITEM        83

/* Mail (97-101) */
#define MSG_MAIL_LIST           97
#define MSG_MAIL_READ           98
#define MSG_MAIL_SEND           99
#define MSG_STORAGE_DEPOSIT_GOLD    100
#define MSG_STORAGE_WITHDRAW_GOLD   101

/* Title System (160-165) */
#define MSG_TITLE_LIST          160
#define MSG_TITLE_EQUIP         161
#define MSG_TITLE_UNEQUIP       162

/* Auction (170-178) */
#define MSG_AUCTION_SEARCH      170
#define MSG_AUCTION_CREATE      171
#define MSG_AUCTION_CANCEL      172
#define MSG_AUCTION_BUY         173
#define MSG_AUCTION_BID         174
#define MSG_AUCTION_MY_LIST     175

/* Storage (180-185) */
#define MSG_STORAGE_OPEN        180
#define MSG_STORAGE_DEPOSIT     181
#define MSG_STORAGE_WITHDRAW    182
#define MSG_STORAGE_EXPAND      183

/* Pet System (190-198) */
#define MSG_PET_SUMMON          190
#define MSG_PET_DISMISS         191
#define MSG_PET_FEED            192
#define MSG_PET_RENAME          193

/* Extended Inventory (200-205) */
#define MSG_INVENTORY_EXPAND    200
#define MSG_EQUIPMENT_CHANGE    201
#define MSG_STAT_RESET          202

/* Extended Dungeon (210-215) */
#define MSG_DUNGEON_CREATE      210
#define MSG_DUNGEON_LEAVE       211
#define MSG_DUNGEON_READY       212
#define MSG_DUNGEON_RESULT      213

/* Extended Party (220-225) */
#define MSG_PARTY_INVITE        220
#define MSG_PARTY_ACCEPT        221
#define MSG_PARTY_DECLINE       222
#define MSG_PARTY_CHANGE_LEADER 223

/* Extended Friend (230-235) */
#define MSG_FRIEND_LIST         230
#define MSG_FRIEND_ACCEPT       231
#define MSG_FRIEND_BLOCK        232
#define MSG_FRIEND_UNBLOCK      233

/* Extended Guild (240-248) */
#define MSG_GUILD_CREATE        240
#define MSG_GUILD_DISBAND       241
#define MSG_GUILD_KICK          242
#define MSG_GUILD_PROMOTE       243
#define MSG_GUILD_DEMOTE        244
#define MSG_GUILD_NOTICE        245
#define MSG_GUILD_SKILL         246

/* Extended Mail (250-253) */
#define MSG_MAIL_DELETE         250
#define MSG_MAIL_CLAIM          251
#define MSG_MAIL_CLAIM_ALL      252

/* Extended Trade (260-267) */
#define MSG_TRADE_ACCEPT        260
#define MSG_TRADE_DECLINE       261
#define MSG_TRADE_ADD_ITEM      262
#define MSG_TRADE_REMOVE_ITEM   263
#define MSG_TRADE_SET_GOLD      264
#define MSG_TRADE_CONFIRM       265
#define MSG_TRADE_CANCEL        266

/* Extended PVP (270-277) */
#define MSG_PVP_INVITE          270
#define MSG_PVP_ACCEPT          271
#define MSG_PVP_DECLINE         272
#define MSG_PVP_READY           273
#define MSG_PVP_RESULT          274

/* Ranking (280-285) */
#define MSG_RANKING_GET         280
#define MSG_RANKING_MY_RANK     281

/* Achievement (290-296) */
#define MSG_ACHIEVEMENT_LIST    290
#define MSG_ACHIEVEMENT_CLAIM   291

/* Phase 7 Extensions (300-306) */
#define MSG_ENHANCE_ITEM        300
#define MSG_SKILL_RESET         301
#define MSG_ITEM_STACK          302
#define MSG_DIFFICULTY_SELECT   303
#define MSG_GUILD_LEVEL_INFO    304
#define MSG_EQUIPMENT_CHECK     305

/* Abyss Dungeon (350-355) */
#define MSG_ABYSS_ENTER         350
#define MSG_ABYSS_RESULT        351

/* Daily/Weekly (380-391) */
#define MSG_DAILY_QUEST_LIST    380
#define MSG_DAILY_QUEST_CLAIM   381
#define MSG_WEEKLY_QUEST_LIST   382
#define MSG_WEEKLY_QUEST_CLAIM  383
#define MSG_EVENT_LIST          390
#define MSG_EVENT_CLAIM         391

/* Tower of Death (410-412) */
#define MSG_TOWER_ENTER         410
#define MSG_TOWER_PROGRESS      411
#define MSG_TOWER_RESULT        412

/* Set System (420) */
#define MSG_SET_EFFECT_INFO     420

/* Season System (430) */
#define MSG_SEASON_INFO         430

/* Equipment Growth (440-441) */
#define MSG_EQUIP_GROWTH_INFO   440
#define MSG_EQUIP_GROWTH_UPGRADE 441

/* Mystery Shop (450-452) */
#define MSG_MYSTERY_SHOP_LIST   450
#define MSG_MYSTERY_SHOP_BUY    451
#define MSG_MYSTERY_SHOP_REFRESH 452

/* Cash Shop (510-514) */
#define MSG_CASH_BALANCE        510
#define MSG_CASH_SHOP_LIST      511
#define MSG_CASH_SHOP_BUY       512
#define MSG_VIP_INFO            513

/* Sign-in (534-536) */
#define MSG_SIGNIN_INFO         534
#define MSG_SIGNIN_CLAIM        535
#define MSG_SIGNIN_MAKEUP       536

/* Cross-Server Match (550-555) */
#define MSG_MATCH_QUEUE         550
#define MSG_MATCH_CANCEL        551
#define MSG_MATCH_STATUS        552
#define MSG_MATCH_ACCEPT        553
#define MSG_MATCH_DECLINE       554
#define MSG_MATCH_RESULT        555

/*
 * ============================================================================
 * Response Message Numbers (Server -> Client)
 * ============================================================================
 */

/* Core Responses (1000-1020) */
#define MSG_RESP_CHECK_CONNECTION   1000
#define MSG_RESP_LOGIN              1001
#define MSG_RESP_SET_UDP            1002
#define MSG_RESP_EXIT               1003
#define MSG_RESP_CHARLIST           1004
#define MSG_RESP_CREATE_CHAR        1005
#define MSG_RESP_DELETE_CHAR        1006
#define MSG_RESP_SELECT_CHAR        1007
#define MSG_RESP_USERINFO           1008
#define MSG_RESP_START_GAME         1015
#define MSG_RESP_DUNGEON_ENTER      1016
#define MSG_RESP_CHAT               1017
#define MSG_RESP_ENTERWORLD         1015

/* Entity Sync (1100-1120) */
#define MSG_RESP_PLAYER_SPAWN       1100
#define MSG_RESP_MONSTER_SPAWN      1101
#define MSG_RESP_ENTITY_MOVE        1102
#define MSG_RESP_ENTITY_ATTACK      1103
#define MSG_RESP_DAMAGE             1104
#define MSG_RESP_ENTITY_DEATH       1105
#define MSG_RESP_ENTITY_DESPAWN     1106
#define MSG_RESP_ENTITY_STATE       1107
#define MSG_RESP_BUFF_ADD           1108
#define MSG_RESP_BUFF_REMOVE        1109
#define MSG_RESP_HP_UPDATE          1110
#define MSG_RESP_MP_UPDATE          1111
#define MSG_RESP_EXP_UPDATE         1112
#define MSG_RESP_LEVEL_UP           1113
#define MSG_RESP_NPC_SPAWN          1114
#define MSG_RESP_ITEM_DROP          1115

/* Inventory Responses (1200-1220) */
#define MSG_RESP_INVENTORY_UPDATE   1200
#define MSG_RESP_EQUIPMENT_UPDATE   1201
#define MSG_RESP_GOLD_UPDATE        1202
#define MSG_RESP_ITEM_USE           1203
#define MSG_RESP_ITEM_ENHANCE       1204
#define MSG_RESP_ITEM_REPAIR        1205

/* Skill Responses (1300-1310) */
#define MSG_RESP_SKILL_LIST         1300
#define MSG_RESP_SKILL_LEARN        1301
#define MSG_RESP_SKILL_UPGRADE      1302
#define MSG_RESP_SKILL_RESET        1303
#define MSG_RESP_SKILL_COOLDOWN     1304

/* Social Responses (1400-1450) */
#define MSG_RESP_FRIEND_LIST        1400
#define MSG_RESP_FRIEND_UPDATE      1401
#define MSG_RESP_GUILD_INFO         1410
#define MSG_RESP_GUILD_MEMBERS      1411
#define MSG_RESP_GUILD_UPDATE       1412
#define MSG_RESP_PARTY_INFO         1420
#define MSG_RESP_PARTY_UPDATE       1421
#define MSG_RESP_PARTY_INVITE       1422
#define MSG_RESP_MAIL_LIST          1430
#define MSG_RESP_MAIL_DETAIL        1431
#define MSG_RESP_MAIL_SEND          1432

/* Dungeon Responses (1500-1520) */
#define MSG_RESP_DUNGEON_INFO       1500
#define MSG_RESP_DUNGEON_ROOM       1501
#define MSG_RESP_DUNGEON_CLEAR      1502
#define MSG_RESP_DUNGEON_REWARD     1503
#define MSG_RESP_DUNGEON_FAIL       1504

/* Trade/Auction Responses (1600-1650) */
#define MSG_RESP_TRADE_REQUEST      1600
#define MSG_RESP_TRADE_UPDATE       1601
#define MSG_RESP_TRADE_COMPLETE     1602
#define MSG_RESP_AUCTION_LIST       1610
#define MSG_RESP_AUCTION_RESULT     1611

/* System Responses (1900-1999) */
#define MSG_RESP_ERROR              1900
#define MSG_RESP_SYSTEM_MESSAGE     1901
#define MSG_RESP_ANNOUNCEMENT       1902
#define MSG_RESP_MAINTENANCE        1903
#define MSG_RESP_KICK               1904

#ifdef __cplusplus
}
#endif

#endif /* CLIENT_NETWORK_H */
