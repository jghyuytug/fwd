#ifndef LOGIN_ACCOUNT_3_ORM_H
#define LOGIN_ACCOUNT_3_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int m_id;       /* INT(10) UNSIGNED */
    int m_channel_no;
    unsigned char login_status; /* TINYINT(1) */
    char last_login_date[20];   /* DATETIME */
    char login_ip[16];          /* VARCHAR(15) -> 16 */
} LoginAccount3;

/* CRUD operations */
int LoginAccount3_Add(DBConnectionManager* manager, const LoginAccount3* record);
int LoginAccount3_Get(DBConnectionManager* manager, unsigned int m_id, LoginAccount3* record);
int LoginAccount3_Update(DBConnectionManager* manager, const LoginAccount3* record);
int LoginAccount3_Delete(DBConnectionManager* manager, unsigned int m_id);
int LoginAccount3_Exists(DBConnectionManager* manager, unsigned int m_id);

/* Business operations */
int LoginAccount3_UpdateLoginStatus(DBConnectionManager* manager, unsigned int m_id,
                                     unsigned char login_status);
int LoginAccount3_UpdateLastLogin(DBConnectionManager* manager, unsigned int m_id,
                                   const char* login_date, const char* login_ip);
int LoginAccount3_GetByStatus(DBConnectionManager* manager, unsigned char login_status,
                               LoginAccount3* records, int max_count, int* actual_count);
int LoginAccount3_GetByChannel(DBConnectionManager* manager, int channel_no,
                                LoginAccount3* records, int max_count, int* actual_count);

/* Utility */
void LoginAccount3_PrintInfo(const LoginAccount3* record);

#endif
