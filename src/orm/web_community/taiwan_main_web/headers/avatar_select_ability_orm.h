#ifndef AVATAR_SELECT_ABILITY_ORM_H
#define AVATAR_SELECT_ABILITY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int it_no;
    int ability_no;
    signed char ability_type;
    signed char rate_change;
    float value;
    signed char job;
    int skill_index;
    int skill_level;
} AvatarSelectAbility;

/* CRUD Operations */
int AvatarSelectAbility_Add(DBConnectionManager* manager, const AvatarSelectAbility* record);
int AvatarSelectAbility_Get(DBConnectionManager* manager, int it_no, int ability_no, AvatarSelectAbility* record);
int AvatarSelectAbility_Update(DBConnectionManager* manager, const AvatarSelectAbility* record);
int AvatarSelectAbility_Delete(DBConnectionManager* manager, int it_no, int ability_no);
int AvatarSelectAbility_Exists(DBConnectionManager* manager, int it_no, int ability_no);
int AvatarSelectAbility_GetAll(DBConnectionManager* manager, AvatarSelectAbility* records, int max_count, int* actual_count);

#endif /* AVATAR_SELECT_ABILITY_ORM_H */