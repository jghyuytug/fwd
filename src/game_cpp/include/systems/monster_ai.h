#ifndef SYSTEMS_MONSTER_AI_H
#define SYSTEMS_MONSTER_AI_H

#include <stdint.h>
#include "game_server_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t damage;
    uint16_t aoe_radius;
    uint8_t  debuff_type;
    uint16_t debuff_duration;
    int16_t  debuff_value;
    uint8_t  skill_used;
} MonsterAttackResult;

int monster_update_ai(MonsterInfo* m, uint16_t player_x, uint16_t player_y,
                      uint32_t player_charac_no, uint32_t current_time);
MonsterAttackResult monster_update_ai_v2(MonsterInfo* m, uint16_t player_x,
                                         uint16_t player_y, uint32_t player_charac_no,
                                         uint32_t current_time);
void monster_apply_hit_stun(MonsterInfo* m, uint8_t stun_frames);
void boss_init_from_template(MonsterInfo* m, uint32_t boss_id, uint8_t difficulty);

/* Phase 16 P2: Multi-target aggro system */
void monster_add_aggro(MonsterInfo* m, uint32_t player_id, int32_t damage);
uint32_t monster_get_highest_aggro_target(MonsterInfo* m);
void monster_decay_aggro(MonsterInfo* m, int decay_percent);
void monster_clear_aggro(MonsterInfo* m);
void monster_update_target_from_aggro(MonsterInfo* m);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEMS_MONSTER_AI_H */
