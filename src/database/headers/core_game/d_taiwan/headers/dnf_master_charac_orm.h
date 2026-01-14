#ifndef DNF_MASTER_CHARAC_ORM_H
#define DNF_MASTER_CHARAC_ORM_H

#include "db_connection_manager.h"

/* DNF主角色数据结构 */
typedef struct {
    unsigned int m_id;              /* 会员ID (主键) */
    unsigned char global_type;      /* 全局类型 (主键) */
    unsigned char server_id;        /* 服务器ID (主键) */
    unsigned int charac_no;         /* 角色编号 */
    char charac_name[21];           /* 角色名称 (最大20字符+\0) */
    unsigned char job;              /* 职业 */
    unsigned char lev;              /* 等级 */
} DnfMasterCharac;

/* CRUD 操作 */
int DnfMasterCharac_Add(DBConnectionManager* manager, const DnfMasterCharac* record);
int DnfMasterCharac_Get(DBConnectionManager* manager, unsigned int m_id, unsigned char global_type,
                        unsigned char server_id, DnfMasterCharac* record);
int DnfMasterCharac_Update(DBConnectionManager* manager, const DnfMasterCharac* record);
int DnfMasterCharac_Delete(DBConnectionManager* manager, unsigned int m_id, unsigned char global_type,
                           unsigned char server_id);
int DnfMasterCharac_Exists(DBConnectionManager* manager, unsigned int m_id, unsigned char global_type,
                           unsigned char server_id);

/* 专项操作 */
int DnfMasterCharac_UpdateLevel(DBConnectionManager* manager, unsigned int m_id, unsigned char global_type,
                                unsigned char server_id, unsigned char lev);
int DnfMasterCharac_UpdateCharacNo(DBConnectionManager* manager, unsigned int m_id, unsigned char global_type,
                                   unsigned char server_id, unsigned int charac_no);
int DnfMasterCharac_SetMasterCharac(DBConnectionManager* manager, unsigned int m_id, unsigned char global_type,
                                    unsigned char server_id, unsigned int charac_no, const char* charac_name,
                                    unsigned char job, unsigned char lev);

/* 查询操作 */
int DnfMasterCharac_Count(DBConnectionManager* manager);
int DnfMasterCharac_GetByMid(DBConnectionManager* manager, unsigned int m_id, DnfMasterCharac** records, int* count);
int DnfMasterCharac_GetByServer(DBConnectionManager* manager, unsigned char server_id, DnfMasterCharac** records, int* count);
int DnfMasterCharac_GetByGlobalType(DBConnectionManager* manager, unsigned char global_type, DnfMasterCharac** records, int* count);
int DnfMasterCharac_GetByCharacNo(DBConnectionManager* manager, unsigned int charac_no, DnfMasterCharac** records, int* count);

/* 批量操作 */
int DnfMasterCharac_Clear(DBConnectionManager* manager);
int DnfMasterCharac_DeleteByMid(DBConnectionManager* manager, unsigned int m_id);
int DnfMasterCharac_DeleteByServer(DBConnectionManager* manager, unsigned char server_id);

/* 工具函数 */
void DnfMasterCharac_PrintInfo(const DnfMasterCharac* record);

#endif /* DNF_MASTER_CHARAC_ORM_H */
