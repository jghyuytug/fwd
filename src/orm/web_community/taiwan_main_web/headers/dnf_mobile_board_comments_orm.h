#ifndef DNF_MOBILE_BOARD_COMMENTS_ORM_H
#define DNF_MOBILE_BOARD_COMMENTS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int dmb_idx;
    char writer_name[21];
    char comments[256];
    int occ_time;
} DnfMobileBoardComments;

/* CRUD Operations */
int DnfMobileBoardComments_Add(DBConnectionManager* manager, const DnfMobileBoardComments* record);
int DnfMobileBoardComments_Get(DBConnectionManager* manager, int no, DnfMobileBoardComments* record);
int DnfMobileBoardComments_Update(DBConnectionManager* manager, const DnfMobileBoardComments* record);
int DnfMobileBoardComments_Delete(DBConnectionManager* manager, int no);
int DnfMobileBoardComments_Exists(DBConnectionManager* manager, int no);
int DnfMobileBoardComments_GetAll(DBConnectionManager* manager, DnfMobileBoardComments* records, int max_count, int* actual_count);

#endif /* DNF_MOBILE_BOARD_COMMENTS_ORM_H */