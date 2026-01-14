#ifndef DNF_PCROOM_ORM_H
#define DNF_PCROOM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int ip_no;
    char district[11];
    char firm_name[26];
    char telephone[11];
    char address[76];
    char leader[16];
    char start_ip[8];
    char end_ip[8];
} DnfPcroom;

/* CRUD Operations */
int DnfPcroom_Add(DBConnectionManager* manager, const DnfPcroom* record);
int DnfPcroom_Get(DBConnectionManager* manager, int ip_no, DnfPcroom* record);
int DnfPcroom_Update(DBConnectionManager* manager, const DnfPcroom* record);
int DnfPcroom_Delete(DBConnectionManager* manager, int ip_no);
int DnfPcroom_Exists(DBConnectionManager* manager, int ip_no);
int DnfPcroom_GetAll(DBConnectionManager* manager, DnfPcroom* records, int max_count, int* actual_count);

#endif /* DNF_PCROOM_ORM_H */