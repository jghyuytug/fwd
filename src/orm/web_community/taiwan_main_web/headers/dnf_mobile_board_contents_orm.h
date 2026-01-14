#ifndef DNF_MOBILE_BOARD_CONTENTS_ORM_H
#define DNF_MOBILE_BOARD_CONTENTS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int dmb_idx;
    char contents[1024];
} DnfMobileBoardContents;

/* CRUD Operations */
int DnfMobileBoardContents_Add(DBConnectionManager* manager, const DnfMobileBoardContents* record);
int DnfMobileBoardContents_Get(DBConnectionManager* manager, int dmb_idx, DnfMobileBoardContents* record);
int DnfMobileBoardContents_Update(DBConnectionManager* manager, const DnfMobileBoardContents* record);
int DnfMobileBoardContents_Delete(DBConnectionManager* manager, int dmb_idx);
int DnfMobileBoardContents_Exists(DBConnectionManager* manager, int dmb_idx);
int DnfMobileBoardContents_GetAll(DBConnectionManager* manager, DnfMobileBoardContents* records, int max_count, int* actual_count);

#endif /* DNF_MOBILE_BOARD_CONTENTS_ORM_H */