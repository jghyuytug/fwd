#ifndef PENPIC_ENTRY_ORM_H
#define PENPIC_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char penpic_type;
    signed char cnt;
} PenpicEntry;

/* CRUD Operations */
int PenpicEntry_Add(DBConnectionManager* manager, const PenpicEntry* record);
int PenpicEntry_Get(DBConnectionManager* manager, int m_id, signed char penpic_type, PenpicEntry* record);
int PenpicEntry_Update(DBConnectionManager* manager, const PenpicEntry* record);
int PenpicEntry_Delete(DBConnectionManager* manager, int m_id, signed char penpic_type);
int PenpicEntry_Exists(DBConnectionManager* manager, int m_id, signed char penpic_type);
int PenpicEntry_GetAll(DBConnectionManager* manager, PenpicEntry* records, int max_count, int* actual_count);

#endif /* PENPIC_ENTRY_ORM_H */