#ifndef PENPIC2_VALUATION_ORM_H
#define PENPIC2_VALUATION_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char penpic_desc[1024];
    char penpic_examine[1024];
} Penpic2Valuation;

/* CRUD Operations */
int Penpic2Valuation_Add(DBConnectionManager* manager, const Penpic2Valuation* record);
int Penpic2Valuation_Get(DBConnectionManager* manager, int no, Penpic2Valuation* record);
int Penpic2Valuation_Update(DBConnectionManager* manager, const Penpic2Valuation* record);
int Penpic2Valuation_Delete(DBConnectionManager* manager, int no);
int Penpic2Valuation_Exists(DBConnectionManager* manager, int no);
int Penpic2Valuation_GetAll(DBConnectionManager* manager, Penpic2Valuation* records, int max_count, int* actual_count);

#endif /* PENPIC2_VALUATION_ORM_H */