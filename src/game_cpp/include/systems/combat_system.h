#ifndef SYSTEMS_COMBAT_SYSTEM_H
#define SYSTEMS_COMBAT_SYSTEM_H

#include <stdint.h>
#include "game_server_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void calculate_combat_stats(ClientSession* s);
uint32_t calculate_skill_damage_v3(ClientSession* s, uint16_t skill_id,
                                   uint8_t skill_level, uint16_t monster_type,
                                   uint8_t monster_level, uint32_t monster_def,
                                   uint8_t* is_crit, uint8_t* is_miss);
uint32_t process_shield(ClientSession* s, uint32_t incoming_damage);
uint8_t get_skill_hit_count(uint16_t skill_id);
uint16_t get_skill_mp_cost(uint16_t skill_id, uint8_t skill_level);
uint16_t get_skill_cooldown(uint16_t skill_id);
int skill_is_on_cooldown(ClientSession* s, uint16_t skill_id);
uint32_t skill_get_cooldown_remaining(ClientSession* s, uint16_t skill_id);
void skill_start_cooldown(ClientSession* s, uint16_t skill_id);
void skill_reset_cooldown(ClientSession* s, uint16_t skill_id);
void skill_reset_all_cooldowns(ClientSession* s);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEMS_COMBAT_SYSTEM_H */
