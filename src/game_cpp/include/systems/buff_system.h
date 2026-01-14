#ifndef SYSTEMS_BUFF_SYSTEM_H
#define SYSTEMS_BUFF_SYSTEM_H

#include <stdint.h>
#include "game_server_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int buff_add(ClientSession* s, uint16_t buff_id, uint32_t source_id,
             int32_t value, uint32_t duration_ms);
int buff_remove(ClientSession* s, uint16_t buff_id);
int buff_has(ClientSession* s, uint16_t buff_id);
int32_t buff_get_stat_modifier(ClientSession* s, BuffType stat_buff);
void buff_update(ClientSession* s);
void buff_process_dots_and_hots(ClientSession* s, uint32_t delta_ms);
void buff_process_dots(ClientSession* s, uint32_t delta_ms);
void buff_clear_all(ClientSession* s);
int buff_remove_debuffs(ClientSession* s);
void buff_apply_stat_modifiers(ClientSession* s);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEMS_BUFF_SYSTEM_H */
