#ifndef TCG_CARD_REF_ORM_H
#define TCG_CARD_REF_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char act_info[11];
    char type[11];
    char card_name[26];
    short card_lev;
    char card_type[4];
    short card_rare;
    short card_num;
} TcgCardRef;

/* CRUD Operations */
int TcgCardRef_Add(DBConnectionManager* manager, const TcgCardRef* record);
int TcgCardRef_GetAll(DBConnectionManager* manager, TcgCardRef* records, int max_count, int* actual_count);

#endif /* TCG_CARD_REF_ORM_H */