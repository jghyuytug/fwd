#ifndef DNF_CHARAC_MOV_ORM_H
#define DNF_CHARAC_MOV_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    int m_id;
    signed char server_id;
    int charac_no;
    signed char move_server_id;
    int move_charac_no;
    int move_check;
} DnfCharacMov;

/* CRUD Operations */
int DnfCharacMov_Add(DBConnectionManager* manager, const DnfCharacMov* record);
int DnfCharacMov_Get(DBConnectionManager* manager, int id, DnfCharacMov* record);
int DnfCharacMov_Update(DBConnectionManager* manager, const DnfCharacMov* record);
int DnfCharacMov_Delete(DBConnectionManager* manager, int id);
int DnfCharacMov_Exists(DBConnectionManager* manager, int id);
int DnfCharacMov_GetAll(DBConnectionManager* manager, DnfCharacMov* records, int max_count, int* actual_count);

#endif /* DNF_CHARAC_MOV_ORM_H */