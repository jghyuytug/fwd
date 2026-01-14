#ifndef MAGAZINE_COMMENT_ORM_H
#define MAGAZINE_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int mz_id;
    int m_id;
    char m_sex;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    char comment_text[201];
    int create_day;
} MagazineComment;

/* CRUD Operations */
int MagazineComment_Add(DBConnectionManager* manager, const MagazineComment* record);
int MagazineComment_Get(DBConnectionManager* manager, int no, MagazineComment* record);
int MagazineComment_Update(DBConnectionManager* manager, const MagazineComment* record);
int MagazineComment_Delete(DBConnectionManager* manager, int no);
int MagazineComment_Exists(DBConnectionManager* manager, int no);
int MagazineComment_GetAll(DBConnectionManager* manager, MagazineComment* records, int max_count, int* actual_count);

#endif /* MAGAZINE_COMMENT_ORM_H */