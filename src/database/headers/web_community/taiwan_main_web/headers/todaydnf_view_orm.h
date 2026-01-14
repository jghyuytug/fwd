#ifndef TODAYDNF_VIEW_ORM_H
#define TODAYDNF_VIEW_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char type;
    int uv;
    int pv;
} TodaydnfView;

/* CRUD Operations */
int TodaydnfView_Add(DBConnectionManager* manager, const TodaydnfView* record);
int TodaydnfView_Get(DBConnectionManager* manager, int no, signed char type, TodaydnfView* record);
int TodaydnfView_Update(DBConnectionManager* manager, const TodaydnfView* record);
int TodaydnfView_Delete(DBConnectionManager* manager, int no, signed char type);
int TodaydnfView_Exists(DBConnectionManager* manager, int no, signed char type);
int TodaydnfView_GetAll(DBConnectionManager* manager, TodaydnfView* records, int max_count, int* actual_count);

#endif /* TODAYDNF_VIEW_ORM_H */