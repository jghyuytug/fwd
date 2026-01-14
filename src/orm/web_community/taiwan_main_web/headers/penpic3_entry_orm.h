#ifndef PENPIC3_ENTRY_ORM_H
#define PENPIC3_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char penpic_type;
    signed char cnt;
} Penpic3Entry;

/* CRUD Operations */
int Penpic3Entry_Add(DBConnectionManager* manager, const Penpic3Entry* record);
int Penpic3Entry_Get(DBConnectionManager* manager, int m_id, signed char penpic_type, Penpic3Entry* record);
int Penpic3Entry_Update(DBConnectionManager* manager, const Penpic3Entry* record);
int Penpic3Entry_Delete(DBConnectionManager* manager, int m_id, signed char penpic_type);
int Penpic3Entry_Exists(DBConnectionManager* manager, int m_id, signed char penpic_type);
int Penpic3Entry_GetAll(DBConnectionManager* manager, Penpic3Entry* records, int max_count, int* actual_count);

#endif /* PENPIC3_ENTRY_ORM_H */