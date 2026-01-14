#ifndef AVATAR_FIXED_HIDDENOPTION_LIST_ORM_H
#define AVATAR_FIXED_HIDDENOPTION_LIST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    short Id;
    signed char Type;
    char Name[256];
    float MinValue;
    float MaxValue;
} AvatarFixedHiddenoptionList;

/* CRUD Operations */
int AvatarFixedHiddenoptionList_Add(DBConnectionManager* manager, const AvatarFixedHiddenoptionList* record);
int AvatarFixedHiddenoptionList_Get(DBConnectionManager* manager, short Id, signed char Type, AvatarFixedHiddenoptionList* record);
int AvatarFixedHiddenoptionList_Update(DBConnectionManager* manager, const AvatarFixedHiddenoptionList* record);
int AvatarFixedHiddenoptionList_Delete(DBConnectionManager* manager, short Id, signed char Type);
int AvatarFixedHiddenoptionList_Exists(DBConnectionManager* manager, short Id, signed char Type);
int AvatarFixedHiddenoptionList_GetAll(DBConnectionManager* manager, AvatarFixedHiddenoptionList* records, int max_count, int* actual_count);

#endif /* AVATAR_FIXED_HIDDENOPTION_LIST_ORM_H */