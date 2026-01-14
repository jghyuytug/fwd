#ifndef DOWNLOAD_LIBRARY_CONTENT_ORM_H
#define DOWNLOAD_LIBRARY_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
} DownloadLibraryContent;

/* CRUD Operations */
int DownloadLibraryContent_Add(DBConnectionManager* manager, const DownloadLibraryContent* record);
int DownloadLibraryContent_Get(DBConnectionManager* manager, int no, DownloadLibraryContent* record);
int DownloadLibraryContent_Update(DBConnectionManager* manager, const DownloadLibraryContent* record);
int DownloadLibraryContent_Delete(DBConnectionManager* manager, int no);
int DownloadLibraryContent_Exists(DBConnectionManager* manager, int no);
int DownloadLibraryContent_GetAll(DBConnectionManager* manager, DownloadLibraryContent* records, int max_count, int* actual_count);

#endif /* DOWNLOAD_LIBRARY_CONTENT_ORM_H */