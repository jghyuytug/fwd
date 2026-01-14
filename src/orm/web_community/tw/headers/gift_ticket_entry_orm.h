#ifndef GIFT_TICKET_ENTRY_ORM_H
#define GIFT_TICKET_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    short gift_no;
    int buyer_id;
    int buyer_date;
    char buyer_code[22];
    int buyer_check;
    int other_id;
    int other_date;
    char other_code[22];
    int other_check;
    char message[201];
} GiftTicketEntry;

/* CRUD Operations */
int GiftTicketEntry_Add(DBConnectionManager* manager, const GiftTicketEntry* record);
int GiftTicketEntry_Get(DBConnectionManager* manager, int id, GiftTicketEntry* record);
int GiftTicketEntry_Update(DBConnectionManager* manager, const GiftTicketEntry* record);
int GiftTicketEntry_Delete(DBConnectionManager* manager, int id);
int GiftTicketEntry_Exists(DBConnectionManager* manager, int id);
int GiftTicketEntry_GetAll(DBConnectionManager* manager, GiftTicketEntry* records, int max_count, int* actual_count);

#endif /* GIFT_TICKET_ENTRY_ORM_H */