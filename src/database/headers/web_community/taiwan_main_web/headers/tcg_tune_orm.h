#ifndef TCG_TUNE_ORM_H
#define TCG_TUNE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char notice;
    signed char category;
    int m_id;
    char m_sex;
    int charac_no;
    char charac_name[21];
    signed char job;
    char title[121];
    int create_day;
    short comment;
    int view;
    int file;
} TcgTune;

/* CRUD Operations */
int TcgTune_Add(DBConnectionManager* manager, const TcgTune* record);
int TcgTune_Get(DBConnectionManager* manager, int no, TcgTune* record);
int TcgTune_Update(DBConnectionManager* manager, const TcgTune* record);
int TcgTune_Delete(DBConnectionManager* manager, int no);
int TcgTune_Exists(DBConnectionManager* manager, int no);
int TcgTune_GetAll(DBConnectionManager* manager, TcgTune* records, int max_count, int* actual_count);

#endif /* TCG_TUNE_ORM_H */