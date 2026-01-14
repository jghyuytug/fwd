#ifndef SECURITY_TIP_ORM_H
#define SECURITY_TIP_ORM_H

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
} SecurityTip;

/* CRUD Operations */
int SecurityTip_Add(DBConnectionManager* manager, const SecurityTip* record);
int SecurityTip_Get(DBConnectionManager* manager, int mz_id, SecurityTip* record);
int SecurityTip_Update(DBConnectionManager* manager, const SecurityTip* record);
int SecurityTip_Delete(DBConnectionManager* manager, int mz_id);
int SecurityTip_Exists(DBConnectionManager* manager, int mz_id);
int SecurityTip_GetAll(DBConnectionManager* manager, SecurityTip* records, int max_count, int* actual_count);

#endif /* SECURITY_TIP_ORM_H */