#ifndef DNF_GAME_MESSAGE_ORM_H
#define DNF_GAME_MESSAGE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char message[256];
    signed char display_type;
    signed char start_h;
    signed char end_h;
    char occ_date[11];
} DnfGameMessage;

/* CRUD Operations */
int DnfGameMessage_Add(DBConnectionManager* manager, const DnfGameMessage* record);
int DnfGameMessage_Get(DBConnectionManager* manager, int no, DnfGameMessage* record);
int DnfGameMessage_Update(DBConnectionManager* manager, const DnfGameMessage* record);
int DnfGameMessage_Delete(DBConnectionManager* manager, int no);
int DnfGameMessage_Exists(DBConnectionManager* manager, int no);
int DnfGameMessage_GetAll(DBConnectionManager* manager, DnfGameMessage* records, int max_count, int* actual_count);

#endif /* DNF_GAME_MESSAGE_ORM_H */