#ifndef KNOWL_FILES_ORM_H
#define KNOWL_FILES_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int gno;
    signed char gf_no;
    char file_server[51];
    char file_location[101];
} KnowlFiles;

/* CRUD Operations */
int KnowlFiles_Add(DBConnectionManager* manager, const KnowlFiles* record);
int KnowlFiles_Get(DBConnectionManager* manager, int gno, signed char gf_no, KnowlFiles* record);
int KnowlFiles_Update(DBConnectionManager* manager, const KnowlFiles* record);
int KnowlFiles_Delete(DBConnectionManager* manager, int gno, signed char gf_no);
int KnowlFiles_Exists(DBConnectionManager* manager, int gno, signed char gf_no);
int KnowlFiles_GetAll(DBConnectionManager* manager, KnowlFiles* records, int max_count, int* actual_count);

#endif /* KNOWL_FILES_ORM_H */