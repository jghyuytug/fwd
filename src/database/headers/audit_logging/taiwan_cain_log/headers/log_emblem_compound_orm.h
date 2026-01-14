#ifndef LOG_EMBLEM_COMPOUND_ORM_H
#define LOG_EMBLEM_COMPOUND_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    signed char channel_no;
    int grade0;
    int grade1;
    int grade2;
    int grade3;
    int grade4;
    int grade5;
    int grade6;
} LogEmblemCompound;

/* CRUD Operations */
int LogEmblemCompound_Add(DBConnectionManager* manager, const LogEmblemCompound* record);
int LogEmblemCompound_Get(DBConnectionManager* manager, const char* occ_date, signed char channel_no, LogEmblemCompound* record);
int LogEmblemCompound_Update(DBConnectionManager* manager, const LogEmblemCompound* record);
int LogEmblemCompound_Delete(DBConnectionManager* manager, const char* occ_date, signed char channel_no);
int LogEmblemCompound_Exists(DBConnectionManager* manager, const char* occ_date, signed char channel_no);
int LogEmblemCompound_GetAll(DBConnectionManager* manager, LogEmblemCompound* records, int max_count, int* actual_count);

#endif /* LOG_EMBLEM_COMPOUND_ORM_H */