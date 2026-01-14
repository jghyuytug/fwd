#ifndef TCG_PUBLIC_INFO_FILE_ORM_H
#define TCG_PUBLIC_INFO_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} TcgPublicInfoFile;

/* CRUD Operations */
int TcgPublicInfoFile_Add(DBConnectionManager* manager, const TcgPublicInfoFile* record);
int TcgPublicInfoFile_Get(DBConnectionManager* manager, int no, TcgPublicInfoFile* record);
int TcgPublicInfoFile_Update(DBConnectionManager* manager, const TcgPublicInfoFile* record);
int TcgPublicInfoFile_Delete(DBConnectionManager* manager, int no);
int TcgPublicInfoFile_Exists(DBConnectionManager* manager, int no);
int TcgPublicInfoFile_GetAll(DBConnectionManager* manager, TcgPublicInfoFile* records, int max_count, int* actual_count);

#endif /* TCG_PUBLIC_INFO_FILE_ORM_H */