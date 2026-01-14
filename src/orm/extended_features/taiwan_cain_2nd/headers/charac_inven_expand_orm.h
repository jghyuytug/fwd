#ifndef CHARAC_INVEN_EXPAND_ORM_H
#define CHARAC_INVEN_EXPAND_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int charac_no;
    int cargo_capacity;
    char current_equipslot[2];
} CharacInvenExpand;

/* CRUD Operations */
int CharacInvenExpand_Add(DBConnectionManager* manager, const CharacInvenExpand* record);
int CharacInvenExpand_Get(DBConnectionManager* manager, int charac_no, CharacInvenExpand* record);
int CharacInvenExpand_Update(DBConnectionManager* manager, const CharacInvenExpand* record);
int CharacInvenExpand_Delete(DBConnectionManager* manager, int charac_no);
int CharacInvenExpand_Exists(DBConnectionManager* manager, int charac_no);
int CharacInvenExpand_GetAll(DBConnectionManager* manager, CharacInvenExpand* records, int max_count, int* actual_count);

#endif /* CHARAC_INVEN_EXPAND_ORM_H */