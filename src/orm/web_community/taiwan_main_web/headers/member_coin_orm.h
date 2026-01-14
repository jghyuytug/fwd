#ifndef MEMBER_COIN_ORM_H
#define MEMBER_COIN_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int pay_coin;
    int event_coin;
} MemberCoin;

/* CRUD Operations */
int MemberCoin_Add(DBConnectionManager* manager, const MemberCoin* record);
int MemberCoin_Get(DBConnectionManager* manager, int m_id, MemberCoin* record);
int MemberCoin_Update(DBConnectionManager* manager, const MemberCoin* record);
int MemberCoin_Delete(DBConnectionManager* manager, int m_id);
int MemberCoin_Exists(DBConnectionManager* manager, int m_id);
int MemberCoin_GetAll(DBConnectionManager* manager, MemberCoin* records, int max_count, int* actual_count);

#endif /* MEMBER_COIN_ORM_H */