#ifndef TCG_GRAND_FILE_ORM_H
#define TCG_GRAND_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} TcgGrandFile;

/* CRUD Operations */
int TcgGrandFile_Add(DBConnectionManager* manager, const TcgGrandFile* record);
int TcgGrandFile_Get(DBConnectionManager* manager, int no, TcgGrandFile* record);
int TcgGrandFile_Update(DBConnectionManager* manager, const TcgGrandFile* record);
int TcgGrandFile_Delete(DBConnectionManager* manager, int no);
int TcgGrandFile_Exists(DBConnectionManager* manager, int no);
int TcgGrandFile_GetAll(DBConnectionManager* manager, TcgGrandFile* records, int max_count, int* actual_count);

#endif /* TCG_GRAND_FILE_ORM_H */