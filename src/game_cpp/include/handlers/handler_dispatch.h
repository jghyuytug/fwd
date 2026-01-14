#ifndef HANDLER_DISPATCH_H
#define HANDLER_DISPATCH_H

#include <stdint.h>
#include "game_server_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int dispatch_message(ClientSession* s, uint16_t msg_no, const uint8_t* data, int len);

#ifdef __cplusplus
}
#endif

#endif /* HANDLER_DISPATCH_H */
