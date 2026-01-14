#ifndef EVENT_ARADLOTTO_0809_ENTRY_ORM_H
#define EVENT_ARADLOTTO_0809_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;           /* 账户ID (主键) */
    int occ_date;       /* 发生日期 (Unix时间戳) */
    char lotto_num[8];  /* 抽奖号码 char(7) → 8 */
} EventAradlotto0809Entry;

/* CRUD */
int EventAradlotto0809Entry_Add(DBConnectionManager* manager, const EventAradlotto0809Entry* entry);
int EventAradlotto0809Entry_Get(DBConnectionManager* manager, int m_id, EventAradlotto0809Entry* entry);
int EventAradlotto0809Entry_Update(DBConnectionManager* manager, const EventAradlotto0809Entry* entry);
int EventAradlotto0809Entry_Delete(DBConnectionManager* manager, int m_id);
int EventAradlotto0809Entry_Exists(DBConnectionManager* manager, int m_id);

/* 业务操作 */
int EventAradlotto0809Entry_GetByLottoNum(DBConnectionManager* manager, const char* lotto_num,
                                           EventAradlotto0809Entry* entries, int max_count, int* actual_count);
int EventAradlotto0809Entry_CountByLottoNum(DBConnectionManager* manager, const char* lotto_num);
int EventAradlotto0809Entry_GetByDateRange(DBConnectionManager* manager, int start_date, int end_date,
                                            EventAradlotto0809Entry* entries, int max_count, int* actual_count);

void EventAradlotto0809Entry_PrintInfo(const EventAradlotto0809Entry* entry);

#endif
