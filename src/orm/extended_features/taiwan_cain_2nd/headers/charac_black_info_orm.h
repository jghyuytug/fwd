#ifndef CHARAC_BLACK_INFO_ORM_H
#define CHARAC_BLACK_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int charac_no;
    short black_point;
    short offset_point;
    char problem_child_time[20];
} CharacBlackInfo;

/* CRUD Operations */
int CharacBlackInfo_Add(DBConnectionManager* manager, const CharacBlackInfo* record);
int CharacBlackInfo_Get(DBConnectionManager* manager, int charac_no, CharacBlackInfo* record);
int CharacBlackInfo_Update(DBConnectionManager* manager, const CharacBlackInfo* record);
int CharacBlackInfo_Delete(DBConnectionManager* manager, int charac_no);
int CharacBlackInfo_Exists(DBConnectionManager* manager, int charac_no);
int CharacBlackInfo_GetAll(DBConnectionManager* manager, CharacBlackInfo* records, int max_count, int* actual_count);

#endif /* CHARAC_BLACK_INFO_ORM_H */