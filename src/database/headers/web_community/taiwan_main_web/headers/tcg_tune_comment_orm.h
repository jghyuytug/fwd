#ifndef TCG_TUNE_COMMENT_ORM_H
#define TCG_TUNE_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    int m_id;
    int charac_no;
    char charac_name[21];
    char comment_text[105];
    char ip[16];
    int create_day;
} TcgTuneComment;

/* CRUD Operations */
int TcgTuneComment_Add(DBConnectionManager* manager, const TcgTuneComment* record);
int TcgTuneComment_Get(DBConnectionManager* manager, int no, TcgTuneComment* record);
int TcgTuneComment_Update(DBConnectionManager* manager, const TcgTuneComment* record);
int TcgTuneComment_Delete(DBConnectionManager* manager, int no);
int TcgTuneComment_Exists(DBConnectionManager* manager, int no);
int TcgTuneComment_GetAll(DBConnectionManager* manager, TcgTuneComment* records, int max_count, int* actual_count);

#endif /* TCG_TUNE_COMMENT_ORM_H */