#ifndef KNOWCAFE_PLAN_CONTENT_ORM_H
#define KNOWCAFE_PLAN_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char skill[1024];
    char content[1024];
    char content_type;
} KnowcafePlanContent;

/* CRUD Operations */
int KnowcafePlanContent_Add(DBConnectionManager* manager, const KnowcafePlanContent* record);
int KnowcafePlanContent_Get(DBConnectionManager* manager, int no, KnowcafePlanContent* record);
int KnowcafePlanContent_Update(DBConnectionManager* manager, const KnowcafePlanContent* record);
int KnowcafePlanContent_Delete(DBConnectionManager* manager, int no);
int KnowcafePlanContent_Exists(DBConnectionManager* manager, int no);
int KnowcafePlanContent_GetAll(DBConnectionManager* manager, KnowcafePlanContent* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_PLAN_CONTENT_ORM_H */