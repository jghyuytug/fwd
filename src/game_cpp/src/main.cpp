/*
 * main.cpp - DNF Game Server (Extended)
 *
 * Implements complete login flow with session management:
 * - CHECK_CONNECTION (msg_no=0)
 * - LOGIN (msg_no=1) with character list
 * - SET_UDP_IP_PORT (msg_no=2)
 * - EXIT (msg_no=3)
 * - SELECT_CHARACTER (msg_no=4)
 * - CREATE_CHARACTER (msg_no=5)
 * - DELETE_CHARACTER (msg_no=6)
 * - RETURN_SELECT_CHARACTER (msg_no=7)
 * - GET_USERINFO (msg_no=8)
 * - START_GAME (msg_no=15)
 * - FINISH_LOADING (msg_no=40)
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

/*==============================================================================
 * Configuration
 *============================================================================*/

#define PORT 10011
#define MAX_EVENTS 64
#define BUFFER_SIZE 65536
#define PACKET_HEADER_SIZE 13

#define MAX_SESSIONS 256
#define MAX_CHARAC_NAME 16
#define MAX_ACCOUNT_NAME 32
#define MAX_CHARACTERS 16

/*==============================================================================
 * Data Structures
 *============================================================================*/

/* Session states */
typedef enum {
    STATE_CONNECTED = 0,
    STATE_AUTHENTICATED,
    STATE_CHAR_SELECT,
    STATE_IN_GAME,
    STATE_DISCONNECTING
} SessionState;

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

/* Character info */
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
    uint8_t  slot;
    uint8_t  deleted;
} CharacterInfo;

/* Account info */
typedef struct {
    uint32_t account_uid;
    char     account_name[MAX_ACCOUNT_NAME];
    uint8_t  account_grade;
    uint8_t  character_count;
    CharacterInfo characters[MAX_CHARACTERS];
    uint32_t premium_expire;
    uint32_t cera_point;
} AccountInfo;

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

    /* Activity tracking */
    uint32_t connect_time;
    uint32_t last_activity;

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

/* Mock account data for testing */
static AccountInfo g_mock_account = {
    10001,          /* account_uid */
    "test_user",    /* account_name */
    0,              /* account_grade */
    2,              /* character_count */
    {               /* characters */
        {
            100001,     /* charac_no */
            "TestSlayer",/* charac_name */
            JOB_SLAYER, /* job */
            1,          /* grow_type */
            85,         /* level */
            1234567,    /* exp */
            50000,      /* hp */
            50000,      /* max_hp */
            30000,      /* mp */
            30000,      /* max_mp */
            1,          /* village_id */
            999999999,  /* gold */
            156,        /* fatigue */
            0,          /* slot */
            0           /* deleted */
        },
        {
            100002,     /* charac_no */
            "TestMage", /* charac_name */
            JOB_MAGE,   /* job */
            2,          /* grow_type */
            70,         /* level */
            567890,     /* exp */
            35000,      /* hp */
            35000,      /* max_hp */
            50000,      /* mp */
            50000,      /* max_mp */
            2,          /* village_id */
            50000000,   /* gold */
            100,        /* fatigue */
            1,          /* slot */
            0           /* deleted */
        }
    },
    0,              /* premium_expire */
    0               /* cera_point */
};

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

