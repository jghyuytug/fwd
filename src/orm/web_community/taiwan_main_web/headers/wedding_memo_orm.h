#ifndef WEDDING_MEMO_ORM_H
#define WEDDING_MEMO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int guild_id;
    int m_id;
    int charac_no;
    char charac_name[21];
    char nick_name[13];
    char memo[101];
    char create_time[20];
} WeddingMemo;

/* CRUD Operations */
int WeddingMemo_Add(DBConnectionManager* manager, const WeddingMemo* record);
int WeddingMemo_Get(DBConnectionManager* manager, int no, WeddingMemo* record);
int WeddingMemo_Update(DBConnectionManager* manager, const WeddingMemo* record);
int WeddingMemo_Delete(DBConnectionManager* manager, int no);
int WeddingMemo_Exists(DBConnectionManager* manager, int no);
int WeddingMemo_GetAll(DBConnectionManager* manager, WeddingMemo* records, int max_count, int* actual_count);

#endif /* WEDDING_MEMO_ORM_H */