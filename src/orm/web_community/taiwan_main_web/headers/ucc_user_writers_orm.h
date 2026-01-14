#ifndef UCC_USER_WRITERS_ORM_H
#define UCC_USER_WRITERS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    int start_time;
    int end_time;
    char comment[1024];
} UccUserWriters;

/* CRUD Operations */
int UccUserWriters_Add(DBConnectionManager* manager, const UccUserWriters* record);
int UccUserWriters_GetAll(DBConnectionManager* manager, UccUserWriters* records, int max_count, int* actual_count);

#endif /* UCC_USER_WRITERS_ORM_H */