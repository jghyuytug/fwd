#ifndef LOG_UPGRADE_POT_ORM_H
#define LOG_UPGRADE_POT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    int id_2670203;
    int id_2670204;
    int id_2670205;
    int id_2670206;
    int id_2670241;
    int id_2670242;
    int id_2670243;
    int id_2670244;
    int id_2670245;
    int id_2670246;
    int id_2670247;
    int id_2670248;
    int id_2670249;
    int id_2670250;
    int id_2670251;
    int id_2670252;
    int id_2670253;
} LogUpgradePot;

/* CRUD Operations */
int LogUpgradePot_Add(DBConnectionManager* manager, const LogUpgradePot* record);
int LogUpgradePot_Get(DBConnectionManager* manager, const char* occ_date, LogUpgradePot* record);
int LogUpgradePot_Update(DBConnectionManager* manager, const LogUpgradePot* record);
int LogUpgradePot_Delete(DBConnectionManager* manager, const char* occ_date);
int LogUpgradePot_Exists(DBConnectionManager* manager, const char* occ_date);
int LogUpgradePot_GetAll(DBConnectionManager* manager, LogUpgradePot* records, int max_count, int* actual_count);

#endif /* LOG_UPGRADE_POT_ORM_H */