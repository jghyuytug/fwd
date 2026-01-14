#ifndef PENPIC3_VALUATION_ORM_H
#define PENPIC3_VALUATION_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char penpic_desc[1024];
    char penpic_examine[1024];
} Penpic3Valuation;

/* CRUD Operations */
int Penpic3Valuation_Add(DBConnectionManager* manager, const Penpic3Valuation* record);
int Penpic3Valuation_Get(DBConnectionManager* manager, int no, Penpic3Valuation* record);
int Penpic3Valuation_Update(DBConnectionManager* manager, const Penpic3Valuation* record);
int Penpic3Valuation_Delete(DBConnectionManager* manager, int no);
int Penpic3Valuation_Exists(DBConnectionManager* manager, int no);
int Penpic3Valuation_GetAll(DBConnectionManager* manager, Penpic3Valuation* records, int max_count, int* actual_count);

#endif /* PENPIC3_VALUATION_ORM_H */