#ifndef TCG_RULE_COMMENT_ORM_H
#define TCG_RULE_COMMENT_ORM_H

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
} TcgRuleComment;

/* CRUD Operations */
int TcgRuleComment_Add(DBConnectionManager* manager, const TcgRuleComment* record);
int TcgRuleComment_Get(DBConnectionManager* manager, int no, TcgRuleComment* record);
int TcgRuleComment_Update(DBConnectionManager* manager, const TcgRuleComment* record);
int TcgRuleComment_Delete(DBConnectionManager* manager, int no);
int TcgRuleComment_Exists(DBConnectionManager* manager, int no);
int TcgRuleComment_GetAll(DBConnectionManager* manager, TcgRuleComment* records, int max_count, int* actual_count);

#endif /* TCG_RULE_COMMENT_ORM_H */