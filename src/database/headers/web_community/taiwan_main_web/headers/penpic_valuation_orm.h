#ifndef PENPIC_VALUATION_ORM_H
#define PENPIC_VALUATION_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char penpic_desc[1024];
    char penpic_examine[1024];
} PenpicValuation;

/* CRUD Operations */
int PenpicValuation_Add(DBConnectionManager* manager, const PenpicValuation* record);
int PenpicValuation_Get(DBConnectionManager* manager, int no, PenpicValuation* record);
int PenpicValuation_Update(DBConnectionManager* manager, const PenpicValuation* record);
int PenpicValuation_Delete(DBConnectionManager* manager, int no);
int PenpicValuation_Exists(DBConnectionManager* manager, int no);
int PenpicValuation_GetAll(DBConnectionManager* manager, PenpicValuation* records, int max_count, int* actual_count);

#endif /* PENPIC_VALUATION_ORM_H */