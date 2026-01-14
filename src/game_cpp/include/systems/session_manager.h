#ifndef SYSTEMS_SESSION_MANAGER_H
#define SYSTEMS_SESSION_MANAGER_H

#include <stdint.h>
#include "game_server_types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    CHAT_TYPE_NORMAL   = 0,
    CHAT_TYPE_WHISPER  = 1,
    CHAT_TYPE_PARTY    = 2,
    CHAT_TYPE_GUILD    = 3,
    CHAT_TYPE_WORLD    = 4,
    CHAT_TYPE_SYSTEM   = 5
};

extern ClientSession g_sessions[MAX_SESSIONS];
extern int g_session_count;

int is_character_online(uint32_t charac_no);
ClientSession* find_session(int fd);
ClientSession* find_session_by_charac(uint32_t charac_no);
ClientSession* find_session_by_name(const char* name);
ClientSession* create_session(int fd, uint32_t ip, uint16_t port);
void destroy_session(ClientSession* s);
void save_character_on_logout(ClientSession* s);
void broadcast_chat(ClientSession* sender, uint8_t chat_type,
                    const char* message, int msg_len, uint32_t area_filter);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEMS_SESSION_MANAGER_H */
