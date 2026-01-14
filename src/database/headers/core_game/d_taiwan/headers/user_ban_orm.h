#ifndef USER_BAN_ORM_H
#define USER_BAN_ORM_H

#include "db_connection_manager.h"

/* User ban record structure */
typedef struct {
    unsigned int no;              /* Primary key, auto_increment */
    unsigned char category;       /* Ban category (default 1) */
    unsigned int m_id;            /* Member ID */
    unsigned short ban_term;      /* Ban duration */
    unsigned char ban_reason;     /* Ban reason code */
    char detail_reason[1024];     /* Detailed ban reason (TEXT) */
    unsigned int ban_date;        /* Ban timestamp */
    char cancel_reason[1024];     /* Cancel reason (TEXT) */
    unsigned int cancel_date;     /* Cancel timestamp */
    unsigned int admin_id;        /* Admin ID who banned */
    unsigned char status;         /* Ban status */
    char first_ssn[7];            /* First part of SSN (VARCHAR 6) */
    char second_ssn[8];           /* Second part of SSN (VARCHAR 7) */
} UserBan;

/* CRUD Operations */
int UserBan_Add(DBConnectionManager* manager, const UserBan* ban);
int UserBan_Get(DBConnectionManager* manager, unsigned int no, UserBan* ban);
int UserBan_Update(DBConnectionManager* manager, const UserBan* ban);
int UserBan_Delete(DBConnectionManager* manager, unsigned int no);
int UserBan_Exists(DBConnectionManager* manager, unsigned int no);

/* Business Operations */
int UserBan_GetByMemberId(DBConnectionManager* manager, unsigned int m_id,
                          UserBan* bans, int max_count, int* actual_count);
int UserBan_GetByStatus(DBConnectionManager* manager, unsigned char status,
                        UserBan* bans, int max_count, int* actual_count);
int UserBan_GetActiveByMemberId(DBConnectionManager* manager, unsigned int m_id,
                                UserBan* ban);
int UserBan_UpdateStatus(DBConnectionManager* manager, unsigned int no,
                         unsigned char new_status);
int UserBan_CancelBan(DBConnectionManager* manager, unsigned int no,
                      const char* cancel_reason, unsigned int admin_id);
int UserBan_GetByFirstSSN(DBConnectionManager* manager, const char* first_ssn,
                          UserBan* bans, int max_count, int* actual_count);

/* Utility Functions */
void UserBan_PrintInfo(const UserBan* ban);

#endif /* USER_BAN_ORM_H */
