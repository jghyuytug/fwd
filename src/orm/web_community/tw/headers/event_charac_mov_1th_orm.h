#ifndef EVENT_CHARAC_MOV_1TH_ORM_H
#define EVENT_CHARAC_MOV_1TH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    int m_id;
    signed char server_id;
    int charac_no;
    signed char move_server_id;
    int move_charac_no;
    int move_check;
} EventCharacMov1Th;

/* CRUD Operations */
int EventCharacMov1Th_Add(DBConnectionManager* manager, const EventCharacMov1Th* record);
int EventCharacMov1Th_Get(DBConnectionManager* manager, int id, EventCharacMov1Th* record);
int EventCharacMov1Th_Update(DBConnectionManager* manager, const EventCharacMov1Th* record);
int EventCharacMov1Th_Delete(DBConnectionManager* manager, int id);
int EventCharacMov1Th_Exists(DBConnectionManager* manager, int id);
int EventCharacMov1Th_GetAll(DBConnectionManager* manager, EventCharacMov1Th* records, int max_count, int* actual_count);

#endif /* EVENT_CHARAC_MOV_1TH_ORM_H */