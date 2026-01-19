#ifndef PVF_SKILL_PARSER_H
#define PVF_SKILL_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file skill_parser.h
 * @brief Skill data parser for PVF .skl files
 */

#include <stddef.h>
#include <stdint.h>

#include "pvf/pvf_data_parser.h"
#include "pvf/pvf_types.h" /* PackSetInternal */

/* Common .skl keys (verified by prototype mapping) */
#define SKL_KEY_SKILL_ID         0x0000227D
#define SKL_KEY_NAME_REF         0x00002A12
#define SKL_KEY_SKILL_TYPE       0x00002282
#define SKL_KEY_MAX_LEVEL        0x00002283
#define SKL_KEY_REQUIRED_LEVEL   0x00002281
#define SKL_KEY_DAMAGE_TYPE      0x00002285
#define SKL_KEY_BASE_DAMAGE      0x00002280
#define SKL_KEY_MP_COST          0x000022A0
#define SKL_KEY_COOLDOWN         0x0000427E
#define SKL_KEY_CAST_TIME        0x0000227F
#define SKL_KEY_HIT_COUNT        0x0000228A
#define SKL_KEY_CRIT_RATE        0x00002290

typedef struct {
    int32_t skill_id;
    uint32_t name_ref;
    int32_t skill_type;
    int32_t max_level;
    int32_t required_level;
    int32_t damage_type;
    int32_t base_damage;
    int32_t mp_cost;
    int32_t cooldown;
    int32_t cast_time;
    int32_t hit_count;
    int32_t crit_rate;

    PVFDataContainer raw_data;
} SkillData;

void Skill_Init(SkillData* skl);
int Skill_Parse(SkillData* skl, const void* data, size_t size);
int Skill_ParseFromPVF(SkillData* skl, struct PackSetInternal* pack, const char* filepath);
void Skill_Free(SkillData* skl);
void Skill_Print(const SkillData* skl);

#ifdef __cplusplus
}
#endif

#endif /* PVF_SKILL_PARSER_H */
