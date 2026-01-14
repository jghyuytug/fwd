#ifndef DOWNLOAD_LIBRARY_ORM_H
#define DOWNLOAD_LIBRARY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    int m_id;
    char m_user_id[17];
    signed char open;
    signed char main;
    char title[121];
    int create_day;
    int view;
    int download;
    char img_link[256];
} DownloadLibrary;

/* CRUD Operations */
int DownloadLibrary_Add(DBConnectionManager* manager, const DownloadLibrary* record);
int DownloadLibrary_Get(DBConnectionManager* manager, int no, DownloadLibrary* record);
int DownloadLibrary_Update(DBConnectionManager* manager, const DownloadLibrary* record);
int DownloadLibrary_Delete(DBConnectionManager* manager, int no);
int DownloadLibrary_Exists(DBConnectionManager* manager, int no);
int DownloadLibrary_GetAll(DBConnectionManager* manager, DownloadLibrary* records, int max_count, int* actual_count);

#endif /* DOWNLOAD_LIBRARY_ORM_H */