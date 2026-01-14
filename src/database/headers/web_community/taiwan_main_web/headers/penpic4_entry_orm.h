#ifndef PENPIC4_ENTRY_ORM_H
#define PENPIC4_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char penpic_type;
    signed char cnt;
} Penpic4Entry;

/* CRUD Operations */
int Penpic4Entry_Add(DBConnectionManager* manager, const Penpic4Entry* record);
int Penpic4Entry_Get(DBConnectionManager* manager, int m_id, signed char penpic_type, Penpic4Entry* record);
int Penpic4Entry_Update(DBConnectionManager* manager, const Penpic4Entry* record);
int Penpic4Entry_Delete(DBConnectionManager* manager, int m_id, signed char penpic_type);
int Penpic4Entry_Exists(DBConnectionManager* manager, int m_id, signed char penpic_type);
int Penpic4Entry_GetAll(DBConnectionManager* manager, Penpic4Entry* records, int max_count, int* actual_count);

#endif /* PENPIC4_ENTRY_ORM_H */