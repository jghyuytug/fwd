#ifndef TMP_CHARAC_ORM_H
#define TMP_CHARAC_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int charac_no;
} TmpCharac;

/* CRUD Operations */
int TmpCharac_Add(DBConnectionManager* manager, const TmpCharac* record);
int TmpCharac_Get(DBConnectionManager* manager, int m_id, int charac_no, TmpCharac* record);
int TmpCharac_Update(DBConnectionManager* manager, const TmpCharac* record);
int TmpCharac_Delete(DBConnectionManager* manager, int m_id, int charac_no);
int TmpCharac_Exists(DBConnectionManager* manager, int m_id, int charac_no);
int TmpCharac_GetAll(DBConnectionManager* manager, TmpCharac* records, int max_count, int* actual_count);

#endif /* TMP_CHARAC_ORM_H */