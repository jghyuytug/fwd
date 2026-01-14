#ifndef CHURN_MEMBER_INFO_ORM_H
#define CHURN_MEMBER_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int m_id;                 /* Member ID (Primary Key) */
    unsigned int accrue_cera;          /* Accumulated CERA points */
    char play_info[31];                /* Play information */
    unsigned int first_reward_time;    /* First reward timestamp */
    unsigned int last_reward_time;     /* Last reward timestamp */
    unsigned char server_id;           /* Server ID */
    unsigned int charac_no;            /* Character number */
    unsigned int item_id;              /* Item ID */
    unsigned char add_info;            /* Additional info flag */
    unsigned int luck_point;           /* Luck points */
    unsigned int last_update_time;     /* Last update timestamp */
    unsigned int second_reward_time;   /* Second reward timestamp */
    unsigned int quest_time;           /* Quest completion timestamp */
} ChurnMemberInfo;

/* CRUD Operations */
int ChurnMemberInfo_Add(DBConnectionManager* manager, const ChurnMemberInfo* info);
int ChurnMemberInfo_Get(DBConnectionManager* manager, unsigned int m_id, ChurnMemberInfo* info);
int ChurnMemberInfo_Update(DBConnectionManager* manager, const ChurnMemberInfo* info);
int ChurnMemberInfo_Delete(DBConnectionManager* manager, unsigned int m_id);
int ChurnMemberInfo_Exists(DBConnectionManager* manager, unsigned int m_id);

/* Business Operations */
int ChurnMemberInfo_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                                 ChurnMemberInfo* infos, int max_count, int* actual_count);
int ChurnMemberInfo_GetActiveMembers(DBConnectionManager* manager, unsigned int since_time,
                                      ChurnMemberInfo* infos, int max_count, int* actual_count);
int ChurnMemberInfo_UpdateRewardTime(DBConnectionManager* manager, unsigned int m_id,
                                      unsigned int reward_time);

/* Utility Functions */
void ChurnMemberInfo_PrintInfo(const ChurnMemberInfo* info);

#endif /* CHURN_MEMBER_INFO_ORM_H */
