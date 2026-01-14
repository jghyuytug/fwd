#ifndef COMBO_SKILL_2ND_ORM_H
#define COMBO_SKILL_2ND_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int charac_no;
    int combo_idx;
    short value1;
    short value2;
    short value3;
    short value4;
    short value5;
    short value6;
} ComboSkill2Nd;

/* CRUD Operations */
int ComboSkill2Nd_Add(DBConnectionManager* manager, const ComboSkill2Nd* record);
int ComboSkill2Nd_Get(DBConnectionManager* manager, int charac_no, int combo_idx, ComboSkill2Nd* record);
int ComboSkill2Nd_Update(DBConnectionManager* manager, const ComboSkill2Nd* record);
int ComboSkill2Nd_Delete(DBConnectionManager* manager, int charac_no, int combo_idx);
int ComboSkill2Nd_Exists(DBConnectionManager* manager, int charac_no, int combo_idx);
int ComboSkill2Nd_GetAll(DBConnectionManager* manager, ComboSkill2Nd* records, int max_count, int* actual_count);

#endif /* COMBO_SKILL_2ND_ORM_H */