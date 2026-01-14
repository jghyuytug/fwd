#ifndef REPL_RECOMM_ORM_H
#define REPL_RECOMM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int rno;
    int m_id;
    char type[2];
} ReplRecomm;

/* CRUD Operations */
int ReplRecomm_Add(DBConnectionManager* manager, const ReplRecomm* record);
int ReplRecomm_Get(DBConnectionManager* manager, int rno, int m_id, ReplRecomm* record);
int ReplRecomm_Update(DBConnectionManager* manager, const ReplRecomm* record);
int ReplRecomm_Delete(DBConnectionManager* manager, int rno, int m_id);
int ReplRecomm_Exists(DBConnectionManager* manager, int rno, int m_id);
int ReplRecomm_GetAll(DBConnectionManager* manager, ReplRecomm* records, int max_count, int* actual_count);

#endif /* REPL_RECOMM_ORM_H */