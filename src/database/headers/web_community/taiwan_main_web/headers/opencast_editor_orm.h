#ifndef OPENCAST_EDITOR_ORM_H
#define OPENCAST_EDITOR_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char reg_time[20];
    char cast_name[21];
    int cate_no;
    char cast_intro[256];
    char name[21];
    char homepage[51];
    char contact[21];
    char email[51];
    char editor_intro[1024];
    signed char mng_type;
    signed char order_num;
    signed char grade;
} OpencastEditor;

/* CRUD Operations */
int OpencastEditor_Add(DBConnectionManager* manager, const OpencastEditor* record);
int OpencastEditor_Get(DBConnectionManager* manager, int m_id, OpencastEditor* record);
int OpencastEditor_Update(DBConnectionManager* manager, const OpencastEditor* record);
int OpencastEditor_Delete(DBConnectionManager* manager, int m_id);
int OpencastEditor_Exists(DBConnectionManager* manager, int m_id);
int OpencastEditor_GetAll(DBConnectionManager* manager, OpencastEditor* records, int max_count, int* actual_count);

#endif /* OPENCAST_EDITOR_ORM_H */