/* Destroy session */
static void destroy_session(ClientSession* s) {
    if (s && s->active) {
        s->active = 0;
        g_session_count--;
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

    /* Build header using memcpy for portability */
    packet[0] = 0;  /* classification */
    memcpy(packet + 1, &total_len, 4);  /* length (little-endian) */
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

/* Build USERINFO response (after login) */
static int build_userinfo_packet(uint8_t* buffer, int max_len, const AccountInfo* account) {
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

/* Build CHARACTER_STATUS response (after select) */
static int build_character_status(uint8_t* buffer, int max_len, const CharacterInfo* ch) {
    if (!buffer || !ch || max_len < 128) return -1;

    int offset = 0;

    buffer[offset++] = 0;  /* Success */
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
    memcpy(buffer + offset, &ch->mp, 2);
    offset += 2;
    memcpy(buffer + offset, &ch->max_mp, 2);
    offset += 2;
    memcpy(buffer + offset, &ch->gold, 4);
    offset += 4;
    memcpy(buffer + offset, &ch->village_id, 4);
    offset += 4;
    memcpy(buffer + offset, &ch->fatigue, 4);
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

/* msg_no=1: LOGIN */
static int handle_login(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] LOGIN from fd=%d, data_len=%d\n", s->fd, len);

    /* Parse account name if present */
    if (len > 0) {
        int name_len = data[0];
        if (name_len > 0 && name_len < MAX_ACCOUNT_NAME && name_len + 1 <= len) {
            memcpy(s->account_name, data + 1, name_len);
            s->account_name[name_len] = '\0';
            printf("[GAME] LOGIN account: %s\n", s->account_name);
        }
    }

    /* Update session state */
    s->state = STATE_AUTHENTICATED;
    s->account_id = g_mock_account.account_uid;
    strncpy(s->account_name, g_mock_account.account_name, sizeof(s->account_name) - 1);

    /* Build and send USERINFO response */
    uint8_t response[512];
    int response_len = build_userinfo_packet(response, sizeof(response), &g_mock_account);

    if (response_len > 0) {
        printf("[GAME] LOGIN success, sending USERINFO (%d bytes)\n", response_len);
        return send_response(s, 1, response, response_len);
    }

    /* Error */
    uint8_t error[4] = {1, 0, 0, 0};
    return send_response(s, 1, error, 4);
}

/* msg_no=2: SET_UDP_IP_PORT */
static int handle_set_udp(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] SET_UDP_IP_PORT from fd=%d\n", s->fd);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 2, response, 4);
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

/* msg_no=4: SELECT_CHARACTER */
static int handle_select_character(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] SELECT_CHARACTER from fd=%d\n", s->fd);

    if (s->state < STATE_AUTHENTICATED) {
        printf("[GAME] SELECT_CHARACTER: not authenticated\n");
        return 0;
    }

    if (len < 4) {
        printf("[GAME] SELECT_CHARACTER: packet too short\n");
        return 0;
    }

    /* Parse character number */
    uint32_t charac_no;
    memcpy(&charac_no, data, 4);
    printf("[GAME] SELECT_CHARACTER: charac_no=%u\n", charac_no);

    /* Find character */
    const CharacterInfo* selected = NULL;
    for (int i = 0; i < g_mock_account.character_count; i++) {
        if (g_mock_account.characters[i].charac_no == charac_no) {
            selected = &g_mock_account.characters[i];
            break;
        }
    }

    if (!selected) {
        printf("[GAME] SELECT_CHARACTER: character not found\n");
        uint8_t error[4] = {4, 0, 0, 0};
        return send_response(s, 4, error, 4);
    }

    /* Update session */
    s->state = STATE_IN_GAME;
    s->character_id = charac_no;
    strncpy(s->character_name, selected->charac_name, sizeof(s->character_name) - 1);

    printf("[GAME] SELECT_CHARACTER: selected '%s' (job=%d, level=%d)\n",
           selected->charac_name, selected->job, selected->level);

    /* Build and send CHARACTER_STATUS */
    uint8_t response[128];
    int response_len = build_character_status(response, sizeof(response), selected);

    if (response_len > 0) {
        return send_response(s, 4, response, response_len);
    }

    return 0;
}

/* msg_no=5: CREATE_CHARACTER */
static int handle_create_character(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] CREATE_CHARACTER from fd=%d\n", s->fd);

    if (s->state < STATE_AUTHENTICATED) {
        uint8_t error[4] = {3, 0, 0, 0};  /* Not authenticated */
        return send_response(s, 5, error, 4);
    }

    if (len < 19) {  /* name(16) + job(1) + grow_type(1) + slot(1) */
        uint8_t error[4] = {1, 0, 0, 0};  /* Invalid packet */
        return send_response(s, 5, error, 4);
    }

    /* Parse data */
    char charac_name[MAX_CHARAC_NAME];
    memcpy(charac_name, data, MAX_CHARAC_NAME);
    charac_name[MAX_CHARAC_NAME - 1] = '\0';
    uint8_t job = data[16];
    uint8_t grow_type = data[17];
    uint8_t slot = data[18];

    printf("[GAME] CREATE_CHARACTER: name='%s' job=%d grow=%d slot=%d\n",
           charac_name, job, grow_type, slot);

    /* Validate */
    if (strlen(charac_name) < 2) {
        uint8_t error[4] = {5, 0, 0, 0};  /* Invalid name */
        return send_response(s, 5, error, 4);
    }

    if (job >= JOB_MAX) {
        uint8_t error[4] = {6, 0, 0, 0};  /* Invalid job */
        return send_response(s, 5, error, 4);
    }

    if (g_mock_account.character_count >= MAX_CHARACTERS) {
        uint8_t error[4] = {7, 0, 0, 0};  /* Too many characters */
        return send_response(s, 5, error, 4);
    }

    /* Create new character (mock) */
    static uint32_t next_charac_no = 100010;
    CharacterInfo new_char;
    memset(&new_char, 0, sizeof(new_char));
    new_char.charac_no = next_charac_no++;
    strncpy(new_char.charac_name, charac_name, MAX_CHARAC_NAME - 1);
    new_char.job = job;
    new_char.grow_type = grow_type;
    new_char.level = 1;
    new_char.hp = 1000;
    new_char.max_hp = 1000;
    new_char.mp = 500;
    new_char.max_mp = 500;
    new_char.village_id = 1;
    new_char.gold = 10000;
    new_char.fatigue = 156;
    new_char.slot = slot;
    new_char.deleted = 0;

    /* Add to account (mock) */
    int idx = g_mock_account.character_count;
    g_mock_account.characters[idx] = new_char;
    g_mock_account.character_count++;

    printf("[GAME] CREATE_CHARACTER: created charac_no=%u\n", new_char.charac_no);

    /* Response */
    uint8_t response[32];
    int offset = 0;
    response[offset++] = 0;  /* Success */
    memcpy(response + offset, &new_char.charac_no, 4);
    offset += 4;
    memset(response + offset, 0, MAX_CHARAC_NAME);
    strncpy((char*)(response + offset), new_char.charac_name, MAX_CHARAC_NAME - 1);
    offset += MAX_CHARAC_NAME;
    response[offset++] = new_char.job;
    response[offset++] = new_char.grow_type;
    response[offset++] = new_char.slot;

    return send_response(s, 5, response, offset);
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

    /* Find and mark deleted (mock) */
    for (int i = 0; i < g_mock_account.character_count; i++) {
        if (g_mock_account.characters[i].charac_no == charac_no) {
            g_mock_account.characters[i].deleted = 1;
            printf("[GAME] DELETE_CHARACTER: marked as deleted\n");
            break;
        }
    }

    uint8_t response[8];
    response[0] = 0;  /* Success */
    memcpy(response + 1, &charac_no, 4);
    return send_response(s, 6, response, 5);
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

    uint8_t response[512];
    int response_len = build_userinfo_packet(response, sizeof(response), &g_mock_account);

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
        return send_response(s, 15, error, 4);
    }

    /* Response: result(1) + village_id(4) + position data */
    uint8_t response[32];
    memset(response, 0, sizeof(response));
    response[0] = 0;  /* Success */
    uint32_t village_id = 1;  /* Default village */
    memcpy(response + 1, &village_id, 4);

    printf("[GAME] START_GAME: entering village %u\n", village_id);
    return send_response(s, 15, response, 16);
}

