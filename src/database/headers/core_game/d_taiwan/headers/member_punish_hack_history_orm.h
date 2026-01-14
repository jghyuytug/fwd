#ifndef MEMBER_PUNISH_HACK_HISTORY_ORM_H
#define MEMBER_PUNISH_HACK_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int m_id;        /* Member ID */
    unsigned int occ_time;    /* Occurrence time (Unix timestamp) */
    unsigned int period;      /* Punishment period duration */
    signed char now_flag;     /* Current status flag */
    signed char auto_flag;    /* Auto punishment flag */
    char reason[251];         /* Reason for punishment (VARCHAR(250) -> 251) */
} MemberPunishHackHistory;

/* CRUD Operations */
int MemberPunishHackHistory_Add(DBConnectionManager* manager, const MemberPunishHackHistory* record);
int MemberPunishHackHistory_DeleteByMId(DBConnectionManager* manager, unsigned int m_id);
int MemberPunishHackHistory_DeleteByMIdAndTime(DBConnectionManager* manager,
                                                 unsigned int m_id,
                                                 unsigned int occ_time);

/* Query Operations */
int MemberPunishHackHistory_GetByMId(DBConnectionManager* manager,
                                      unsigned int m_id,
                                      MemberPunishHackHistory* records,
                                      int max_count,
                                      int* actual_count);

int MemberPunishHackHistory_GetByTimeRange(DBConnectionManager* manager,
                                            unsigned int start_time,
                                            unsigned int end_time,
                                            MemberPunishHackHistory* records,
                                            int max_count,
                                            int* actual_count);

int MemberPunishHackHistory_GetActive(DBConnectionManager* manager,
                                       unsigned int m_id,
                                       MemberPunishHackHistory* records,
                                       int max_count,
                                       int* actual_count);

int MemberPunishHackHistory_CountByMId(DBConnectionManager* manager, unsigned int m_id);

/* Update Operations */
int MemberPunishHackHistory_UpdateFlag(DBConnectionManager* manager,
                                        unsigned int m_id,
                                        unsigned int occ_time,
                                        signed char now_flag);

/* Utility */
void MemberPunishHackHistory_PrintInfo(const MemberPunishHackHistory* record);

#endif
