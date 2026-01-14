#ifndef NEWMAGAZINE_ISSUE_OF_WEEK_ORM_H
#define NEWMAGAZINE_ISSUE_OF_WEEK_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char occ_date[11];
    char title[51];
    char contents[1024];
    char dnf_main[1024];
} NewmagazineIssueOfWeek;

/* CRUD Operations */
int NewmagazineIssueOfWeek_Add(DBConnectionManager* manager, const NewmagazineIssueOfWeek* record);
int NewmagazineIssueOfWeek_Get(DBConnectionManager* manager, int no, NewmagazineIssueOfWeek* record);
int NewmagazineIssueOfWeek_Update(DBConnectionManager* manager, const NewmagazineIssueOfWeek* record);
int NewmagazineIssueOfWeek_Delete(DBConnectionManager* manager, int no);
int NewmagazineIssueOfWeek_Exists(DBConnectionManager* manager, int no);
int NewmagazineIssueOfWeek_GetAll(DBConnectionManager* manager, NewmagazineIssueOfWeek* records, int max_count, int* actual_count);

#endif /* NEWMAGAZINE_ISSUE_OF_WEEK_ORM_H */