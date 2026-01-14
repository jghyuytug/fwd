#ifndef GAME_PACKET_UTILS_H
#define GAME_PACKET_UTILS_H

#include <stdint.h>
#include "game_server_types.h"

#ifndef PACKET_HEADER_SIZE
#define PACKET_HEADER_SIZE 13
#endif

#ifdef __cplusplus
extern "C" {
#endif

int send_response(ClientSession* s, uint16_t msg_no, const uint8_t* data, int data_len);
int build_userinfo_packet(uint8_t* buffer, int max_len, ClientSession* s);
int build_character_status(uint8_t* buffer, int max_len, const DBCharacterInfo* ch);

#ifdef __cplusplus
}
#endif

#endif /* GAME_PACKET_UTILS_H */
