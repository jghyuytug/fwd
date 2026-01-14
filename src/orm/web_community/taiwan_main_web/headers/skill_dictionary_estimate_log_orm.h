#ifndef SKILL_DICTIONARY_ESTIMATE_LOG_ORM_H
#define SKILL_DICTIONARY_ESTIMATE_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
    char m_user_id[17];
    int charac_idx;
    int skill_group;
    int skill_idx;
    int dungeon_cnt;
    int challenge_cnt;
    int create_day;
} SkillDictionaryEstimateLog;

/* CRUD Operations */
int SkillDictionaryEstimateLog_Add(DBConnectionManager* manager, const SkillDictionaryEstimateLog* record);
int SkillDictionaryEstimateLog_Get(DBConnectionManager* manager, int no, SkillDictionaryEstimateLog* record);
int SkillDictionaryEstimateLog_Update(DBConnectionManager* manager, const SkillDictionaryEstimateLog* record);
int SkillDictionaryEstimateLog_Delete(DBConnectionManager* manager, int no);
int SkillDictionaryEstimateLog_Exists(DBConnectionManager* manager, int no);
int SkillDictionaryEstimateLog_GetAll(DBConnectionManager* manager, SkillDictionaryEstimateLog* records, int max_count, int* actual_count);

#endif /* SKILL_DICTIONARY_ESTIMATE_LOG_ORM_H */