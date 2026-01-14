#ifndef WIKI_AGREEMENT_ORM_H
#define WIKI_AGREEMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    char url_go[21];
    char url_to[21];
    int reg_date;
    char subject[101];
    char content[1024];
} WikiAgreement;

/* CRUD Operations */
int WikiAgreement_Add(DBConnectionManager* manager, const WikiAgreement* record);
int WikiAgreement_Get(DBConnectionManager* manager, int id, WikiAgreement* record);
int WikiAgreement_Update(DBConnectionManager* manager, const WikiAgreement* record);
int WikiAgreement_Delete(DBConnectionManager* manager, int id);
int WikiAgreement_Exists(DBConnectionManager* manager, int id);
int WikiAgreement_GetAll(DBConnectionManager* manager, WikiAgreement* records, int max_count, int* actual_count);

#endif /* WIKI_AGREEMENT_ORM_H */