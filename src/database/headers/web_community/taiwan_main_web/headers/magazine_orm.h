#ifndef MAGAZINE_ORM_H
#define MAGAZINE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int mz_id;
    signed char degree;
    signed char category;
    signed char seq;
    char title[121];
    int reg_date;
    int hists;
    int m_id;
    char user_id[21];
    int mod_date;
    char search_word[1024];
    char thumbnail_url[101];
    char writer[16];
    char summary[256];
    signed char open_flag;
} Magazine;

/* CRUD Operations */
int Magazine_Add(DBConnectionManager* manager, const Magazine* record);
int Magazine_Get(DBConnectionManager* manager, int mz_id, Magazine* record);
int Magazine_Update(DBConnectionManager* manager, const Magazine* record);
int Magazine_Delete(DBConnectionManager* manager, int mz_id);
int Magazine_Exists(DBConnectionManager* manager, int mz_id);
int Magazine_GetAll(DBConnectionManager* manager, Magazine* records, int max_count, int* actual_count);

#endif /* MAGAZINE_ORM_H */