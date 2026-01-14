#ifndef EVENT_1208_BINGO_ORM_H
#define EVENT_1208_BINGO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
    char occ_date[20];
    int board;
    signed char reward;
} Event1208Bingo;

/* CRUD Operations */
int Event1208Bingo_Add(DBConnectionManager* manager, const Event1208Bingo* record);
int Event1208Bingo_Get(DBConnectionManager* manager, int no, Event1208Bingo* record);
int Event1208Bingo_Update(DBConnectionManager* manager, const Event1208Bingo* record);
int Event1208Bingo_Delete(DBConnectionManager* manager, int no);
int Event1208Bingo_Exists(DBConnectionManager* manager, int no);
int Event1208Bingo_GetAll(DBConnectionManager* manager, Event1208Bingo* records, int max_count, int* actual_count);

#endif /* EVENT_1208_BINGO_ORM_H */