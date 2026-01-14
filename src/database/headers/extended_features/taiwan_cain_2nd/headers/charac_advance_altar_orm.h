#ifndef CHARAC_ADVANCE_ALTAR_ORM_H
#define CHARAC_ADVANCE_ALTAR_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int charac_no;
    int ridable_id;
    short ticket_free;
    short ticket_cera;
    int star_game;
    int star_cera;
    int star_usable;
    short survival_best;
    short star_reset_count;
    short is_unlock_stage_effect;
} CharacAdvanceAltar;

/* CRUD Operations */
int CharacAdvanceAltar_Add(DBConnectionManager* manager, const CharacAdvanceAltar* record);
int CharacAdvanceAltar_Get(DBConnectionManager* manager, int charac_no, int ridable_id, CharacAdvanceAltar* record);
int CharacAdvanceAltar_Update(DBConnectionManager* manager, const CharacAdvanceAltar* record);
int CharacAdvanceAltar_Delete(DBConnectionManager* manager, int charac_no, int ridable_id);
int CharacAdvanceAltar_Exists(DBConnectionManager* manager, int charac_no, int ridable_id);
int CharacAdvanceAltar_GetAll(DBConnectionManager* manager, CharacAdvanceAltar* records, int max_count, int* actual_count);

#endif /* CHARAC_ADVANCE_ALTAR_ORM_H */