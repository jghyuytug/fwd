#ifndef EXP_LEVEL_REF_ORM_H
#define EXP_LEVEL_REF_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int exp;
    int lev;
} ExpLevelRef;

/* CRUD Operations */
int ExpLevelRef_Add(DBConnectionManager* manager, const ExpLevelRef* record);
int ExpLevelRef_Get(DBConnectionManager* manager, int exp, ExpLevelRef* record);
int ExpLevelRef_Update(DBConnectionManager* manager, const ExpLevelRef* record);
int ExpLevelRef_Delete(DBConnectionManager* manager, int exp);
int ExpLevelRef_Exists(DBConnectionManager* manager, int exp);
int ExpLevelRef_GetAll(DBConnectionManager* manager, ExpLevelRef* records, int max_count, int* actual_count);

#endif /* EXP_LEVEL_REF_ORM_H */