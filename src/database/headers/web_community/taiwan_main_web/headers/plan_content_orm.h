#ifndef PLAN_CONTENT_ORM_H
#define PLAN_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char skill[1024];
    char content[1024];
    char content_type;
} PlanContent;

/* CRUD Operations */
int PlanContent_Add(DBConnectionManager* manager, const PlanContent* record);
int PlanContent_Get(DBConnectionManager* manager, int no, PlanContent* record);
int PlanContent_Update(DBConnectionManager* manager, const PlanContent* record);
int PlanContent_Delete(DBConnectionManager* manager, int no);
int PlanContent_Exists(DBConnectionManager* manager, int no);
int PlanContent_GetAll(DBConnectionManager* manager, PlanContent* records, int max_count, int* actual_count);

#endif /* PLAN_CONTENT_ORM_H */