#ifndef GIFT_TICKET_ENTRY_ORM_H
#define GIFT_TICKET_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int id;              /* AUTO_INCREMENT primary key */
    unsigned short gift_no;       /* Gift number */
    unsigned int buyer_id;        /* Buyer member ID (indexed) */
    unsigned int buyer_date;      /* Buyer purchase date */
    char buyer_code[22];          /* Buyer code (VARCHAR 21 + 1) */
    unsigned int buyer_check;     /* Buyer check value */
    unsigned int other_id;        /* Receiver member ID (indexed) */
    unsigned int other_date;      /* Receiver date */
    char other_code[22];          /* Receiver code (VARCHAR 21 + 1) */
    unsigned int other_check;     /* Receiver check value */
    char message[201];            /* Gift message (VARCHAR 200 + 1) */
} GiftTicketEntry;

/* CRUD Operations */
int GiftTicketEntry_Add(DBConnectionManager* manager, const GiftTicketEntry* entry);
int GiftTicketEntry_Get(DBConnectionManager* manager, unsigned int id, GiftTicketEntry* entry);
int GiftTicketEntry_Update(DBConnectionManager* manager, const GiftTicketEntry* entry);
int GiftTicketEntry_Delete(DBConnectionManager* manager, unsigned int id);
int GiftTicketEntry_Exists(DBConnectionManager* manager, unsigned int id);

/* Business Operations - Query by indexed fields */
int GiftTicketEntry_GetByBuyerId(DBConnectionManager* manager, unsigned int buyer_id,
                                  GiftTicketEntry* entries, int max_count, int* actual_count);
int GiftTicketEntry_GetByOtherId(DBConnectionManager* manager, unsigned int other_id,
                                  GiftTicketEntry* entries, int max_count, int* actual_count);
int GiftTicketEntry_GetAll(DBConnectionManager* manager, GiftTicketEntry* entries,
                            int max_count, int* actual_count);

/* Utility Functions */
void GiftTicketEntry_PrintInfo(const GiftTicketEntry* entry);

#endif
