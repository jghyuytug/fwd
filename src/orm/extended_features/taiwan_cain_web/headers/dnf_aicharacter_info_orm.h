#ifndef DNF_AICHARACTER_INFO_ORM_H
#define DNF_AICHARACTER_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int idx;
    char ai_name_kr[121];
} DnfAicharacterInfo;

/* CRUD Operations */
int DnfAicharacterInfo_Add(DBConnectionManager* manager, const DnfAicharacterInfo* record);
int DnfAicharacterInfo_GetAll(DBConnectionManager* manager, DnfAicharacterInfo* records, int max_count, int* actual_count);

#endif /* DNF_AICHARACTER_INFO_ORM_H */