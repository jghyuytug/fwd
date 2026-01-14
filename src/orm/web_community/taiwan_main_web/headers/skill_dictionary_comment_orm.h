#ifndef SKILL_DICTIONARY_COMMENT_ORM_H
#define SKILL_DICTIONARY_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char m_nickname[13];
    int m_id;
    char m_user_id[17];
    char m_sex;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    char comment_text[256];
    char ip[16];
    int create_day;
    int skill_group;
    int skill_idx;
    int charac_idx;
} SkillDictionaryComment;

/* CRUD Operations */
int SkillDictionaryComment_Add(DBConnectionManager* manager, const SkillDictionaryComment* record);
int SkillDictionaryComment_Get(DBConnectionManager* manager, int no, SkillDictionaryComment* record);
int SkillDictionaryComment_Update(DBConnectionManager* manager, const SkillDictionaryComment* record);
int SkillDictionaryComment_Delete(DBConnectionManager* manager, int no);
int SkillDictionaryComment_Exists(DBConnectionManager* manager, int no);
int SkillDictionaryComment_GetAll(DBConnectionManager* manager, SkillDictionaryComment* records, int max_count, int* actual_count);

#endif /* SKILL_DICTIONARY_COMMENT_ORM_H */