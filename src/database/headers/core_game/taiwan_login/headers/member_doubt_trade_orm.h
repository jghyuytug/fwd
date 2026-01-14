#ifndef MEMBER_DOUBT_TRADE_ORM_H
#define MEMBER_DOUBT_TRADE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char last_update_time[20];
    unsigned short over_count;
} MemberDoubtTrade;

/* CRUD Operations */
int MemberDoubtTrade_Add(DBConnectionManager* manager, const MemberDoubtTrade* trade);
int MemberDoubtTrade_Get(DBConnectionManager* manager, int m_id, MemberDoubtTrade* trade);
int MemberDoubtTrade_Update(DBConnectionManager* manager, const MemberDoubtTrade* trade);
int MemberDoubtTrade_Delete(DBConnectionManager* manager, int m_id);
int MemberDoubtTrade_Exists(DBConnectionManager* manager, int m_id);

/* Business Operations */
int MemberDoubtTrade_IncrementCount(DBConnectionManager* manager, int m_id);
int MemberDoubtTrade_GetByMinCount(DBConnectionManager* manager, unsigned short min_count,
                                    MemberDoubtTrade* trades, int max_count, int* actual_count);
int MemberDoubtTrade_DeleteAll(DBConnectionManager* manager);

/* Utility Functions */
void MemberDoubtTrade_PrintInfo(const MemberDoubtTrade* trade);

#endif
