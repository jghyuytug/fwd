#ifndef CHARAC_ADVANCE_ALTAR_ITEM_DESC_ORM_H
#define CHARAC_ADVANCE_ALTAR_ITEM_DESC_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int ridable_id;
    short item_type;
    int item_id;
} CharacAdvanceAltarItemDesc;

/* CRUD Operations */
int CharacAdvanceAltarItemDesc_Add(DBConnectionManager* manager, const CharacAdvanceAltarItemDesc* record);
int CharacAdvanceAltarItemDesc_Get(DBConnectionManager* manager, int ridable_id, short item_type, int item_id, CharacAdvanceAltarItemDesc* record);
int CharacAdvanceAltarItemDesc_Update(DBConnectionManager* manager, const CharacAdvanceAltarItemDesc* record);
int CharacAdvanceAltarItemDesc_Delete(DBConnectionManager* manager, int ridable_id, short item_type, int item_id);
int CharacAdvanceAltarItemDesc_Exists(DBConnectionManager* manager, int ridable_id, short item_type, int item_id);
int CharacAdvanceAltarItemDesc_GetAll(DBConnectionManager* manager, CharacAdvanceAltarItemDesc* records, int max_count, int* actual_count);

#endif /* CHARAC_ADVANCE_ALTAR_ITEM_DESC_ORM_H */