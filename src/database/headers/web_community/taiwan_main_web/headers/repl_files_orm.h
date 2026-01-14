#ifndef REPL_FILES_ORM_H
#define REPL_FILES_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int rno;
    int rf_no;
    char file_server[51];
    char file_location[101];
} ReplFiles;

/* CRUD Operations */
int ReplFiles_Add(DBConnectionManager* manager, const ReplFiles* record);
int ReplFiles_Get(DBConnectionManager* manager, int rno, int rf_no, ReplFiles* record);
int ReplFiles_Update(DBConnectionManager* manager, const ReplFiles* record);
int ReplFiles_Delete(DBConnectionManager* manager, int rno, int rf_no);
int ReplFiles_Exists(DBConnectionManager* manager, int rno, int rf_no);
int ReplFiles_GetAll(DBConnectionManager* manager, ReplFiles* records, int max_count, int* actual_count);

#endif /* REPL_FILES_ORM_H */