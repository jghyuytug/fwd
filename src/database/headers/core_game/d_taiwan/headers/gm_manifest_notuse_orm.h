#ifndef GM_MANIFEST_NOTUSE_ORM_H
#define GM_MANIFEST_NOTUSE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;                     /* Member ID (primary key) */
    unsigned char level;          /* GM level */
} GmManifestNotuse;

/* CRUD Operations */
int GmManifestNotuse_Add(DBConnectionManager* manager, const GmManifestNotuse* gm);
int GmManifestNotuse_Get(DBConnectionManager* manager, int m_id, GmManifestNotuse* gm);
int GmManifestNotuse_Update(DBConnectionManager* manager, const GmManifestNotuse* gm);
int GmManifestNotuse_Delete(DBConnectionManager* manager, int m_id);
int GmManifestNotuse_Exists(DBConnectionManager* manager, int m_id);

/* Business Operations */
int GmManifestNotuse_GetByLevel(DBConnectionManager* manager, unsigned char level,
                                 GmManifestNotuse* gms, int max_count, int* actual_count);
int GmManifestNotuse_GetAll(DBConnectionManager* manager, GmManifestNotuse* gms,
                             int max_count, int* actual_count);

/* Utility Functions */
void GmManifestNotuse_PrintInfo(const GmManifestNotuse* gm);

#endif
