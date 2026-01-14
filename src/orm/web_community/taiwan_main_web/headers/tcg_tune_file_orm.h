#ifndef TCG_TUNE_FILE_ORM_H
#define TCG_TUNE_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} TcgTuneFile;

/* CRUD Operations */
int TcgTuneFile_Add(DBConnectionManager* manager, const TcgTuneFile* record);
int TcgTuneFile_Get(DBConnectionManager* manager, int no, TcgTuneFile* record);
int TcgTuneFile_Update(DBConnectionManager* manager, const TcgTuneFile* record);
int TcgTuneFile_Delete(DBConnectionManager* manager, int no);
int TcgTuneFile_Exists(DBConnectionManager* manager, int no);
int TcgTuneFile_GetAll(DBConnectionManager* manager, TcgTuneFile* records, int max_count, int* actual_count);

#endif /* TCG_TUNE_FILE_ORM_H */