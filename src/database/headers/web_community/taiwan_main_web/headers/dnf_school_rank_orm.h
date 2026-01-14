#ifndef DNF_SCHOOL_RANK_ORM_H
#define DNF_SCHOOL_RANK_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char category;
    signed char grade;
    signed char region;
    int school_id;
    char name[71];
    long long point;
    int rank;
} DnfSchoolRank;

/* CRUD Operations */
int DnfSchoolRank_Add(DBConnectionManager* manager, const DnfSchoolRank* record);
int DnfSchoolRank_Get(DBConnectionManager* manager, signed char category, signed char grade, signed char region, int school_id, DnfSchoolRank* record);
int DnfSchoolRank_Update(DBConnectionManager* manager, const DnfSchoolRank* record);
int DnfSchoolRank_Delete(DBConnectionManager* manager, signed char category, signed char grade, signed char region, int school_id);
int DnfSchoolRank_Exists(DBConnectionManager* manager, signed char category, signed char grade, signed char region, int school_id);
int DnfSchoolRank_GetAll(DBConnectionManager* manager, DnfSchoolRank* records, int max_count, int* actual_count);

#endif /* DNF_SCHOOL_RANK_ORM_H */