#include "systems/session_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <unistd.h>

#include "db/db_game.h"
#include "game_packet_utils.h"
#include "game_server_context.h"

#ifndef PACKET_HEADER_SIZE
#define PACKET_HEADER_SIZE 13
#endif

ClientSession g_sessions[MAX_SESSIONS];
int g_session_count = 0;

extern int db_update_character_hp_mp(uint32_t charac_no, uint16_t hp, uint16_t mp);

int is_character_online(uint32_t charac_no) {
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active &&
            g_sessions[i].state == STATE_IN_GAME &&
            g_sessions[i].character_id == charac_no) {
            return 1;
        }
    }
    return 0;
}

ClientSession* find_session_by_charac(uint32_t charac_no) {
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active &&
            g_sessions[i].state == STATE_IN_GAME &&
            g_sessions[i].character_id == charac_no) {
            return &g_sessions[i];
        }
    }
    return NULL;
}

ClientSession* find_session(int fd) {
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].active && g_sessions[i].fd == fd) {
            return &g_sessions[i];
        }
    }
    return NULL;
}

ClientSession* create_session(int fd, uint32_t ip, uint16_t port) {
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

void save_character_on_logout(ClientSession* s) {
    if (!s || s->character_id == 0) return;

    printf("[GAME] Saving character %s (id=%u) on logout\n",
           s->character_name, s->character_id);

    db_update_gold(s->character_id, s->inventory.money);
    db_update_character_hp_mp(s->character_id, (uint16_t)s->current_hp, 0);

    printf("[GAME] Character saved: gold=%u hp=%u\n",
           s->inventory.money, s->current_hp);
}

void destroy_session(ClientSession* s) {
    if (s && s->active) {
        if (s->guild_id > 0 && s->character_id > 0) {
            guild_ipc_notify_online(s->guild_id, s->character_id, 0);
        }

        save_character_on_logout(s);

        s->active = 0;
        g_session_count--;
        printf("[GAME] Session destroyed for %s (id=%u)\n",
               s->character_name, s->character_id);
    }
}

int send_response(ClientSession* s, uint16_t msg_no, const uint8_t* data, int data_len) {
    int total_len = PACKET_HEADER_SIZE + data_len;
    uint8_t* packet = (uint8_t*)malloc(total_len);
    if (!packet) return -1;

    memset(packet, 0, total_len);

    packet[0] = 0;
    memcpy(packet + 1, &total_len, 4);
    memcpy(packet + 5, &msg_no, 2);
    packet[6] = 0;
    uint32_t checksum = 0;
    memcpy(packet + 7, &checksum, 4);
    uint16_t seq = ++g_server_state.sequence;
    memcpy(packet + 11, &seq, 2);

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

int build_userinfo_packet(uint8_t* buffer, int max_len, ClientSession* s) {
    if (!buffer || !s || max_len < 512) return -1;

    int offset = 0;
    uint32_t result = 0;
    memcpy(buffer + offset, &result, 4);
    offset += 4;

    uint32_t char_count = (uint32_t)s->character_count;
    memcpy(buffer + offset, &char_count, 4);
    offset += 4;

    for (int i = 0; i < s->character_count; i++) {
        const DBCharacterInfo* ch = &s->characters[i];
        int needed = 120;
        if (offset + needed > max_len) {
            return -1;
        }

        memcpy(buffer + offset, ch, sizeof(DBCharacterInfo));
        offset += sizeof(DBCharacterInfo);
    }

    memcpy(buffer + offset, &s->account_id, 4);
    offset += 4;
    memcpy(buffer + offset, &s->fatigue, 4);
    offset += 4;
    memcpy(buffer + offset, &s->vip_level, 1);
    offset += 1;

    return offset;
}

ClientSession* find_session_by_name(const char* name) {
    if (!name) return NULL;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (g_sessions[i].fd > 0 && g_sessions[i].state >= STATE_IN_GAME) {
            if (strncmp(g_sessions[i].character_name, name, 20) == 0) {
                return &g_sessions[i];
            }
        }
    }
    return NULL;
}

void broadcast_chat(ClientSession* sender, uint8_t chat_type,
                    const char* message, int msg_len, uint32_t area_filter) {
    uint8_t response[300];
    memset(response, 0, sizeof(response));
    int offset = 0;

    response[offset++] = 0;
    response[offset++] = chat_type;

    memset(response + offset, 0, 20);
    strncpy((char*)(response + offset), sender->character_name, 19);
    offset += 20;

    response[offset++] = (uint8_t)msg_len;
    if (msg_len > 0 && message) {
        memcpy(response + offset, message, msg_len);
        offset += msg_len;
    }

    for (int i = 0; i < MAX_SESSIONS; i++) {
        ClientSession* target = &g_sessions[i];
        if (target->fd <= 0 || target->state < STATE_IN_GAME) continue;

        int should_send = 0;

        switch (chat_type) {
            case CHAT_TYPE_WORLD:
                should_send = 1;
                break;
            case CHAT_TYPE_NORMAL:
                if (area_filter == 0 || target->current_village == area_filter) {
                    should_send = 1;
                }
                break;
            case CHAT_TYPE_GUILD:
                if (sender->guild_id > 0 && target->guild_id == sender->guild_id) {
                    should_send = 1;
                }
                break;
            case CHAT_TYPE_PARTY:
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
