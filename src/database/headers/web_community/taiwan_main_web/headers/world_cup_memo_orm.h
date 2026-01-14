#ifndef WORLD_CUP_MEMO_ORM_H
#define WORLD_CUP_MEMO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char nick_name[13];
    char memo[106];
    char create_time[20];
} WorldCupMemo;

/* CRUD Operations */
int WorldCupMemo_Add(DBConnectionManager* manager, const WorldCupMemo* record);
int WorldCupMemo_Get(DBConnectionManager* manager, int no, WorldCupMemo* record);
int WorldCupMemo_Update(DBConnectionManager* manager, const WorldCupMemo* record);
int WorldCupMemo_Delete(DBConnectionManager* manager, int no);
int WorldCupMemo_Exists(DBConnectionManager* manager, int no);
int WorldCupMemo_GetAll(DBConnectionManager* manager, WorldCupMemo* records, int max_count, int* actual_count);

#endif /* WORLD_CUP_MEMO_ORM_H */