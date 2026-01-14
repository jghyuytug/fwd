#ifndef CHARAC_TITLEBOOK_ORM_H
#define CHARAC_TITLEBOOK_ORM_H

#include "db_connection_manager.h"

/**
 * 角色称号簿 ORM
 *
 * 表: charac_titlebook
 * 主键: charac_no
 * 功能: 记录角色获得的称号，分为4个分类存储（特定称号、通用称号、绝望之塔称号、活动称号）
 */

#define MAX_TITLEBOOK_BLOB_SIZE 65535

/* 数据结构 */
typedef struct {
    unsigned int charac_no;              /* 角色编号 (主键) */

    /* 称号数据区（BLOB） */
    unsigned char* specific_section;     /* 特定称号区 */
    unsigned int specific_section_len;   /* 特定称号区长度 */

    unsigned char* general_section;      /* 通用称号区 */
    unsigned int general_section_len;    /* 通用称号区长度 */

    unsigned char* despair;              /* 绝望之塔称号区 */
    unsigned int despair_len;            /* 绝望之塔称号区长度 */

    unsigned char* event;                /* 活动称号区 */
    unsigned int event_len;              /* 活动称号区长度 */
} CharacTitlebook;

/* ==================== CRUD 操作 ==================== */

/**
 * 添加或更新角色称号簿
 */
int CharacTitlebook_Add(DBConnectionManager* manager, const CharacTitlebook* record);

/**
 * 获取角色称号簿
 */
int CharacTitlebook_Get(DBConnectionManager* manager, unsigned int charac_no, CharacTitlebook* record);

/**
 * 更新角色称号簿
 */
int CharacTitlebook_Update(DBConnectionManager* manager, const CharacTitlebook* record);

/**
 * 删除角色称号簿
 */
int CharacTitlebook_Delete(DBConnectionManager* manager, unsigned int charac_no);

/**
 * 检查角色称号簿是否存在
 */
int CharacTitlebook_Exists(DBConnectionManager* manager, unsigned int charac_no);

/* ==================== 专项操作 ==================== */

/**
 * 更新特定称号区
 */
int CharacTitlebook_UpdateSpecificSection(DBConnectionManager* manager, unsigned int charac_no,
                                           const unsigned char* data, unsigned int data_len);

/**
 * 更新通用称号区
 */
int CharacTitlebook_UpdateGeneralSection(DBConnectionManager* manager, unsigned int charac_no,
                                          const unsigned char* data, unsigned int data_len);

/**
 * 更新绝望之塔称号区
 */
int CharacTitlebook_UpdateDespair(DBConnectionManager* manager, unsigned int charac_no,
                                   const unsigned char* data, unsigned int data_len);

/**
 * 更新活动称号区
 */
int CharacTitlebook_UpdateEvent(DBConnectionManager* manager, unsigned int charac_no,
                                 const unsigned char* data, unsigned int data_len);

/* ==================== 查询操作 ==================== */

/**
 * 获取总称号簿记录数
 */
int CharacTitlebook_GetTotalCount(DBConnectionManager* manager);

/* ==================== 工具函数 ==================== */

/**
 * 打印角色称号簿信息（不显示二进制数据详情）
 */
void CharacTitlebook_PrintInfo(const CharacTitlebook* record);

/**
 * 释放CharacTitlebook结构中的动态内存
 */
void CharacTitlebook_Free(CharacTitlebook* record);

#endif /* CHARAC_TITLEBOOK_ORM_H */
