#ifndef GEN_FILES_ORM_H
#define GEN_FILES_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int gno;
    signed char gf_no;
    char file_server[51];
    char file_location[101];
} GenFiles;

/* CRUD Operations */
int GenFiles_Add(DBConnectionManager* manager, const GenFiles* record);
int GenFiles_Get(DBConnectionManager* manager, int gno, signed char gf_no, GenFiles* record);
int GenFiles_Update(DBConnectionManager* manager, const GenFiles* record);
int GenFiles_Delete(DBConnectionManager* manager, int gno, signed char gf_no);
int GenFiles_Exists(DBConnectionManager* manager, int gno, signed char gf_no);
int GenFiles_GetAll(DBConnectionManager* manager, GenFiles* records, int max_count, int* actual_count);

#endif /* GEN_FILES_ORM_H */