/* msg_no=16: SELECT_DUNGEON */
static int handle_select_dungeon(ClientSession* s, const uint8_t* data, int len) {
    printf("[GAME] SELECT_DUNGEON from fd=%d\n", s->fd);

    if (len < 8) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 16, error, 4);
    }

    uint32_t dungeon_id, difficulty;
    memcpy(&dungeon_id, data, 4);
    memcpy(&difficulty, data + 4, 4);

    printf("[GAME] SELECT_DUNGEON: dungeon=%u difficulty=%u\n", dungeon_id, difficulty);

    uint8_t response[16];
    memset(response, 0, sizeof(response));
    response[0] = 0;  /* Success */
    memcpy(response + 1, &dungeon_id, 4);

    return send_response(s, 16, response, 8);
}

/* msg_no=40: FINISH_LOADING */
static int handle_finish_loading(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] FINISH_LOADING from fd=%d\n", s->fd);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 40, response, 4);
}

/* msg_no=45: GIVEUP_GAME */
static int handle_giveup_game(ClientSession* s, const uint8_t* data, int len) {
    (void)data; (void)len;
    printf("[GAME] GIVEUP_GAME from fd=%d\n", s->fd);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 45, response, 4);
}

/* Default handler for unknown messages */
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

    /* Data after header */
    const uint8_t* data = packet + PACKET_HEADER_SIZE;
    int data_len = pkt_len - PACKET_HEADER_SIZE;

    printf("[GAME] Packet: class=%d len=%u msg=%d seq=%u data_len=%d\n",
           classification, length, msg_no, sequence, data_len);

    /* Update activity */
    s->last_activity = (uint32_t)time(NULL);

    /* Dispatch by message number */
    switch (msg_no) {
        case 0:  return handle_check_connection(s, data, data_len);
        case 1:  return handle_login(s, data, data_len);
        case 2:  return handle_set_udp(s, data, data_len);
        case 3:  return handle_exit(s, data, data_len);
        case 4:  return handle_select_character(s, data, data_len);
        case 5:  return handle_create_character(s, data, data_len);
        case 6:  return handle_delete_character(s, data, data_len);
        case 7:  return handle_return_select(s, data, data_len);
        case 8:  return handle_get_userinfo(s, data, data_len);
        case 15: return handle_start_game(s, data, data_len);
        case 16: return handle_select_dungeon(s, data, data_len);
        case 40: return handle_finish_loading(s, data, data_len);
        case 45: return handle_giveup_game(s, data, data_len);
        default: return handle_default(s, msg_no, data, data_len);
    }
}

