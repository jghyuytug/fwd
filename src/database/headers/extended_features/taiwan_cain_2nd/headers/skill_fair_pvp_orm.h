#ifndef SKILL_FAIR_PVP_ORM_H
#define SKILL_FAIR_PVP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int charac_no;
    int remain_sp;
    int sp_garbage;
    int used_sp;
    signed char lethe_flag;
    int remain_sp_2nd;
    signed char lethe_flag_2nd;
    short remain_sfp_1st;
    short remain_sfp_2nd;
    signed char script_version;
} SkillFairPvp;

/* CRUD Operations */
int SkillFairPvp_Add(DBConnectionManager* manager, const SkillFairPvp* record);
int SkillFairPvp_Get(DBConnectionManager* manager, int charac_no, SkillFairPvp* record);
int SkillFairPvp_Update(DBConnectionManager* manager, const SkillFairPvp* record);
int SkillFairPvp_Delete(DBConnectionManager* manager, int charac_no);
int SkillFairPvp_Exists(DBConnectionManager* manager, int charac_no);
int SkillFairPvp_GetAll(DBConnectionManager* manager, SkillFairPvp* records, int max_count, int* actual_count);

#endif /* SKILL_FAIR_PVP_ORM_H */