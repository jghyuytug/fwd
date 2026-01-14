#ifndef BOARD_INFO_ORM_H
#define BOARD_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char bd_id;
    char name[41];
    char class[2];
    int reg_date;
    char reg_admin[11];
} BoardInfo;

/* CRUD Operations */
int BoardInfo_Add(DBConnectionManager* manager, const BoardInfo* record);
int BoardInfo_Get(DBConnectionManager* manager, signed char bd_id, BoardInfo* record);
int BoardInfo_Update(DBConnectionManager* manager, const BoardInfo* record);
int BoardInfo_Delete(DBConnectionManager* manager, signed char bd_id);
int BoardInfo_Exists(DBConnectionManager* manager, signed char bd_id);
int BoardInfo_GetAll(DBConnectionManager* manager, BoardInfo* records, int max_count, int* actual_count);

#endif /* BOARD_INFO_ORM_H */