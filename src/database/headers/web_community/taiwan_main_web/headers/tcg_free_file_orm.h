#ifndef TCG_FREE_FILE_ORM_H
#define TCG_FREE_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} TcgFreeFile;

/* CRUD Operations */
int TcgFreeFile_Add(DBConnectionManager* manager, const TcgFreeFile* record);
int TcgFreeFile_Get(DBConnectionManager* manager, int no, TcgFreeFile* record);
int TcgFreeFile_Update(DBConnectionManager* manager, const TcgFreeFile* record);
int TcgFreeFile_Delete(DBConnectionManager* manager, int no);
int TcgFreeFile_Exists(DBConnectionManager* manager, int no);
int TcgFreeFile_GetAll(DBConnectionManager* manager, TcgFreeFile* records, int max_count, int* actual_count);

#endif /* TCG_FREE_FILE_ORM_H */