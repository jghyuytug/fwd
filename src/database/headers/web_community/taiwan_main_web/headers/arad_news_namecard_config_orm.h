#ifndef ARAD_NEWS_NAMECARD_CONFIG_ORM_H
#define ARAD_NEWS_NAMECARD_CONFIG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    signed char lev;
    char referer_url[129];
    signed char blog_bgcolor;
} AradNewsNamecardConfig;

/* CRUD Operations */
int AradNewsNamecardConfig_Add(DBConnectionManager* manager, const AradNewsNamecardConfig* record);
int AradNewsNamecardConfig_Get(DBConnectionManager* manager, int m_id, AradNewsNamecardConfig* record);
int AradNewsNamecardConfig_Update(DBConnectionManager* manager, const AradNewsNamecardConfig* record);
int AradNewsNamecardConfig_Delete(DBConnectionManager* manager, int m_id);
int AradNewsNamecardConfig_Exists(DBConnectionManager* manager, int m_id);
int AradNewsNamecardConfig_GetAll(DBConnectionManager* manager, AradNewsNamecardConfig* records, int max_count, int* actual_count);

#endif /* ARAD_NEWS_NAMECARD_CONFIG_ORM_H */