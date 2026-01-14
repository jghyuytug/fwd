#ifndef TCG_RULE_FILE_ORM_H
#define TCG_RULE_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} TcgRuleFile;

/* CRUD Operations */
int TcgRuleFile_Add(DBConnectionManager* manager, const TcgRuleFile* record);
int TcgRuleFile_Get(DBConnectionManager* manager, int no, TcgRuleFile* record);
int TcgRuleFile_Update(DBConnectionManager* manager, const TcgRuleFile* record);
int TcgRuleFile_Delete(DBConnectionManager* manager, int no);
int TcgRuleFile_Exists(DBConnectionManager* manager, int no);
int TcgRuleFile_GetAll(DBConnectionManager* manager, TcgRuleFile* records, int max_count, int* actual_count);

#endif /* TCG_RULE_FILE_ORM_H */