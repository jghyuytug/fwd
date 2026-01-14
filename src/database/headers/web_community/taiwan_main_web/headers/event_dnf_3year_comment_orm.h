#ifndef EVENT_DNF_3YEAR_COMMENT_ORM_H
#define EVENT_DNF_3YEAR_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
    signed char server_id;
    int charac_no;
    signed char job;
    char comment[31];
    int feel_cnt;
    signed char open_flag;
} EventDnf3YearComment;

/* CRUD Operations */
int EventDnf3YearComment_Add(DBConnectionManager* manager, const EventDnf3YearComment* record);
int EventDnf3YearComment_Get(DBConnectionManager* manager, int no, EventDnf3YearComment* record);
int EventDnf3YearComment_Update(DBConnectionManager* manager, const EventDnf3YearComment* record);
int EventDnf3YearComment_Delete(DBConnectionManager* manager, int no);
int EventDnf3YearComment_Exists(DBConnectionManager* manager, int no);
int EventDnf3YearComment_GetAll(DBConnectionManager* manager, EventDnf3YearComment* records, int max_count, int* actual_count);

#endif /* EVENT_DNF_3YEAR_COMMENT_ORM_H */