#ifndef SKILL_LETHE_ORM_H
#define SKILL_LETHE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int charac_no;
    signed char flag;
} SkillLethe;

/* CRUD Operations */
int SkillLethe_Add(DBConnectionManager* manager, const SkillLethe* record);
int SkillLethe_Get(DBConnectionManager* manager, int m_id, int charac_no, SkillLethe* record);
int SkillLethe_Update(DBConnectionManager* manager, const SkillLethe* record);
int SkillLethe_Delete(DBConnectionManager* manager, int m_id, int charac_no);
int SkillLethe_Exists(DBConnectionManager* manager, int m_id, int charac_no);
int SkillLethe_GetAll(DBConnectionManager* manager, SkillLethe* records, int max_count, int* actual_count);

#endif /* SKILL_LETHE_ORM_H */