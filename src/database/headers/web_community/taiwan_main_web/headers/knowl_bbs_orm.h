#ifndef KNOWL_BBS_ORM_H
#define KNOWL_BBS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int gno;
    int kn_id;
    int reg_date;
    int mod_date;
    int hits;
    char body_type[2];
    int reg_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    char subject[51];
    signed char status;
    int up;
    int down;
    signed char other_flag;
    char url_go[21];
    char url_to[21];
} KnowlBbs;

/* CRUD Operations */
int KnowlBbs_Add(DBConnectionManager* manager, const KnowlBbs* record);
int KnowlBbs_Get(DBConnectionManager* manager, int gno, KnowlBbs* record);
int KnowlBbs_Update(DBConnectionManager* manager, const KnowlBbs* record);
int KnowlBbs_Delete(DBConnectionManager* manager, int gno);
int KnowlBbs_Exists(DBConnectionManager* manager, int gno);
int KnowlBbs_GetAll(DBConnectionManager* manager, KnowlBbs* records, int max_count, int* actual_count);

#endif /* KNOWL_BBS_ORM_H */