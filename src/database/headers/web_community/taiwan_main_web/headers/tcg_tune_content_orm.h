#ifndef TCG_TUNE_CONTENT_ORM_H
#define TCG_TUNE_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} TcgTuneContent;

/* CRUD Operations */
int TcgTuneContent_Add(DBConnectionManager* manager, const TcgTuneContent* record);
int TcgTuneContent_Get(DBConnectionManager* manager, int no, TcgTuneContent* record);
int TcgTuneContent_Update(DBConnectionManager* manager, const TcgTuneContent* record);
int TcgTuneContent_Delete(DBConnectionManager* manager, int no);
int TcgTuneContent_Exists(DBConnectionManager* manager, int no);
int TcgTuneContent_GetAll(DBConnectionManager* manager, TcgTuneContent* records, int max_count, int* actual_count);

#endif /* TCG_TUNE_CONTENT_ORM_H */