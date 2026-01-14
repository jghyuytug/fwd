#ifndef LETTER_DEL_ORM_H
#define LETTER_DEL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char sdate[11];
    int letter_id;
    int charac_no;
    int send_charac_no;
    char send_charac_name[21];
    char letter_text[256];
    char reg_date[20];
    signed char stat;
} LetterDel;

/* CRUD Operations */
int LetterDel_Add(DBConnectionManager* manager, const LetterDel* record);
int LetterDel_Get(DBConnectionManager* manager, const char* sdate, int letter_id, LetterDel* record);
int LetterDel_Update(DBConnectionManager* manager, const LetterDel* record);
int LetterDel_Delete(DBConnectionManager* manager, const char* sdate, int letter_id);
int LetterDel_Exists(DBConnectionManager* manager, const char* sdate, int letter_id);
int LetterDel_GetAll(DBConnectionManager* manager, LetterDel* records, int max_count, int* actual_count);

#endif /* LETTER_DEL_ORM_H */