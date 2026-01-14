#ifndef PLAN_CNT_ORM_H
#define PLAN_CNT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char category;
    signed char sub_job;
    int cnt;
} PlanCnt;

/* CRUD Operations */
int PlanCnt_Add(DBConnectionManager* manager, const PlanCnt* record);
int PlanCnt_Get(DBConnectionManager* manager, signed char category, signed char sub_job, PlanCnt* record);
int PlanCnt_Update(DBConnectionManager* manager, const PlanCnt* record);
int PlanCnt_Delete(DBConnectionManager* manager, signed char category, signed char sub_job);
int PlanCnt_Exists(DBConnectionManager* manager, signed char category, signed char sub_job);
int PlanCnt_GetAll(DBConnectionManager* manager, PlanCnt* records, int max_count, int* actual_count);

#endif /* PLAN_CNT_ORM_H */