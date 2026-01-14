#ifndef MEMBER_AVATAR_COIN_ORM_H
#define MEMBER_AVATAR_COIN_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int avatar_coin;
} MemberAvatarCoin;

/* CRUD Operations */
int MemberAvatarCoin_Add(DBConnectionManager* manager, const MemberAvatarCoin* record);
int MemberAvatarCoin_Get(DBConnectionManager* manager, int m_id, MemberAvatarCoin* record);
int MemberAvatarCoin_Update(DBConnectionManager* manager, const MemberAvatarCoin* record);
int MemberAvatarCoin_Delete(DBConnectionManager* manager, int m_id);
int MemberAvatarCoin_Exists(DBConnectionManager* manager, int m_id);
int MemberAvatarCoin_GetAll(DBConnectionManager* manager, MemberAvatarCoin* records, int max_count, int* actual_count);

#endif /* MEMBER_AVATAR_COIN_ORM_H */