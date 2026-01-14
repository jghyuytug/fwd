#ifndef SKILL_INDEX_ORM_H
#define SKILL_INDEX_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int job;
    int skill_idx;
    char skill_name[31];
} SkillIndex;

/* CRUD Operations */
int SkillIndex_Add(DBConnectionManager* manager, const SkillIndex* record);
int SkillIndex_Get(DBConnectionManager* manager, int no, SkillIndex* record);
int SkillIndex_Update(DBConnectionManager* manager, const SkillIndex* record);
int SkillIndex_Delete(DBConnectionManager* manager, int no);
int SkillIndex_Exists(DBConnectionManager* manager, int no);
int SkillIndex_GetAll(DBConnectionManager* manager, SkillIndex* records, int max_count, int* actual_count);

#endif /* SKILL_INDEX_ORM_H */