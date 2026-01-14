#ifndef NEWMAGAZINE_UCC_ORM_H
#define NEWMAGAZINE_UCC_ORM_H

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
    signed char server_id;
    int charac_no;
    char charac_name[21];
    int mod_date;
    int point;
    int point_cnt;
    char search_word[1024];
    char thumbnail_url[101];
    char writer[16];
    char summary[256];
    signed char open_flag;
    int open_date;
} NewmagazineUcc;

/* CRUD Operations */
int NewmagazineUcc_Add(DBConnectionManager* manager, const NewmagazineUcc* record);
int NewmagazineUcc_Get(DBConnectionManager* manager, int mz_id, NewmagazineUcc* record);
int NewmagazineUcc_Update(DBConnectionManager* manager, const NewmagazineUcc* record);
int NewmagazineUcc_Delete(DBConnectionManager* manager, int mz_id);
int NewmagazineUcc_Exists(DBConnectionManager* manager, int mz_id);
int NewmagazineUcc_GetAll(DBConnectionManager* manager, NewmagazineUcc* records, int max_count, int* actual_count);

#endif /* NEWMAGAZINE_UCC_ORM_H */