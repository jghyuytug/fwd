#ifndef DNF_FAQ_ORM_H
#define DNF_FAQ_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char cat1;
    signed char cat2;
    int regdate;
    int hit;
    char subject[201];
} DnfFaq;

/* CRUD Operations */
int DnfFaq_Add(DBConnectionManager* manager, const DnfFaq* record);
int DnfFaq_Get(DBConnectionManager* manager, int no, DnfFaq* record);
int DnfFaq_Update(DBConnectionManager* manager, const DnfFaq* record);
int DnfFaq_Delete(DBConnectionManager* manager, int no);
int DnfFaq_Exists(DBConnectionManager* manager, int no);
int DnfFaq_GetAll(DBConnectionManager* manager, DnfFaq* records, int max_count, int* actual_count);

#endif /* DNF_FAQ_ORM_H */