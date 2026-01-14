#ifndef BUG_REPORT_ORM_H
#define BUG_REPORT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    char occ_date1[20];
    char occ_date2[20];
    char enroll_date[20];
    char title[51];
    char comment[1024];
    char attachment1[201];
    char attachment2[201];
    char attachment3[201];
    char bug_class[2];
} BugReport;

/* CRUD Operations */
int BugReport_Add(DBConnectionManager* manager, const BugReport* record);
int BugReport_Get(DBConnectionManager* manager, int no, BugReport* record);
int BugReport_Update(DBConnectionManager* manager, const BugReport* record);
int BugReport_Delete(DBConnectionManager* manager, int no);
int BugReport_Exists(DBConnectionManager* manager, int no);
int BugReport_GetAll(DBConnectionManager* manager, BugReport* records, int max_count, int* actual_count);

#endif /* BUG_REPORT_ORM_H */