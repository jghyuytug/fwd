#ifndef NEXON_NONE_MEMNER_RESTRICTION_ORM_H
#define NEXON_NONE_MEMNER_RESTRICTION_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int m_id;
    unsigned int charac_id;
    char last_trade_time[20];
    unsigned int total_trade_gold;
    unsigned short trade_count;
    signed char nexon_user;
} NexonNoneMemnerRestriction;

/* CRUD Operations - Note: No primary key, using m_id+charac_id as identifier */
int NexonNoneMemnerRestriction_Add(DBConnectionManager* manager, const NexonNoneMemnerRestriction* restriction);
int NexonNoneMemnerRestriction_Get(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_id,
                                    NexonNoneMemnerRestriction* restriction);
int NexonNoneMemnerRestriction_Update(DBConnectionManager* manager, const NexonNoneMemnerRestriction* restriction);
int NexonNoneMemnerRestriction_Delete(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_id);
int NexonNoneMemnerRestriction_Exists(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_id);

/* Business Operations */
int NexonNoneMemnerRestriction_GetByMember(DBConnectionManager* manager, unsigned int m_id,
                                            NexonNoneMemnerRestriction* restrictions,
                                            int max_count, int* actual_count);
int NexonNoneMemnerRestriction_GetByNexonStatus(DBConnectionManager* manager, signed char nexon_user,
                                                 NexonNoneMemnerRestriction* restrictions,
                                                 int max_count, int* actual_count);
int NexonNoneMemnerRestriction_UpdateTradeInfo(DBConnectionManager* manager, unsigned int m_id,
                                                unsigned int charac_id, unsigned int gold_amount);
int NexonNoneMemnerRestriction_GetHighTraders(DBConnectionManager* manager, unsigned int min_gold,
                                               NexonNoneMemnerRestriction* restrictions,
                                               int max_count, int* actual_count);

/* Utility */
void NexonNoneMemnerRestriction_PrintInfo(const NexonNoneMemnerRestriction* restriction);

#endif
