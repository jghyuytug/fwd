#ifndef ADMIN_MEMBER_ORM_H
#define ADMIN_MEMBER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char user_id[21];
    char password[21];
    char name[21];
    char email[256];
    char phone[256];
    char msn[21];
    char comment[1024];
    int reg_date;
    char confirm[2];
    char level[256];
    char level_group1[3];
    char level_group2[3];
    char level_group3[3];
    char level_group4[3];
    char level_group5[3];
    char level_group6[3];
} AdminMember;

/* CRUD Operations */
int AdminMember_Add(DBConnectionManager* manager, const AdminMember* record);
int AdminMember_Get(DBConnectionManager* manager, int no, AdminMember* record);
int AdminMember_Update(DBConnectionManager* manager, const AdminMember* record);
int AdminMember_Delete(DBConnectionManager* manager, int no);
int AdminMember_Exists(DBConnectionManager* manager, int no);
int AdminMember_GetAll(DBConnectionManager* manager, AdminMember* records, int max_count, int* actual_count);

#endif /* ADMIN_MEMBER_ORM_H */