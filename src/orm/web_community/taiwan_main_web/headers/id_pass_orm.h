#ifndef ID_PASS_ORM_H
#define ID_PASS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
    char user_id[17];
    char name[17];
    char phone[13];
    int code;
    char tid[21];
    char update_day[20];
    char create_day[20];
} IdPass;

/* CRUD Operations */
int IdPass_Add(DBConnectionManager* manager, const IdPass* record);
int IdPass_Get(DBConnectionManager* manager, int no, IdPass* record);
int IdPass_Update(DBConnectionManager* manager, const IdPass* record);
int IdPass_Delete(DBConnectionManager* manager, int no);
int IdPass_Exists(DBConnectionManager* manager, int no);
int IdPass_GetAll(DBConnectionManager* manager, IdPass* records, int max_count, int* actual_count);

#endif /* ID_PASS_ORM_H */