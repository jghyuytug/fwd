#ifndef DNF_EVENT_ADDRESS_ORM_H
#define DNF_EVENT_ADDRESS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int event_id;
    int m_id;
    char occ_date[20];
    char zipcode[8];
    char address[151];
    char phone_no[16];
} DnfEventAddress;

/* CRUD Operations */
int DnfEventAddress_Add(DBConnectionManager* manager, const DnfEventAddress* record);
int DnfEventAddress_Get(DBConnectionManager* manager, int event_id, int m_id, DnfEventAddress* record);
int DnfEventAddress_Update(DBConnectionManager* manager, const DnfEventAddress* record);
int DnfEventAddress_Delete(DBConnectionManager* manager, int event_id, int m_id);
int DnfEventAddress_Exists(DBConnectionManager* manager, int event_id, int m_id);
int DnfEventAddress_GetAll(DBConnectionManager* manager, DnfEventAddress* records, int max_count, int* actual_count);

#endif /* DNF_EVENT_ADDRESS_ORM_H */