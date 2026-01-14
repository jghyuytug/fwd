#ifndef CHURN_REWARD_HISTORY_201506_ORM_H
#define CHURN_REWARD_HISTORY_201506_ORM_H

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
} ChurnRewardHistory201506;

/* CRUD Operations */
int ChurnRewardHistory201506_Add(DBConnectionManager* manager, const ChurnRewardHistory201506* history);
int ChurnRewardHistory201506_Get(DBConnectionManager* manager, unsigned int m_id,
                                  const char* occ_time, ChurnRewardHistory201506* history);
int ChurnRewardHistory201506_Delete(DBConnectionManager* manager, unsigned int m_id, const char* occ_time);
int ChurnRewardHistory201506_Exists(DBConnectionManager* manager, unsigned int m_id, const char* occ_time);

/* Business Operations */
int ChurnRewardHistory201506_GetByMember(DBConnectionManager* manager, unsigned int m_id,
                                          ChurnRewardHistory201506* histories, int max_count, int* actual_count);
int ChurnRewardHistory201506_GetByTimeRange(DBConnectionManager* manager, const char* start_time,
                                              const char* end_time, ChurnRewardHistory201506* histories,
                                              int max_count, int* actual_count);

/* Utility Functions */
void ChurnRewardHistory201506_PrintInfo(const ChurnRewardHistory201506* history);

#endif /* CHURN_REWARD_HISTORY_201506_ORM_H */
