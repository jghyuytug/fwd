#ifndef LOG_DEATHTOWER_VALUE_ORM_H
#define LOG_DEATHTOWER_VALUE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    signed char type;
    signed char level;
    int clear_stage;
    int recipeCnt;
    int rareCnt;
    int uniqCnt;
    int card_item_goldprice;
    int card_gold;
    int repair_price;
    int commonCnt;
    int uncommonCnt;
    int try_cnt;
} LogDeathtowerValue;

/* CRUD Operations */
int LogDeathtowerValue_Add(DBConnectionManager* manager, const LogDeathtowerValue* record);
int LogDeathtowerValue_Get(DBConnectionManager* manager, const char* occ_date, signed char type, signed char level, LogDeathtowerValue* record);
int LogDeathtowerValue_Update(DBConnectionManager* manager, const LogDeathtowerValue* record);
int LogDeathtowerValue_Delete(DBConnectionManager* manager, const char* occ_date, signed char type, signed char level);
int LogDeathtowerValue_Exists(DBConnectionManager* manager, const char* occ_date, signed char type, signed char level);
int LogDeathtowerValue_GetAll(DBConnectionManager* manager, LogDeathtowerValue* records, int max_count, int* actual_count);

#endif /* LOG_DEATHTOWER_VALUE_ORM_H */