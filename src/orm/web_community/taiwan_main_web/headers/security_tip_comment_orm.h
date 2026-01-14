#ifndef SECURITY_TIP_COMMENT_ORM_H
#define SECURITY_TIP_COMMENT_ORM_H

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
} SecurityTipComment;

/* CRUD Operations */
int SecurityTipComment_Add(DBConnectionManager* manager, const SecurityTipComment* record);
int SecurityTipComment_Get(DBConnectionManager* manager, int no, SecurityTipComment* record);
int SecurityTipComment_Update(DBConnectionManager* manager, const SecurityTipComment* record);
int SecurityTipComment_Delete(DBConnectionManager* manager, int no);
int SecurityTipComment_Exists(DBConnectionManager* manager, int no);
int SecurityTipComment_GetAll(DBConnectionManager* manager, SecurityTipComment* records, int max_count, int* actual_count);

#endif /* SECURITY_TIP_COMMENT_ORM_H */