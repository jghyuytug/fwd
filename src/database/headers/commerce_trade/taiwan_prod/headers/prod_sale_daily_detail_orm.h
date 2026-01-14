#ifndef PROD_SALE_DAILY_DETAIL_ORM_H
#define PROD_SALE_DAILY_DETAIL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char sale_date[11];
    short slot_part_code;
    int ipg_no;
    char sex;
    signed char age;
    signed char job;
    int volume;
    long long amount;
} ProdSaleDailyDetail;

/* CRUD Operations */
int ProdSaleDailyDetail_Add(DBConnectionManager* manager, const ProdSaleDailyDetail* record);
int ProdSaleDailyDetail_Get(DBConnectionManager* manager, const char* sale_date, int ipg_no, char sex, signed char age, signed char job, ProdSaleDailyDetail* record);
int ProdSaleDailyDetail_Update(DBConnectionManager* manager, const ProdSaleDailyDetail* record);
int ProdSaleDailyDetail_Delete(DBConnectionManager* manager, const char* sale_date, int ipg_no, char sex, signed char age, signed char job);
int ProdSaleDailyDetail_Exists(DBConnectionManager* manager, const char* sale_date, int ipg_no, char sex, signed char age, signed char job);
int ProdSaleDailyDetail_GetAll(DBConnectionManager* manager, ProdSaleDailyDetail* records, int max_count, int* actual_count);

#endif /* PROD_SALE_DAILY_DETAIL_ORM_H */