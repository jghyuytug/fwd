#ifndef DNF_SCHL_CD_ORM_H
#define DNF_SCHL_CD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char schl_cd[5];
    char schl_name[31];
    char file_name[256];
    char param[256];
    char description[256];
} DnfSchlCd;

/* CRUD Operations */
int DnfSchlCd_Add(DBConnectionManager* manager, const DnfSchlCd* record);
int DnfSchlCd_Get(DBConnectionManager* manager, const char* schl_cd, DnfSchlCd* record);
int DnfSchlCd_Update(DBConnectionManager* manager, const DnfSchlCd* record);
int DnfSchlCd_Delete(DBConnectionManager* manager, const char* schl_cd);
int DnfSchlCd_Exists(DBConnectionManager* manager, const char* schl_cd);
int DnfSchlCd_GetAll(DBConnectionManager* manager, DnfSchlCd* records, int max_count, int* actual_count);

#endif /* DNF_SCHL_CD_ORM_H */