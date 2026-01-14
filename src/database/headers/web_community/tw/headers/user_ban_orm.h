#ifndef USER_BAN_ORM_H
#define USER_BAN_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    int m_id;
    short ban_term;
    signed char ban_reason;
    char detail_reason[1024];
    int ban_date;
    char cancel_reason[1024];
    int cancel_date;
    int admin_id;
    signed char status;
    char first_ssn[7];
    char second_ssn[8];
} UserBan;

/* CRUD Operations */
int UserBan_Add(DBConnectionManager* manager, const UserBan* record);
int UserBan_Get(DBConnectionManager* manager, int no, UserBan* record);
int UserBan_Update(DBConnectionManager* manager, const UserBan* record);
int UserBan_Delete(DBConnectionManager* manager, int no);
int UserBan_Exists(DBConnectionManager* manager, int no);
int UserBan_GetAll(DBConnectionManager* manager, UserBan* records, int max_count, int* actual_count);

#endif /* USER_BAN_ORM_H */