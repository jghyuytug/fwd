#ifndef OPTIONS_ORM_H
#define OPTIONS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int Id;
    int RechargeOption_Ratio;
    char PostalTitle[256];
    char PostalText[256];
    char CharacUpgradeLevelExpTable[1024];
    char AdminId[256];
} Options;

/* CRUD Operations */
int Options_Add(DBConnectionManager* manager, const Options* record);
int Options_Get(DBConnectionManager* manager, int Id, Options* record);
int Options_Update(DBConnectionManager* manager, const Options* record);
int Options_Delete(DBConnectionManager* manager, int Id);
int Options_Exists(DBConnectionManager* manager, int Id);
int Options_GetAll(DBConnectionManager* manager, Options* records, int max_count, int* actual_count);

#endif /* OPTIONS_ORM_H */