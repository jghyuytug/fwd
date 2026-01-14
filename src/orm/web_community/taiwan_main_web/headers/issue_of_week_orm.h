#ifndef ISSUE_OF_WEEK_ORM_H
#define ISSUE_OF_WEEK_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char occ_date[11];
    char title[51];
    char contents[1024];
    char dnf_main[1024];
    char radio_url[129];
    signed char opt_flag;
    int view_cnt;
} IssueOfWeek;

/* CRUD Operations */
int IssueOfWeek_Add(DBConnectionManager* manager, const IssueOfWeek* record);
int IssueOfWeek_Get(DBConnectionManager* manager, int no, IssueOfWeek* record);
int IssueOfWeek_Update(DBConnectionManager* manager, const IssueOfWeek* record);
int IssueOfWeek_Delete(DBConnectionManager* manager, int no);
int IssueOfWeek_Exists(DBConnectionManager* manager, int no);
int IssueOfWeek_GetAll(DBConnectionManager* manager, IssueOfWeek* records, int max_count, int* actual_count);

#endif /* ISSUE_OF_WEEK_ORM_H */