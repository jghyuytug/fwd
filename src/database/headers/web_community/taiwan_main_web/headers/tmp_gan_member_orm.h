#ifndef TMP_GAN_MEMBER_ORM_H
#define TMP_GAN_MEMBER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int reg_date;
} TmpGanMember;

/* CRUD Operations */
int TmpGanMember_Add(DBConnectionManager* manager, const TmpGanMember* record);
int TmpGanMember_Get(DBConnectionManager* manager, int m_id, TmpGanMember* record);
int TmpGanMember_Update(DBConnectionManager* manager, const TmpGanMember* record);
int TmpGanMember_Delete(DBConnectionManager* manager, int m_id);
int TmpGanMember_Exists(DBConnectionManager* manager, int m_id);
int TmpGanMember_GetAll(DBConnectionManager* manager, TmpGanMember* records, int max_count, int* actual_count);

#endif /* TMP_GAN_MEMBER_ORM_H */