#ifndef DNF_RESTRICT_STATE_ORM_H
#define DNF_RESTRICT_STATE_ORM_H

#include "db_connection_manager.h"

/* DNF限制状态数据结构 */
typedef struct {
    int server_group;           /* 服务器组 (主键1) */
    int category;               /* 分类 (主键2) */
    int restrict_code;          /* 限制代码 (主键3) */
    char restrict_value;        /* 限制值 */
    char mod_date[20];          /* 修改日期 (YYYY-MM-DD HH:MM:SS) */
    char reg_date[20];          /* 注册日期 (YYYY-MM-DD HH:MM:SS) */
} DnfRestrictState;

/* CRUD 操作 */
int DnfRestrictState_Add(DBConnectionManager* manager, const DnfRestrictState* state);
int DnfRestrictState_Get(DBConnectionManager* manager, int server_group, int category,
                          int restrict_code, DnfRestrictState* state);
int DnfRestrictState_Update(DBConnectionManager* manager, const DnfRestrictState* state);
int DnfRestrictState_Delete(DBConnectionManager* manager, int server_group, int category,
                             int restrict_code);
int DnfRestrictState_Exists(DBConnectionManager* manager, int server_group, int category,
                             int restrict_code);

/* 业务操作 */
int DnfRestrictState_GetByServerGroup(DBConnectionManager* manager, int server_group,
                                       DnfRestrictState* states, int max_count, int* actual_count);
int DnfRestrictState_GetByCategory(DBConnectionManager* manager, int category,
                                    DnfRestrictState* states, int max_count, int* actual_count);
int DnfRestrictState_GetByServerGroupAndCategory(DBConnectionManager* manager, int server_group,
                                                  int category, DnfRestrictState* states,
                                                  int max_count, int* actual_count);
int DnfRestrictState_SetRestrictValue(DBConnectionManager* manager, int server_group,
                                       int category, int restrict_code, char restrict_value);
int DnfRestrictState_DeleteByServerGroup(DBConnectionManager* manager, int server_group);
int DnfRestrictState_CountByServerGroup(DBConnectionManager* manager, int server_group);

/* 工具函数 */
void DnfRestrictState_PrintInfo(const DnfRestrictState* state);

#endif /* DNF_RESTRICT_STATE_ORM_H */
