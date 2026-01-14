#ifndef NEWMAGAZINE_RECOMM_ORM_H
#define NEWMAGAZINE_RECOMM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int mz_id;
} NewmagazineRecomm;

/* CRUD Operations */
int NewmagazineRecomm_Add(DBConnectionManager* manager, const NewmagazineRecomm* record);
int NewmagazineRecomm_Get(DBConnectionManager* manager, int m_id, int mz_id, NewmagazineRecomm* record);
int NewmagazineRecomm_Update(DBConnectionManager* manager, const NewmagazineRecomm* record);
int NewmagazineRecomm_Delete(DBConnectionManager* manager, int m_id, int mz_id);
int NewmagazineRecomm_Exists(DBConnectionManager* manager, int m_id, int mz_id);
int NewmagazineRecomm_GetAll(DBConnectionManager* manager, NewmagazineRecomm* records, int max_count, int* actual_count);

#endif /* NEWMAGAZINE_RECOMM_ORM_H */