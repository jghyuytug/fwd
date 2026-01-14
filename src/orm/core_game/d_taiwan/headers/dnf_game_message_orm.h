#ifndef DNF_GAME_MESSAGE_ORM_H
#define DNF_GAME_MESSAGE_ORM_H

#include "db_connection_manager.h"

/* DNF游戏消息数据结构 */
typedef struct {
    unsigned int no;                /* 消息编号 (主键, auto_increment) */
    char message[256];              /* 消息内容 (最大255字符+\0) */
    unsigned char display_type;     /* 显示类型 (1=默认) */
    unsigned char start_h;          /* 开始小时 (0-23) */
    unsigned char end_h;            /* 结束小时 (0-23) */
    char occ_date[11];              /* 发生日期 (YYYY-MM-DD格式+\0) */
} DnfGameMessage;

/* CRUD 操作 */
int DnfGameMessage_Add(DBConnectionManager* manager, const DnfGameMessage* record);
int DnfGameMessage_Get(DBConnectionManager* manager, unsigned int no, DnfGameMessage* record);
int DnfGameMessage_Update(DBConnectionManager* manager, const DnfGameMessage* record);
int DnfGameMessage_Delete(DBConnectionManager* manager, unsigned int no);
int DnfGameMessage_Exists(DBConnectionManager* manager, unsigned int no);

/* 专项操作 */
int DnfGameMessage_UpdateMessage(DBConnectionManager* manager, unsigned int no, const char* message);
int DnfGameMessage_UpdateDisplayType(DBConnectionManager* manager, unsigned int no, unsigned char display_type);
int DnfGameMessage_UpdateTimeRange(DBConnectionManager* manager, unsigned int no,
                                   unsigned char start_h, unsigned char end_h);
int DnfGameMessage_SetMessageSchedule(DBConnectionManager* manager, unsigned int no,
                                      const char* message, unsigned char display_type,
                                      unsigned char start_h, unsigned char end_h, const char* occ_date);

/* 查询操作 */
int DnfGameMessage_GetByDisplayType(DBConnectionManager* manager, unsigned char display_type,
                                     DnfGameMessage** records, int* count);
int DnfGameMessage_GetByDate(DBConnectionManager* manager, const char* occ_date,
                             DnfGameMessage** records, int* count);
int DnfGameMessage_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                   const char* end_date, DnfGameMessage** records, int* count);
int DnfGameMessage_GetActiveMessages(DBConnectionManager* manager, unsigned char display_type,
                                      const char* current_date, unsigned char current_hour,
                                      DnfGameMessage** records, int* count);
int DnfGameMessage_Count(DBConnectionManager* manager);

/* 批量操作 */
int DnfGameMessage_DeleteByDate(DBConnectionManager* manager, const char* occ_date);
int DnfGameMessage_Clear(DBConnectionManager* manager);

/* 工具函数 */
void DnfGameMessage_PrintInfo(const DnfGameMessage* record);

#endif /* DNF_GAME_MESSAGE_ORM_H */
