#ifndef CH_STATUS_ORM_H
#define CH_STATUS_ORM_H

#include "db_connection_manager.h"

/* 角色状态数据结构 */
typedef struct {
    unsigned char gc_group;     /* 分组ID */
    unsigned char gc_status;    /* 状态值 */
} ChStatus;

/* CRUD 操作 */
int ChStatus_Add(DBConnectionManager* manager, const ChStatus* record);
int ChStatus_Get(DBConnectionManager* manager, unsigned char gc_group, ChStatus* record);
int ChStatus_Update(DBConnectionManager* manager, const ChStatus* record);
int ChStatus_Delete(DBConnectionManager* manager, unsigned char gc_group);
int ChStatus_Exists(DBConnectionManager* manager, unsigned char gc_group);

/* 专项操作 */
int ChStatus_UpdateStatus(DBConnectionManager* manager, unsigned char gc_group, unsigned char gc_status);
int ChStatus_SetStatus(DBConnectionManager* manager, unsigned char gc_group, unsigned char gc_status);

/* 查询操作 */
int ChStatus_Count(DBConnectionManager* manager);
int ChStatus_GetAll(DBConnectionManager* manager, ChStatus** records, int* count);
int ChStatus_GetByStatus(DBConnectionManager* manager, unsigned char gc_status, ChStatus** records, int* count);
int ChStatus_GetByGroupRange(DBConnectionManager* manager, unsigned char min_group, unsigned char max_group,
                              ChStatus** records, int* count);

/* 批量操作 */
int ChStatus_Clear(DBConnectionManager* manager);
int ChStatus_BatchDelete(DBConnectionManager* manager, const unsigned char* gc_groups, int count);
int ChStatus_BatchSetStatus(DBConnectionManager* manager, const unsigned char* gc_groups, int count,
                             unsigned char gc_status);

/* 工具函数 */
void ChStatus_PrintInfo(const ChStatus* record);

#endif /* CH_STATUS_ORM_H */
