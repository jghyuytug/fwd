#ifndef M_WITHDRAW_ORM_H
#define M_WITHDRAW_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int m_id;          /* AUTO_INCREMENT */
    char user_id[7];
    char user_name[6];
    char first_ssn[4];
    char second_ssn[4];
    char passwd[9];
    char mobile_no[8];
    int reg_date;               /* Unix timestamp */
    char email[26];
    unsigned char q_no;
    char q_answer[16];
    char updt_date[20];         /* TIMESTAMP - YYYY-MM-DD HH:MM:SS */
    unsigned char state;
    short w_type;
    char w_cause[101];
    int w_date;                 /* Unix timestamp */
    char nickname[9];
} MWithdraw;

/* CRUD Operations */
int MWithdraw_Add(DBConnectionManager* manager, const MWithdraw* withdraw);
int MWithdraw_Get(DBConnectionManager* manager, unsigned int m_id, MWithdraw* withdraw);
int MWithdraw_Update(DBConnectionManager* manager, const MWithdraw* withdraw);
int MWithdraw_Delete(DBConnectionManager* manager, unsigned int m_id);
int MWithdraw_Exists(DBConnectionManager* manager, unsigned int m_id);

/* Specialized Operations */
int MWithdraw_UpdateState(DBConnectionManager* manager, unsigned int m_id, unsigned char new_state);
int MWithdraw_UpdateWithdrawInfo(DBConnectionManager* manager, unsigned int m_id,
                                  short w_type, const char* w_cause, int w_date);
int MWithdraw_UpdateEmail(DBConnectionManager* manager, unsigned int m_id, const char* new_email);

/* Query Operations */
int MWithdraw_GetByUserId(DBConnectionManager* manager, const char* user_id, MWithdraw* withdraw);
int MWithdraw_GetByRegDateRange(DBConnectionManager* manager, int start_date, int end_date,
                                 MWithdraw* withdraws, int max_count, int* actual_count);
int MWithdraw_GetByState(DBConnectionManager* manager, unsigned char state,
                         MWithdraw* withdraws, int max_count, int* actual_count);
int MWithdraw_GetByWithdrawType(DBConnectionManager* manager, short w_type,
                                 MWithdraw* withdraws, int max_count, int* actual_count);
int MWithdraw_CountByState(DBConnectionManager* manager, unsigned char state);
int MWithdraw_CountByWithdrawType(DBConnectionManager* manager, short w_type);
int MWithdraw_CountByRegDateRange(DBConnectionManager* manager, int start_date, int end_date);

/* Batch Operations */
int MWithdraw_DeleteByRegDateRange(DBConnectionManager* manager, int start_date, int end_date);

/* Utility Functions */
void MWithdraw_PrintInfo(const MWithdraw* withdraw);

#endif /* M_WITHDRAW_ORM_H */
