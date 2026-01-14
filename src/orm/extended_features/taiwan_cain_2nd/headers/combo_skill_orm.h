#ifndef COMBO_SKILL_ORM_H
#define COMBO_SKILL_ORM_H

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
} ComboSkill;

/* CRUD Operations */
int ComboSkill_Add(DBConnectionManager* manager, const ComboSkill* record);
int ComboSkill_Get(DBConnectionManager* manager, int charac_no, int combo_idx, ComboSkill* record);
int ComboSkill_Update(DBConnectionManager* manager, const ComboSkill* record);
int ComboSkill_Delete(DBConnectionManager* manager, int charac_no, int combo_idx);
int ComboSkill_Exists(DBConnectionManager* manager, int charac_no, int combo_idx);
int ComboSkill_GetAll(DBConnectionManager* manager, ComboSkill* records, int max_count, int* actual_count);

#endif /* COMBO_SKILL_ORM_H */