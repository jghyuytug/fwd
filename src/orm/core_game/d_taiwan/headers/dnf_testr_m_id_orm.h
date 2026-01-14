#ifndef DNF_TESTR_M_ID_ORM_H
#define DNF_TESTR_M_ID_ORM_H

#include "db_connection_manager.h"

/* DNF测试会员ID数据结构 */
typedef struct {
    int m_id;                   /* 会员ID (主键) */
    unsigned char sex;          /* 性别 (0=女, 1=男) */
} DnfTestrMId;

/* CRUD 操作 */
int DnfTestrMId_Add(DBConnectionManager* manager, const DnfTestrMId* member);
int DnfTestrMId_Get(DBConnectionManager* manager, int m_id, DnfTestrMId* member);
int DnfTestrMId_Update(DBConnectionManager* manager, const DnfTestrMId* member);
int DnfTestrMId_Delete(DBConnectionManager* manager, int m_id);
int DnfTestrMId_Exists(DBConnectionManager* manager, int m_id);

/* 业务操作 */
int DnfTestrMId_GetAll(DBConnectionManager* manager, DnfTestrMId* members,
                        int max_count, int* actual_count);
int DnfTestrMId_GetBySex(DBConnectionManager* manager, unsigned char sex,
                          DnfTestrMId* members, int max_count, int* actual_count);
int DnfTestrMId_SetSex(DBConnectionManager* manager, int m_id, unsigned char sex);
int DnfTestrMId_CountAll(DBConnectionManager* manager);
int DnfTestrMId_CountBySex(DBConnectionManager* manager, unsigned char sex);
int DnfTestrMId_DeleteAll(DBConnectionManager* manager);

/* 工具函数 */
void DnfTestrMId_PrintInfo(const DnfTestrMId* member);

#endif /* DNF_TESTR_M_ID_ORM_H */
