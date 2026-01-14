#ifndef UCC_PUBLIC_INFO_FILE_ORM_H
#define UCC_PUBLIC_INFO_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} UccPublicInfoFile;

/* CRUD Operations */
int UccPublicInfoFile_Add(DBConnectionManager* manager, const UccPublicInfoFile* record);
int UccPublicInfoFile_Get(DBConnectionManager* manager, int no, UccPublicInfoFile* record);
int UccPublicInfoFile_Update(DBConnectionManager* manager, const UccPublicInfoFile* record);
int UccPublicInfoFile_Delete(DBConnectionManager* manager, int no);
int UccPublicInfoFile_Exists(DBConnectionManager* manager, int no);
int UccPublicInfoFile_GetAll(DBConnectionManager* manager, UccPublicInfoFile* records, int max_count, int* actual_count);

#endif /* UCC_PUBLIC_INFO_FILE_ORM_H */