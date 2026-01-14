#ifndef LOG_DEATHTOWER_PLAYDATA_PARTY_ORM_H
#define LOG_DEATHTOWER_PLAYDATA_PARTY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char type;
    signed char party_count;
    int playcount;
    int avg_clear_count;
} LogDeathtowerPlaydataParty;

/* CRUD Operations */
int LogDeathtowerPlaydataParty_Add(DBConnectionManager* manager, const LogDeathtowerPlaydataParty* record);
int LogDeathtowerPlaydataParty_Get(DBConnectionManager* manager, const char* occ_time, signed char type, signed char party_count, LogDeathtowerPlaydataParty* record);
int LogDeathtowerPlaydataParty_Update(DBConnectionManager* manager, const LogDeathtowerPlaydataParty* record);
int LogDeathtowerPlaydataParty_Delete(DBConnectionManager* manager, const char* occ_time, signed char type, signed char party_count);
int LogDeathtowerPlaydataParty_Exists(DBConnectionManager* manager, const char* occ_time, signed char type, signed char party_count);
int LogDeathtowerPlaydataParty_GetAll(DBConnectionManager* manager, LogDeathtowerPlaydataParty* records, int max_count, int* actual_count);

#endif /* LOG_DEATHTOWER_PLAYDATA_PARTY_ORM_H */