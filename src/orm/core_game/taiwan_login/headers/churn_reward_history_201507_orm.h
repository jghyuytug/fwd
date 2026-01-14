#ifndef CHURN_REWARD_HISTORY_201507_ORM_H
#define CHURN_REWARD_HISTORY_201507_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int m_id;           /* Member ID (PK1) */
    char occ_time[20];           /* Occurrence time (PK2) */
    unsigned char server_id;     /* Server ID */
    unsigned int charac_no;      /* Character number */
    unsigned int item_id;        /* Item ID */
    unsigned int add_info;       /* Additional info */
    unsigned int luck_point;     /* Luck points */
    unsigned int reward_order;   /* Reward order */
    unsigned int cera;           /* CERA amount */
} ChurnRewardHistory201507;

/* CRUD Operations */
int ChurnRewardHistory201507_Add(DBConnectionManager* manager, const ChurnRewardHistory201507* history);
int ChurnRewardHistory201507_Get(DBConnectionManager* manager, unsigned int m_id,
                                  const char* occ_time, ChurnRewardHistory201507* history);
int ChurnRewardHistory201507_Delete(DBConnectionManager* manager, unsigned int m_id, const char* occ_time);
int ChurnRewardHistory201507_Exists(DBConnectionManager* manager, unsigned int m_id, const char* occ_time);

/* Business Operations */
int ChurnRewardHistory201507_GetByMember(DBConnectionManager* manager, unsigned int m_id,
                                          ChurnRewardHistory201507* histories, int max_count, int* actual_count);
int ChurnRewardHistory201507_GetByTimeRange(DBConnectionManager* manager, const char* start_time,
                                              const char* end_time, ChurnRewardHistory201507* histories,
                                              int max_count, int* actual_count);

/* Utility Functions */
void ChurnRewardHistory201507_PrintInfo(const ChurnRewardHistory201507* history);

#endif /* CHURN_REWARD_HISTORY_201507_ORM_H */
