#ifndef SKILL_DICTIONARY_INFO_ORM_H
#define SKILL_DICTIONARY_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int charac_idx;
    int skill_group;
    int skill_idx;
    int dungeon_cnt;
    int challenge_cnt;
    int comment_cnt;
    int estimate_cnt;
} SkillDictionaryInfo;

/* CRUD Operations */
int SkillDictionaryInfo_Add(DBConnectionManager* manager, const SkillDictionaryInfo* record);
int SkillDictionaryInfo_Get(DBConnectionManager* manager, int no, SkillDictionaryInfo* record);
int SkillDictionaryInfo_Update(DBConnectionManager* manager, const SkillDictionaryInfo* record);
int SkillDictionaryInfo_Delete(DBConnectionManager* manager, int no);
int SkillDictionaryInfo_Exists(DBConnectionManager* manager, int no);
int SkillDictionaryInfo_GetAll(DBConnectionManager* manager, SkillDictionaryInfo* records, int max_count, int* actual_count);

#endif /* SKILL_DICTIONARY_INFO_ORM_H */