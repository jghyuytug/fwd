#ifndef VILLAGE_TICKET_ORM_H
#define VILLAGE_TICKET_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int charac_no;
    short village;
} VillageTicket;

/* CRUD Operations */
int VillageTicket_Add(DBConnectionManager* manager, const VillageTicket* record);
int VillageTicket_Get(DBConnectionManager* manager, int charac_no, short village, VillageTicket* record);
int VillageTicket_Update(DBConnectionManager* manager, const VillageTicket* record);
int VillageTicket_Delete(DBConnectionManager* manager, int charac_no, short village);
int VillageTicket_Exists(DBConnectionManager* manager, int charac_no, short village);
int VillageTicket_GetAll(DBConnectionManager* manager, VillageTicket* records, int max_count, int* actual_count);

#endif /* VILLAGE_TICKET_ORM_H */