#ifndef GM_MANIFEST_NOTUSE_ORM_H
#define GM_MANIFEST_NOTUSE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char level;
} GmManifestNotuse;

/* CRUD Operations */
int GmManifestNotuse_Add(DBConnectionManager* manager, const GmManifestNotuse* record);
int GmManifestNotuse_Get(DBConnectionManager* manager, int m_id, GmManifestNotuse* record);
int GmManifestNotuse_Update(DBConnectionManager* manager, const GmManifestNotuse* record);
int GmManifestNotuse_Delete(DBConnectionManager* manager, int m_id);
int GmManifestNotuse_Exists(DBConnectionManager* manager, int m_id);
int GmManifestNotuse_GetAll(DBConnectionManager* manager, GmManifestNotuse* records, int max_count, int* actual_count);

#endif /* GM_MANIFEST_NOTUSE_ORM_H */