#ifndef LOG_EMBLEM_CREATE_ORM_H
#define LOG_EMBLEM_CREATE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char cur_date[11];
    int grade0;
    int grade1;
    int grade2;
    int grade3;
    int grade4;
    int grade5;
    int grade6;
} LogEmblemCreate;

/* CRUD Operations */
int LogEmblemCreate_Add(DBConnectionManager* manager, const LogEmblemCreate* record);
int LogEmblemCreate_Get(DBConnectionManager* manager, const char* cur_date, LogEmblemCreate* record);
int LogEmblemCreate_Update(DBConnectionManager* manager, const LogEmblemCreate* record);
int LogEmblemCreate_Delete(DBConnectionManager* manager, const char* cur_date);
int LogEmblemCreate_Exists(DBConnectionManager* manager, const char* cur_date);
int LogEmblemCreate_GetAll(DBConnectionManager* manager, LogEmblemCreate* records, int max_count, int* actual_count);

#endif /* LOG_EMBLEM_CREATE_ORM_H */