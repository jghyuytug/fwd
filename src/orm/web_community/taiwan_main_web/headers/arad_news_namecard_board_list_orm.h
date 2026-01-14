#ifndef ARAD_NEWS_NAMECARD_BOARD_LIST_ORM_H
#define ARAD_NEWS_NAMECARD_BOARD_LIST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char board_type;
    int board_no;
    int create_time;
} AradNewsNamecardBoardList;

/* CRUD Operations */
int AradNewsNamecardBoardList_Add(DBConnectionManager* manager, const AradNewsNamecardBoardList* record);
int AradNewsNamecardBoardList_Get(DBConnectionManager* manager, int m_id, signed char board_type, int board_no, AradNewsNamecardBoardList* record);
int AradNewsNamecardBoardList_Update(DBConnectionManager* manager, const AradNewsNamecardBoardList* record);
int AradNewsNamecardBoardList_Delete(DBConnectionManager* manager, int m_id, signed char board_type, int board_no);
int AradNewsNamecardBoardList_Exists(DBConnectionManager* manager, int m_id, signed char board_type, int board_no);
int AradNewsNamecardBoardList_GetAll(DBConnectionManager* manager, AradNewsNamecardBoardList* records, int max_count, int* actual_count);

#endif /* ARAD_NEWS_NAMECARD_BOARD_LIST_ORM_H */