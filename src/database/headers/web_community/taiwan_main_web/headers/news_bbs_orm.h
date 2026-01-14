#ifndef NEWS_BBS_ORM_H
#define NEWS_BBS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char bbs_code;
    signed char emph_yn;
    int no;
    char user_id[13];
    int reg_date;
    signed char html_yn;
    char subject[51];
    char body[1024];
    short hits;
    int prev_no;
    int next_no;
    int updt_date;
    signed char use_yn;
    char file_name[51];
} NewsBbs;

/* CRUD Operations */
int NewsBbs_Add(DBConnectionManager* manager, const NewsBbs* record);
int NewsBbs_Get(DBConnectionManager* manager, signed char bbs_code, signed char emph_yn, int no, NewsBbs* record);
int NewsBbs_Update(DBConnectionManager* manager, const NewsBbs* record);
int NewsBbs_Delete(DBConnectionManager* manager, signed char bbs_code, signed char emph_yn, int no);
int NewsBbs_Exists(DBConnectionManager* manager, signed char bbs_code, signed char emph_yn, int no);
int NewsBbs_GetAll(DBConnectionManager* manager, NewsBbs* records, int max_count, int* actual_count);

#endif /* NEWS_BBS_ORM_H */