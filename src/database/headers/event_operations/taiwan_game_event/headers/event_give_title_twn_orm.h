#ifndef EVENT_GIVE_TITLE_TWN_ORM_H
#define EVENT_GIVE_TITLE_TWN_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char server_id;
    int charac_no;
    char occ_date[20];
} EventGiveTitleTwn;

/* CRUD Operations */
int EventGiveTitleTwn_Add(DBConnectionManager* manager, const EventGiveTitleTwn* record);
int EventGiveTitleTwn_Get(DBConnectionManager* manager, int m_id, EventGiveTitleTwn* record);
int EventGiveTitleTwn_Update(DBConnectionManager* manager, const EventGiveTitleTwn* record);
int EventGiveTitleTwn_Delete(DBConnectionManager* manager, int m_id);
int EventGiveTitleTwn_Exists(DBConnectionManager* manager, int m_id);
int EventGiveTitleTwn_GetAll(DBConnectionManager* manager, EventGiveTitleTwn* records, int max_count, int* actual_count);

#endif /* EVENT_GIVE_TITLE_TWN_ORM_H */