#ifndef KNOWCAFE_RANK_ORM_H
#define KNOWCAFE_RANK_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char knowcafe_type;
    int rank;
    int m_id;
    int total_point;
    int now_point;
    int old_rank;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    signed char black;
} KnowcafeRank;

/* CRUD Operations */
int KnowcafeRank_Add(DBConnectionManager* manager, const KnowcafeRank* record);
int KnowcafeRank_GetAll(DBConnectionManager* manager, KnowcafeRank* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_RANK_ORM_H */