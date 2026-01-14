#ifndef OPENCAST_SUBSCRIBE_ORM_H
#define OPENCAST_SUBSCRIBE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
    int cc_no;
    char occ_date[11];
} OpencastSubscribe;

/* CRUD Operations */
int OpencastSubscribe_Add(DBConnectionManager* manager, const OpencastSubscribe* record);
int OpencastSubscribe_Get(DBConnectionManager* manager, int no, OpencastSubscribe* record);
int OpencastSubscribe_Update(DBConnectionManager* manager, const OpencastSubscribe* record);
int OpencastSubscribe_Delete(DBConnectionManager* manager, int no);
int OpencastSubscribe_Exists(DBConnectionManager* manager, int no);
int OpencastSubscribe_GetAll(DBConnectionManager* manager, OpencastSubscribe* records, int max_count, int* actual_count);

#endif /* OPENCAST_SUBSCRIBE_ORM_H */