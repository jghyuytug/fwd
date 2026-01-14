#ifndef TODAYDNF_ORM_H
#define TODAYDNF_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char first_title[33];
    char first_thumb[193];
    char first_referer[33];
    char first_url[193];
    short first_hit;
    char second_title[33];
    char second_thumb[193];
    char second_referer[33];
    char second_url[193];
    short second_hit;
    char third_title[33];
    char third_thumb[193];
    char third_referer[33];
    char third_url[193];
    short third_hit;
    char open_date[11];
    char reg_date[20];
} Todaydnf;

/* CRUD Operations */
int Todaydnf_Add(DBConnectionManager* manager, const Todaydnf* record);
int Todaydnf_Get(DBConnectionManager* manager, int no, Todaydnf* record);
int Todaydnf_Update(DBConnectionManager* manager, const Todaydnf* record);
int Todaydnf_Delete(DBConnectionManager* manager, int no);
int Todaydnf_Exists(DBConnectionManager* manager, int no);
int Todaydnf_GetAll(DBConnectionManager* manager, Todaydnf* records, int max_count, int* actual_count);

#endif /* TODAYDNF_ORM_H */