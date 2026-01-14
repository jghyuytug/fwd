#ifndef ARAD_NEWS_RECOM_ORM_H
#define ARAD_NEWS_RECOM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
    int content_no;
    int remote_addr;
} AradNewsRecom;

/* CRUD Operations */
int AradNewsRecom_Add(DBConnectionManager* manager, const AradNewsRecom* record);
int AradNewsRecom_Get(DBConnectionManager* manager, int no, AradNewsRecom* record);
int AradNewsRecom_Update(DBConnectionManager* manager, const AradNewsRecom* record);
int AradNewsRecom_Delete(DBConnectionManager* manager, int no);
int AradNewsRecom_Exists(DBConnectionManager* manager, int no);
int AradNewsRecom_GetAll(DBConnectionManager* manager, AradNewsRecom* records, int max_count, int* actual_count);

#endif /* ARAD_NEWS_RECOM_ORM_H */