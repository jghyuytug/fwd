#ifndef BAD_USER_ORM_H
#define BAD_USER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
    int bad_code;
    int create_day;
    int exit_day;
    int admin_n;
} BadUser;

/* CRUD Operations */
int BadUser_Add(DBConnectionManager* manager, const BadUser* record);
int BadUser_Get(DBConnectionManager* manager, int no, BadUser* record);
int BadUser_Update(DBConnectionManager* manager, const BadUser* record);
int BadUser_Delete(DBConnectionManager* manager, int no);
int BadUser_Exists(DBConnectionManager* manager, int no);
int BadUser_GetAll(DBConnectionManager* manager, BadUser* records, int max_count, int* actual_count);

#endif /* BAD_USER_ORM_H */