/*==============================================================================
 * Client Handler
 *============================================================================*/

static int handle_client(ClientSession* s) {
    int n = recv(s->fd, s->recv_buffer + s->recv_len, BUFFER_SIZE - s->recv_len, 0);
    if (n <= 0) {
        if (n == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
            printf("[GAME] Client disconnected: fd=%d\n", s->fd);
            return -1;
        }
        return 0;
    }

    s->recv_len += n;
    printf("[GAME] Received %d bytes from fd=%d (total: %d)\n", n, s->fd, s->recv_len);

    /* Process complete packets */
    while (s->recv_len >= PACKET_HEADER_SIZE) {
        uint32_t pkt_len;
        memcpy(&pkt_len, s->recv_buffer + 1, 4);

        if (pkt_len > BUFFER_SIZE || pkt_len < PACKET_HEADER_SIZE) {
            printf("[GAME] Invalid packet length: %u, resetting buffer\n", pkt_len);
            s->recv_len = 0;
            break;
        }

        if ((int)pkt_len > s->recv_len) {
            /* Incomplete packet */
            break;
        }

        /* Handle complete packet */
        int result = handle_packet(s, s->recv_buffer, pkt_len);

        /* Remove processed packet */
        int remaining = s->recv_len - pkt_len;
        if (remaining > 0) {
            memmove(s->recv_buffer, s->recv_buffer + pkt_len, remaining);
        }
        s->recv_len = remaining;

        if (result < 0) {
            return -1;  /* Disconnect requested */
        }
    }

    return 0;
}

/*==============================================================================
 * Main
 *============================================================================*/

int main(int argc, char* argv[]) {
    int port = PORT;
    if (argc > 1) {
        port = atoi(argv[1]);
    }

    printf("=== DNF Game Server (Extended) ===\n");
    printf("Port: %d\n", port);
    printf("Max Sessions: %d\n", MAX_SESSIONS);
    printf("==================================\n");

    /* Initialize sessions */
    memset(g_sessions, 0, sizeof(g_sessions));

    /* Create listen socket */
    g_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_listen_fd < 0) {
        perror("socket");
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
        return 1;
    }

    if (listen(g_listen_fd, 128) < 0) {
        perror("listen");
        close(g_listen_fd);
        return 1;
    }

    printf("[GAME] Listening on 0.0.0.0:%d\n", port);

    /* Create epoll (use epoll_create for older kernels) */
    g_epoll_fd = epoll_create(MAX_EVENTS);
    if (g_epoll_fd < 0) {
        perror("epoll_create");
        close(g_listen_fd);
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
                        ev.events = EPOLLIN | EPOLLET;
                        ev.data.fd = client_fd;
                        epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);

                        printf("[GAME] New connection: fd=%d from %s:%d (sessions=%d)\n",
                               client_fd,
                               inet_ntoa(client_addr.sin_addr),
                               ntohs(client_addr.sin_port),
                               g_session_count);
                    } else {
                        printf("[GAME] Max sessions reached, rejecting connection\n");
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
    printf("[GAME] Server stopped\n");

    return 0;
}
