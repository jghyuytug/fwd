#ifndef CHARAC_EVENT_ITEMS_ORM_H
#define CHARAC_EVENT_ITEMS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    int charac_no;
    int it_id;
    int event_code;
    char reg_time[20];
    char delete_time[20];
    signed char delete_flag;
    int stack_count;
} CharacEventItems;

/* CRUD Operations */
int CharacEventItems_Add(DBConnectionManager* manager, const CharacEventItems* record);
int CharacEventItems_Get(DBConnectionManager* manager, int id, CharacEventItems* record);
int CharacEventItems_Update(DBConnectionManager* manager, const CharacEventItems* record);
int CharacEventItems_Delete(DBConnectionManager* manager, int id);
int CharacEventItems_Exists(DBConnectionManager* manager, int id);
int CharacEventItems_GetAll(DBConnectionManager* manager, CharacEventItems* records, int max_count, int* actual_count);

#endif /* CHARAC_EVENT_ITEMS_ORM_H */