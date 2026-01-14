#ifndef TCG_RULE_CONTENT_ORM_H
#define TCG_RULE_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} TcgRuleContent;

/* CRUD Operations */
int TcgRuleContent_Add(DBConnectionManager* manager, const TcgRuleContent* record);
int TcgRuleContent_Get(DBConnectionManager* manager, int no, TcgRuleContent* record);
int TcgRuleContent_Update(DBConnectionManager* manager, const TcgRuleContent* record);
int TcgRuleContent_Delete(DBConnectionManager* manager, int no);
int TcgRuleContent_Exists(DBConnectionManager* manager, int no);
int TcgRuleContent_GetAll(DBConnectionManager* manager, TcgRuleContent* records, int max_count, int* actual_count);

#endif /* TCG_RULE_CONTENT_ORM_H */