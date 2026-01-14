#ifndef BAD_USER_ORM_H
#define BAD_USER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int no;          /* AUTO_INCREMENT primary key */
    int m_id;                 /* Member ID */
    int bad_code;             /* Bad behavior code */
    int create_day;           /* Creation day */
    int exit_day;             /* Exit day */
    int admin_n;              /* Administrator number */
} BadUser;

/* CRUD Operations */
int BadUser_Add(DBConnectionManager* manager, const BadUser* user);
int BadUser_Get(DBConnectionManager* manager, unsigned int no, BadUser* user);
int BadUser_Update(DBConnectionManager* manager, const BadUser* user);
int BadUser_Delete(DBConnectionManager* manager, unsigned int no);
int BadUser_Exists(DBConnectionManager* manager, unsigned int no);

/* Business Operations - Query by indexed fields */
int BadUser_GetByMId(DBConnectionManager* manager, int m_id,
                      BadUser* users, int max_count, int* actual_count);
int BadUser_GetByBadCode(DBConnectionManager* manager, int bad_code,
                          BadUser* users, int max_count, int* actual_count);
int BadUser_GetByExitDay(DBConnectionManager* manager, int exit_day,
                          BadUser* users, int max_count, int* actual_count);
int BadUser_GetAll(DBConnectionManager* manager, BadUser* users,
                    int max_count, int* actual_count);

/* Utility Functions */
void BadUser_PrintInfo(const BadUser* user);

#endif
