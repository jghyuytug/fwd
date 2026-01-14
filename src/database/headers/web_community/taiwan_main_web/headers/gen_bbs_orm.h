#ifndef GEN_BBS_ORM_H
#define GEN_BBS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int gno;
    signed char bd_id;
    signed char empyn;
    int mgno;
    signed char open;
    signed char main;
    int reg_date;
    int mod_date;
    int hits;
    char body_type[2];
    int m_id;
    char reg_id[21];
    char subject[51];
    signed char cs_flag;
    char requestor[21];
    signed char order_flag;
} GenBbs;

/* CRUD Operations */
int GenBbs_Add(DBConnectionManager* manager, const GenBbs* record);
int GenBbs_Get(DBConnectionManager* manager, int gno, GenBbs* record);
int GenBbs_Update(DBConnectionManager* manager, const GenBbs* record);
int GenBbs_Delete(DBConnectionManager* manager, int gno);
int GenBbs_Exists(DBConnectionManager* manager, int gno);
int GenBbs_GetAll(DBConnectionManager* manager, GenBbs* records, int max_count, int* actual_count);

#endif /* GEN_BBS_ORM_H */