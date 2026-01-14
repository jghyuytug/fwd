#ifndef POSTAL_ITEM_BASE_ORM_H
#define POSTAL_ITEM_BASE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int Id;
    signed char Target;
    char ItemName[256];
    int ItemCode;
    int ItemCount;
    int MaxCount;
    int Upgrade;
    signed char Best;
    signed char AmplifyOptionType;
    signed char AmplifyValue;
    int Gold;
    signed char SealFlag;
    int SeperateUpgrade;
    int Endurance;
    signed char PostalType;
    signed char IsEQU;
    char CreatedAt[20];
    long long Probability;
} PostalItemBase;

/* CRUD Operations */
int PostalItemBase_Add(DBConnectionManager* manager, const PostalItemBase* record);
int PostalItemBase_Get(DBConnectionManager* manager, int Id, PostalItemBase* record);
int PostalItemBase_Update(DBConnectionManager* manager, const PostalItemBase* record);
int PostalItemBase_Delete(DBConnectionManager* manager, int Id);
int PostalItemBase_Exists(DBConnectionManager* manager, int Id);
int PostalItemBase_GetAll(DBConnectionManager* manager, PostalItemBase* records, int max_count, int* actual_count);

#endif /* POSTAL_ITEM_BASE_ORM_H */