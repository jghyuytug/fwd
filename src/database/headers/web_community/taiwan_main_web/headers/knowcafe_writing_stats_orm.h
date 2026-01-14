#ifndef KNOWCAFE_WRITING_STATS_ORM_H
#define KNOWCAFE_WRITING_STATS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char date_enroll[11];
    int tip_sum;
    int tim_recom;
    int qa_sum;
    int qa_recom;
    int job_sum;
    int job_recom;
    int skill_sum;
    int skill_recom;
    int tip_hit;
    int qa_hit;
    int job_hit;
    int skill_hit;
    int discussion_sum;
    int discussion_recom;
    int discussion_hit;
} KnowcafeWritingStats;

/* CRUD Operations */
int KnowcafeWritingStats_Add(DBConnectionManager* manager, const KnowcafeWritingStats* record);
int KnowcafeWritingStats_Get(DBConnectionManager* manager, const char* date_enroll, KnowcafeWritingStats* record);
int KnowcafeWritingStats_Update(DBConnectionManager* manager, const KnowcafeWritingStats* record);
int KnowcafeWritingStats_Delete(DBConnectionManager* manager, const char* date_enroll);
int KnowcafeWritingStats_Exists(DBConnectionManager* manager, const char* date_enroll);
int KnowcafeWritingStats_GetAll(DBConnectionManager* manager, KnowcafeWritingStats* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_WRITING_STATS_ORM_H */