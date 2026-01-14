#ifndef SKILL_ORM_H
#define SKILL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int charac_no;
    int remain_sp;
    int sp_garbage;
    int used_sp;
    int remain_sp_2nd;
    signed char lethe_flag;
    signed char lethe_flag_2nd;
    int remain_sfp_2nd;
    int remain_sfp_1st;
    signed char script_version;
} Skill;

/* CRUD Operations */
int Skill_Add(DBConnectionManager* manager, const Skill* record);
int Skill_Get(DBConnectionManager* manager, int charac_no, Skill* record);
int Skill_Update(DBConnectionManager* manager, const Skill* record);
int Skill_Delete(DBConnectionManager* manager, int charac_no);
int Skill_Exists(DBConnectionManager* manager, int charac_no);
int Skill_GetAll(DBConnectionManager* manager, Skill* records, int max_count, int* actual_count);

#endif /* SKILL_ORM_H */