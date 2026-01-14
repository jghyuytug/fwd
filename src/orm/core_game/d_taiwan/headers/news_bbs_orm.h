#ifndef NEWS_BBS_ORM_H
#define NEWS_BBS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned char bbs_code;
    unsigned char emph_yn;
    unsigned int no;
    char user_id[13];
    int reg_date;
    unsigned char html_yn;
    char subject[51];
    char body[4096];
    unsigned short hits;
    int prev_no;
    int next_no;
    int updt_date;
    unsigned char use_yn;
    char file_name[51];
} NewsBbs;

/* CRUD Operations */
int NewsBbs_Add(DBConnectionManager* manager, const NewsBbs* bbs);
int NewsBbs_GetByNo(DBConnectionManager* manager, unsigned int no, NewsBbs* bbs);
int NewsBbs_Update(DBConnectionManager* manager, const NewsBbs* bbs);
int NewsBbs_Delete(DBConnectionManager* manager, unsigned int no);
int NewsBbs_Exists(DBConnectionManager* manager, unsigned int no);

/* Business Operations */
int NewsBbs_GetByBbsCode(DBConnectionManager* manager, unsigned char bbs_code,
                         NewsBbs* bbs_list, int max_count, int* actual_count);
int NewsBbs_GetAll(DBConnectionManager* manager, NewsBbs* bbs_list,
                   int max_count, int* actual_count);
int NewsBbs_IncrementHits(DBConnectionManager* manager, unsigned int no);
int NewsBbs_GetByUserId(DBConnectionManager* manager, const char* user_id,
                        NewsBbs* bbs_list, int max_count, int* actual_count);
int NewsBbs_SetPrevNext(DBConnectionManager* manager, unsigned int no,
                        int prev_no, int next_no);

/* Utility Functions */
void NewsBbs_PrintInfo(const NewsBbs* bbs);

#endif
