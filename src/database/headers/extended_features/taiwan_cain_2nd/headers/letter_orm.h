#ifndef LETTER_ORM_H
#define LETTER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int letter_id;
    int charac_no;
    int send_charac_no;
    char send_charac_name[21];
    char letter_text[256];
    char reg_date[20];
    signed char stat;
} Letter;

/* CRUD Operations */
int Letter_Add(DBConnectionManager* manager, const Letter* record);
int Letter_Get(DBConnectionManager* manager, int letter_id, Letter* record);
int Letter_Update(DBConnectionManager* manager, const Letter* record);
int Letter_Delete(DBConnectionManager* manager, int letter_id);
int Letter_Exists(DBConnectionManager* manager, int letter_id);
int Letter_GetAll(DBConnectionManager* manager, Letter* records, int max_count, int* actual_count);

#endif /* LETTER_ORM_H */