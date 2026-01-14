#ifndef HANDLER_LOGIN_H
#define HANDLER_LOGIN_H

#include <stdint.h>
#include "game_server_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int handle_check_connection(ClientSession* s, const uint8_t* data, int len);
int handle_login(ClientSession* s, const uint8_t* data, int len);
int handle_set_udp(ClientSession* s, const uint8_t* data, int len);
int handle_exit(ClientSession* s, const uint8_t* data, int len);
int handle_select_character(ClientSession* s, const uint8_t* data, int len);
int handle_create_character(ClientSession* s, const uint8_t* data, int len);
int handle_delete_character(ClientSession* s, const uint8_t* data, int len);
int handle_return_select(ClientSession* s, const uint8_t* data, int len);
int handle_get_userinfo(ClientSession* s, const uint8_t* data, int len);

#ifdef __cplusplus
}
#endif

#endif /* HANDLER_LOGIN_H */
