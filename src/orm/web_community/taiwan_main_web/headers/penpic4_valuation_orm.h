#ifndef PENPIC4_VALUATION_ORM_H
#define PENPIC4_VALUATION_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char penpic_desc[1024];
    char penpic_examine[1024];
} Penpic4Valuation;

/* CRUD Operations */
int Penpic4Valuation_Add(DBConnectionManager* manager, const Penpic4Valuation* record);
int Penpic4Valuation_Get(DBConnectionManager* manager, int no, Penpic4Valuation* record);
int Penpic4Valuation_Update(DBConnectionManager* manager, const Penpic4Valuation* record);
int Penpic4Valuation_Delete(DBConnectionManager* manager, int no);
int Penpic4Valuation_Exists(DBConnectionManager* manager, int no);
int Penpic4Valuation_GetAll(DBConnectionManager* manager, Penpic4Valuation* records, int max_count, int* actual_count);

#endif /* PENPIC4_VALUATION_ORM_H */