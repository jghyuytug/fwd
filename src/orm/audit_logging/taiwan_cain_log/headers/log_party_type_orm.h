#ifndef LOG_PARTY_TYPE_ORM_H
#define LOG_PARTY_TYPE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int channel_no;
    char occ_date[11];
    int guild_count;
    int member_count;
    int helped_count;
    int normal_count;
    int member_helped_count;
} LogPartyType;

/* CRUD Operations */
int LogPartyType_Add(DBConnectionManager* manager, const LogPartyType* record);
int LogPartyType_Get(DBConnectionManager* manager, int channel_no, const char* occ_date, LogPartyType* record);
int LogPartyType_Update(DBConnectionManager* manager, const LogPartyType* record);
int LogPartyType_Delete(DBConnectionManager* manager, int channel_no, const char* occ_date);
int LogPartyType_Exists(DBConnectionManager* manager, int channel_no, const char* occ_date);
int LogPartyType_GetAll(DBConnectionManager* manager, LogPartyType* records, int max_count, int* actual_count);

#endif /* LOG_PARTY_TYPE_ORM_H */