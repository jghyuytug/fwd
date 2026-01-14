#ifndef KNOWL_CLASS_ORM_H
#define KNOWL_CLASS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int kn_id;
    int sup_id;
    char kn_name[51];
    int cur_depth;
} KnowlClass;

/* CRUD Operations */
int KnowlClass_Add(DBConnectionManager* manager, const KnowlClass* record);
int KnowlClass_Get(DBConnectionManager* manager, int kn_id, KnowlClass* record);
int KnowlClass_Update(DBConnectionManager* manager, const KnowlClass* record);
int KnowlClass_Delete(DBConnectionManager* manager, int kn_id);
int KnowlClass_Exists(DBConnectionManager* manager, int kn_id);
int KnowlClass_GetAll(DBConnectionManager* manager, KnowlClass* records, int max_count, int* actual_count);

#endif /* KNOWL_CLASS_ORM_H */