#ifndef M_WITHDRAW_ORM_H
#define M_WITHDRAW_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char user_id[7];
    char user_name[6];
    char first_ssn[4];
    char second_ssn[4];
    char passwd[9];
    char mobile_no[8];
    int reg_date;
    char email[26];
    signed char q_no;
    char q_answer[16];
    char updt_date[20];
    signed char state;
    short w_type;
    char w_cause[101];
    int w_date;
    char nickname[9];
} MWithdraw;

/* CRUD Operations */
int MWithdraw_Add(DBConnectionManager* manager, const MWithdraw* record);
int MWithdraw_Get(DBConnectionManager* manager, int m_id, MWithdraw* record);
int MWithdraw_Update(DBConnectionManager* manager, const MWithdraw* record);
int MWithdraw_Delete(DBConnectionManager* manager, int m_id);
int MWithdraw_Exists(DBConnectionManager* manager, int m_id);
int MWithdraw_GetAll(DBConnectionManager* manager, MWithdraw* records, int max_count, int* actual_count);

#endif /* M_WITHDRAW_ORM_H */