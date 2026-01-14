#ifndef TCG_PUBLIC_INFO_ORM_H
#define TCG_PUBLIC_INFO_ORM_H

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
} TcgPublicInfo;

/* CRUD Operations */
int TcgPublicInfo_Add(DBConnectionManager* manager, const TcgPublicInfo* record);
int TcgPublicInfo_Get(DBConnectionManager* manager, int no, TcgPublicInfo* record);
int TcgPublicInfo_Update(DBConnectionManager* manager, const TcgPublicInfo* record);
int TcgPublicInfo_Delete(DBConnectionManager* manager, int no);
int TcgPublicInfo_Exists(DBConnectionManager* manager, int no);
int TcgPublicInfo_GetAll(DBConnectionManager* manager, TcgPublicInfo* records, int max_count, int* actual_count);

#endif /* TCG_PUBLIC_INFO_ORM_H */