#ifndef SECURITY_TIP_CONTENT_ORM_H
#define SECURITY_TIP_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int mz_id;
    char content[4096];
} SecurityTipContent;

/* CRUD Operations */
int SecurityTipContent_Add(DBConnectionManager* manager, const SecurityTipContent* record);
int SecurityTipContent_Get(DBConnectionManager* manager, int mz_id, SecurityTipContent* record);
int SecurityTipContent_Update(DBConnectionManager* manager, const SecurityTipContent* record);
int SecurityTipContent_Delete(DBConnectionManager* manager, int mz_id);
int SecurityTipContent_Exists(DBConnectionManager* manager, int mz_id);
int SecurityTipContent_GetAll(DBConnectionManager* manager, SecurityTipContent* records, int max_count, int* actual_count);

#endif /* SECURITY_TIP_CONTENT_ORM_H */