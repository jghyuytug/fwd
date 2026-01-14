#include "db/db_skills.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <zlib.h>

#include "database/headers/db_connection_manager.h"
#include "game_server_context.h"
#include "orm/extended_features/taiwan_cain_2nd/headers/skill_orm.h"

static int parse_skill_slot_blob(const unsigned char* blob, unsigned long blob_len,
                                 DBSkillInfo* skill) {
    if (!blob || blob_len < 8 || !skill) {
        return -1;
    }

    uint32_t uncompressed_size = blob[0] | (blob[1] << 8) | (blob[2] << 16) | (blob[3] << 24);
    if (uncompressed_size > 1024) {
        std::printf("[DB][SKILL] Skill blob too large: %u\n", uncompressed_size);
        return -1;
    }

    unsigned char decompressed[1024];
    uLongf dest_len = sizeof(decompressed);
    int ret = uncompress(decompressed, &dest_len, blob + 4, blob_len - 4);
    if (ret != Z_OK) {
        std::printf("[DB][SKILL] Skill blob decompress failed: %d\n", ret);
        return -1;
    }

    skill->skill_count = 0;
    for (uLongf i = 0; i + 1 < dest_len && skill->skill_count < MAX_PARSED_SKILLS; i += 2) {
        uint16_t skill_id = decompressed[i] | (decompressed[i + 1] << 8);
        if (skill_id > 0) {
            skill->skill_ids[skill->skill_count++] = skill_id;
        }
    }
    return 0;
}

static void load_skill_slot(uint32_t charac_no, DBSkillInfo* skill) {
    if (!skill) return;

    char query[256];
    std::snprintf(query, sizeof(query),
                  "SELECT skill_slot FROM skill WHERE charac_no=%u", charac_no);

    DBQueryResult result;
    std::memset(&result, 0, sizeof(result));

    if (DBConnectionManager_ExecuteQuery(&g_db_manager, DB_TYPE_CAIN, query, &result) < 0) {
        std::printf("[DB][SKILL] Failed to query skill_slot for charac_no=%u\n", charac_no);
        return;
    }

    char* values[1] = {0};
    unsigned long* lengths = NULL;
    if (DBQueryResult_FetchRowWithLengths(&result, values, &lengths) > 0) {
        if (values[0] && lengths && lengths[0] > 4) {
            parse_skill_slot_blob(reinterpret_cast<const unsigned char*>(values[0]),
                                  lengths[0], skill);
        }
    }

    DBQueryResult_Free(&result);
}

int db_get_skill(uint32_t charac_no, DBSkillInfo* skill) {
    if (!skill) {
        return -1;
    }

    std::memset(skill, 0, sizeof(DBSkillInfo));
    skill->charac_no = charac_no;

    Skill orm_skill;
    std::memset(&orm_skill, 0, sizeof(orm_skill));
    if (Skill_Get(&g_db_manager, static_cast<int>(charac_no), &orm_skill) < 0) {
        std::printf("[DB][SKILL] Skill_Get failed for charac_no=%u\n", charac_no);
        return -1;
    }

    skill->remain_sp = orm_skill.remain_sp > 0 ? static_cast<uint32_t>(orm_skill.remain_sp) : 0;
    skill->used_sp = orm_skill.used_sp > 0 ? static_cast<uint32_t>(orm_skill.used_sp) : 0;
    skill->remain_sp_2nd =
        orm_skill.remain_sp_2nd > 0 ? static_cast<uint32_t>(orm_skill.remain_sp_2nd) : 0;

    load_skill_slot(charac_no, skill);

    skill->skill_loaded = 1;
    std::printf("[DB] Loaded skill for charac_no=%u: remain_sp=%u, used_sp=%u, skills=%u\n",
                charac_no, skill->remain_sp, skill->used_sp, skill->skill_count);

    if (skill->skill_count > 0) {
        std::printf("[DB] Skill IDs:");
        for (uint8_t i = 0; i < skill->skill_count && i < 10; i++) {
            std::printf(" %u", skill->skill_ids[i]);
        }
        if (skill->skill_count > 10) {
            std::printf(" ...");
        }
        std::printf("\n");
    }

    return 0;
}
