#ifndef INTERNET_LINE_ORM_H
#define INTERNET_LINE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char line_code;
    char line_name[11];
} InternetLine;

/* CRUD Operations */
int InternetLine_Add(DBConnectionManager* manager, const InternetLine* record);
int InternetLine_Get(DBConnectionManager* manager, signed char line_code, InternetLine* record);
int InternetLine_Update(DBConnectionManager* manager, const InternetLine* record);
int InternetLine_Delete(DBConnectionManager* manager, signed char line_code);
int InternetLine_Exists(DBConnectionManager* manager, signed char line_code);
int InternetLine_GetAll(DBConnectionManager* manager, InternetLine* records, int max_count, int* actual_count);

#endif /* INTERNET_LINE_ORM_H */