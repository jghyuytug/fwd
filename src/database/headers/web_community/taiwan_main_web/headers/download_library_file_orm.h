#ifndef DOWNLOAD_LIBRARY_FILE_ORM_H
#define DOWNLOAD_LIBRARY_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int file_no;
    int no;
    char file_info[10];
    char file_url[256];
} DownloadLibraryFile;

/* CRUD Operations */
int DownloadLibraryFile_Add(DBConnectionManager* manager, const DownloadLibraryFile* record);
int DownloadLibraryFile_Get(DBConnectionManager* manager, int file_no, DownloadLibraryFile* record);
int DownloadLibraryFile_Update(DBConnectionManager* manager, const DownloadLibraryFile* record);
int DownloadLibraryFile_Delete(DBConnectionManager* manager, int file_no);
int DownloadLibraryFile_Exists(DBConnectionManager* manager, int file_no);
int DownloadLibraryFile_GetAll(DBConnectionManager* manager, DownloadLibraryFile* records, int max_count, int* actual_count);

#endif /* DOWNLOAD_LIBRARY_FILE_ORM_H */