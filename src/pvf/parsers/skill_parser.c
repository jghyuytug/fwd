/**
 * @file skill_parser.c
 * @brief Skill data parser (.skl)
 */

#include "pvf/parsers/skill_parser.h"
#include "pvf/pvf.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void Skill_Init(SkillData* skl) {
    if (skl == NULL) return;
    memset(skl, 0, sizeof(*skl));
    skl->raw_data.entries = NULL;
    skl->raw_data.count = 0;
    skl->raw_data.capacity = 0;
}

int Skill_Parse(SkillData* skl, const void* data, size_t size) {
    if (skl == NULL || data == NULL) {
        return PVF_ERROR_INVALID_PARAMETER;
    }

    Skill_Init(skl);

    int ret = PVFDataContainer_Parse(&skl->raw_data, data, size);
    if (ret != PVF_SUCCESS) {
        return ret;
    }

    PVFDataEntry* entry;

    skl->skill_id = PVFDataContainer_GetInt(&skl->raw_data, SKL_KEY_SKILL_ID, 0);

    entry = PVFDataContainer_Find(&skl->raw_data, SKL_KEY_NAME_REF);
    if (entry && entry->type == PVF_VALUE_TYPE_REFERENCE) {
        skl->name_ref = entry->value.ref_value;
    }

    skl->skill_type = PVFDataContainer_GetInt(&skl->raw_data, SKL_KEY_SKILL_TYPE, 0);
    skl->max_level = PVFDataContainer_GetInt(&skl->raw_data, SKL_KEY_MAX_LEVEL, 0);
    skl->required_level = PVFDataContainer_GetInt(&skl->raw_data, SKL_KEY_REQUIRED_LEVEL, 0);
    skl->damage_type = PVFDataContainer_GetInt(&skl->raw_data, SKL_KEY_DAMAGE_TYPE, 0);
    skl->base_damage = PVFDataContainer_GetInt(&skl->raw_data, SKL_KEY_BASE_DAMAGE, 0);
    skl->mp_cost = PVFDataContainer_GetInt(&skl->raw_data, SKL_KEY_MP_COST, 0);
    skl->cooldown = PVFDataContainer_GetInt(&skl->raw_data, SKL_KEY_COOLDOWN, 0);
    skl->cast_time = PVFDataContainer_GetInt(&skl->raw_data, SKL_KEY_CAST_TIME, 0);
    skl->hit_count = PVFDataContainer_GetInt(&skl->raw_data, SKL_KEY_HIT_COUNT, 0);
    skl->crit_rate = PVFDataContainer_GetInt(&skl->raw_data, SKL_KEY_CRIT_RATE, 0);

    return PVF_SUCCESS;
}

int Skill_ParseFromPVF(SkillData* skl, struct PackSetInternal* pack, const char* filepath) {
    if (skl == NULL || pack == NULL || filepath == NULL) {
        return PVF_ERROR_INVALID_PARAMETER;
    }

    PVFIndexEntry* entry = PackSet_GetFile(pack, filepath);
    if (entry == NULL) {
        return PVF_ERROR_FILE_NOT_FOUND;
    }

    unsigned char* buffer = (unsigned char*)malloc(entry->size);
    if (buffer == NULL) {
        return PVF_ERROR_MEMORY;
    }

    int ret = PackSet_ExtractFile(pack, entry, buffer, entry->size);
    if (ret != PVF_SUCCESS) {
        free(buffer);
        return ret;
    }

    ret = Skill_Parse(skl, buffer, entry->size);
    free(buffer);
    return ret;
}

void Skill_Free(SkillData* skl) {
    if (skl == NULL) return;
    PVFDataContainer_Free(&skl->raw_data);
    memset(skl, 0, sizeof(*skl));
}

void Skill_Print(const SkillData* skl) {
    if (skl == NULL) return;

    printf("========================================\n");
    printf("Skill (.skl)\n");
    printf("========================================\n");
    printf("Skill ID:      %d\n", skl->skill_id);
    printf("Name Ref:      0x%08x\n", skl->name_ref);
    printf("Skill Type:    %d\n", skl->skill_type);
    printf("Max Level:     %d\n", skl->max_level);
    printf("Req Level:     %d\n", skl->required_level);
    printf("Damage Type:   %d\n", skl->damage_type);
    printf("Base Damage:   %d\n", skl->base_damage);
    printf("MP Cost:       %d\n", skl->mp_cost);
    printf("Cooldown:      %d\n", skl->cooldown);
    printf("Cast Time:     %d\n", skl->cast_time);
    printf("Hit Count:     %d\n", skl->hit_count);
    printf("Crit Rate:     %d\n", skl->crit_rate);
    printf("Raw Entries:   %d\n", skl->raw_data.count);
    printf("========================================\n");
}
