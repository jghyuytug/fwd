#ifndef UCC_PUBLIC_INFO_ORM_H
#define UCC_PUBLIC_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char notice;
    signed char category;
    int m_id;
    char m_sex;
    int charac_no;
    char charac_name[21];
    signed char job;
    char title[121];
    int create_day;
    short comment;
    int view;
    int file;
} UccPublicInfo;

/* CRUD Operations */
int UccPublicInfo_Add(DBConnectionManager* manager, const UccPublicInfo* record);
int UccPublicInfo_Get(DBConnectionManager* manager, int no, UccPublicInfo* record);
int UccPublicInfo_Update(DBConnectionManager* manager, const UccPublicInfo* record);
int UccPublicInfo_Delete(DBConnectionManager* manager, int no);
int UccPublicInfo_Exists(DBConnectionManager* manager, int no);
int UccPublicInfo_GetAll(DBConnectionManager* manager, UccPublicInfo* records, int max_count, int* actual_count);

#endif /* UCC_PUBLIC_INFO_ORM_H */