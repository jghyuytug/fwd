#ifndef LOGIN_ACCOUNT_1_ORM_H
#define LOGIN_ACCOUNT_1_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int m_id;       /* INT(10) UNSIGNED */
    int m_channel_no;
    unsigned char login_status; /* TINYINT(1) */
    char last_login_date[20];   /* DATETIME */
    char login_ip[16];          /* VARCHAR(15) -> 16 */
} LoginAccount1;

/* CRUD operations */
int LoginAccount1_Add(DBConnectionManager* manager, const LoginAccount1* record);
int LoginAccount1_Get(DBConnectionManager* manager, unsigned int m_id, LoginAccount1* record);
int LoginAccount1_Update(DBConnectionManager* manager, const LoginAccount1* record);
int LoginAccount1_Delete(DBConnectionManager* manager, unsigned int m_id);
int LoginAccount1_Exists(DBConnectionManager* manager, unsigned int m_id);

/* Business operations */
int LoginAccount1_UpdateLoginStatus(DBConnectionManager* manager, unsigned int m_id,
                                     unsigned char login_status);
int LoginAccount1_UpdateLastLogin(DBConnectionManager* manager, unsigned int m_id,
                                   const char* login_date, const char* login_ip);
int LoginAccount1_GetByStatus(DBConnectionManager* manager, unsigned char login_status,
                               LoginAccount1* records, int max_count, int* actual_count);
int LoginAccount1_GetByChannel(DBConnectionManager* manager, int channel_no,
                                LoginAccount1* records, int max_count, int* actual_count);

/* Utility */
void LoginAccount1_PrintInfo(const LoginAccount1* record);

#endif
