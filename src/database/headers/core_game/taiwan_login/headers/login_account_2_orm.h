#ifndef LOGIN_ACCOUNT_2_ORM_H
#define LOGIN_ACCOUNT_2_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int m_id;       /* INT(10) UNSIGNED */
    int m_channel_no;
    unsigned char login_status; /* TINYINT(1) */
    char last_login_date[20];   /* DATETIME */
    char login_ip[16];          /* VARCHAR(15) -> 16 */
} LoginAccount2;

/* CRUD operations */
int LoginAccount2_Add(DBConnectionManager* manager, const LoginAccount2* record);
int LoginAccount2_Get(DBConnectionManager* manager, unsigned int m_id, LoginAccount2* record);
int LoginAccount2_Update(DBConnectionManager* manager, const LoginAccount2* record);
int LoginAccount2_Delete(DBConnectionManager* manager, unsigned int m_id);
int LoginAccount2_Exists(DBConnectionManager* manager, unsigned int m_id);

/* Business operations */
int LoginAccount2_UpdateLoginStatus(DBConnectionManager* manager, unsigned int m_id,
                                     unsigned char login_status);
int LoginAccount2_UpdateLastLogin(DBConnectionManager* manager, unsigned int m_id,
                                   const char* login_date, const char* login_ip);
int LoginAccount2_GetByStatus(DBConnectionManager* manager, unsigned char login_status,
                               LoginAccount2* records, int max_count, int* actual_count);
int LoginAccount2_GetByChannel(DBConnectionManager* manager, int channel_no,
                                LoginAccount2* records, int max_count, int* actual_count);

/* Utility */
void LoginAccount2_PrintInfo(const LoginAccount2* record);

#endif
