#ifndef CHARAC_DUNGEON_TEST_ORM_H
#define CHARAC_DUNGEON_TEST_ORM_H

#include "db_connection_manager.h"

/* 角色副本测试数据结构 */
typedef struct {
    unsigned int charac_no;        /* 角色编号 (主键) */
    unsigned char* dungeon;        /* 副本测试数据 (blob) */
    unsigned int dungeon_len;      /* 副本数据长度 */
} CharacDungeonTest;

/* CRUD 操作 */
int CharacDungeonTest_Add(DBConnectionManager* manager, const CharacDungeonTest* record);
int CharacDungeonTest_Get(DBConnectionManager* manager, unsigned int charac_no, CharacDungeonTest* record);
int CharacDungeonTest_Update(DBConnectionManager* manager, const CharacDungeonTest* record);
int CharacDungeonTest_Delete(DBConnectionManager* manager, unsigned int charac_no);
int CharacDungeonTest_Exists(DBConnectionManager* manager, unsigned int charac_no);

/* 查询操作 */
int CharacDungeonTest_Count(DBConnectionManager* manager);
int CharacDungeonTest_GetAll(DBConnectionManager* manager, CharacDungeonTest** records, int* count);

/* 批量操作 */
int CharacDungeonTest_Clear(DBConnectionManager* manager);
int CharacDungeonTest_BatchDelete(DBConnectionManager* manager, const unsigned int* charac_nos, int count);

/* 工具函数 */
void CharacDungeonTest_Free(CharacDungeonTest* record);
void CharacDungeonTest_PrintInfo(const CharacDungeonTest* record);

#endif /* CHARAC_DUNGEON_TEST_ORM_H */
