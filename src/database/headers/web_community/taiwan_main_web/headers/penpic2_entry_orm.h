#ifndef PENPIC2_ENTRY_ORM_H
#define PENPIC2_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char penpic_type;
    signed char cnt;
} Penpic2Entry;

/* CRUD Operations */
int Penpic2Entry_Add(DBConnectionManager* manager, const Penpic2Entry* record);
int Penpic2Entry_Get(DBConnectionManager* manager, int m_id, signed char penpic_type, Penpic2Entry* record);
int Penpic2Entry_Update(DBConnectionManager* manager, const Penpic2Entry* record);
int Penpic2Entry_Delete(DBConnectionManager* manager, int m_id, signed char penpic_type);
int Penpic2Entry_Exists(DBConnectionManager* manager, int m_id, signed char penpic_type);
int Penpic2Entry_GetAll(DBConnectionManager* manager, Penpic2Entry* records, int max_count, int* actual_count);

#endif /* PENPIC2_ENTRY_ORM_H */