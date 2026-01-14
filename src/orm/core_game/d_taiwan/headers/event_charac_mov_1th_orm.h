#ifndef EVENT_CHARAC_MOV_1TH_ORM_H
#define EVENT_CHARAC_MOV_1TH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int id;
    int m_id;
    unsigned char server_id;
    int charac_no;
    unsigned char move_server_id;
    int move_charac_no;
    int move_check;
    char event_code[51];
    char reg_date[20];
} EventCharacMov1th;

/* CRUD Operations */
int EventCharacMov1th_Add(DBConnectionManager* manager, const EventCharacMov1th* record);
int EventCharacMov1th_Get(DBConnectionManager* manager, unsigned int id, EventCharacMov1th* record);
int EventCharacMov1th_Update(DBConnectionManager* manager, const EventCharacMov1th* record);
int EventCharacMov1th_Delete(DBConnectionManager* manager, unsigned int id);
int EventCharacMov1th_Exists(DBConnectionManager* manager, unsigned int id);

/* Business Operations */
int EventCharacMov1th_GetByMember(DBConnectionManager* manager, int m_id,
                                   EventCharacMov1th* records, int max_count, int* actual_count);
int EventCharacMov1th_GetByCharacter(DBConnectionManager* manager, int m_id,
                                      unsigned char server_id, int charac_no,
                                      EventCharacMov1th* record);
int EventCharacMov1th_GetByEventCode(DBConnectionManager* manager, const char* event_code,
                                      EventCharacMov1th* records, int max_count, int* actual_count);
int EventCharacMov1th_GetPendingMoves(DBConnectionManager* manager,
                                       EventCharacMov1th* records, int max_count, int* actual_count);
int EventCharacMov1th_UpdateMoveCheck(DBConnectionManager* manager, unsigned int id, int move_check);
int EventCharacMov1th_CountByMember(DBConnectionManager* manager, int m_id, int* count);

/* Utility */
void EventCharacMov1th_PrintInfo(const EventCharacMov1th* record);

#endif
