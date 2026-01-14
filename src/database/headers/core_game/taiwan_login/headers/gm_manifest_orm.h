#ifndef GM_MANIFEST_ORM_H
#define GM_MANIFEST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    unsigned char level;
} GmManifest;

/* CRUD Operations */
int GmManifest_Add(DBConnectionManager* manager, const GmManifest* gm);
int GmManifest_Get(DBConnectionManager* manager, int m_id, GmManifest* gm);
int GmManifest_Update(DBConnectionManager* manager, const GmManifest* gm);
int GmManifest_Delete(DBConnectionManager* manager, int m_id);
int GmManifest_Exists(DBConnectionManager* manager, int m_id);

/* Business Operations */
int GmManifest_GetAll(DBConnectionManager* manager,
                      GmManifest* gms,
                      int max_count,
                      int* actual_count);
int GmManifest_GetByLevel(DBConnectionManager* manager,
                          unsigned char level,
                          GmManifest* gms,
                          int max_count,
                          int* actual_count);
int GmManifest_GetCount(DBConnectionManager* manager, int* count);

/* Utility */
void GmManifest_PrintInfo(const GmManifest* gm);

#endif
