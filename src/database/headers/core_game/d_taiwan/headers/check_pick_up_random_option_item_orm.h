#ifndef CHECK_PICK_UP_RANDOM_OPTION_ITEM_ORM_H
#define CHECK_PICK_UP_RANDOM_OPTION_ITEM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int m_id;
    unsigned char check_count;
} CheckPickUpRandomOptionItem;

/* CRUD Operations */
int CheckPickUpRandomOptionItem_Add(DBConnectionManager* manager, const CheckPickUpRandomOptionItem* item);
int CheckPickUpRandomOptionItem_Get(DBConnectionManager* manager, unsigned int m_id, CheckPickUpRandomOptionItem* item);
int CheckPickUpRandomOptionItem_Update(DBConnectionManager* manager, const CheckPickUpRandomOptionItem* item);
int CheckPickUpRandomOptionItem_Delete(DBConnectionManager* manager, unsigned int m_id);
int CheckPickUpRandomOptionItem_Exists(DBConnectionManager* manager, unsigned int m_id);

/* Specialized Operations */
int CheckPickUpRandomOptionItem_IncrementCount(DBConnectionManager* manager, unsigned int m_id);
int CheckPickUpRandomOptionItem_ResetCount(DBConnectionManager* manager, unsigned int m_id);
int CheckPickUpRandomOptionItem_UpdateCount(DBConnectionManager* manager, unsigned int m_id, unsigned char new_count);

/* Query Operations */
int CheckPickUpRandomOptionItem_GetAll(DBConnectionManager* manager, CheckPickUpRandomOptionItem* items,
                                        int max_count, int* actual_count);
int CheckPickUpRandomOptionItem_GetByCountRange(DBConnectionManager* manager, unsigned char min_count,
                                                  unsigned char max_count, CheckPickUpRandomOptionItem* items,
                                                  int max_items, int* actual_count);
int CheckPickUpRandomOptionItem_CountAll(DBConnectionManager* manager);
int CheckPickUpRandomOptionItem_CountByCount(DBConnectionManager* manager, unsigned char check_count);

/* Batch Operations */
int CheckPickUpRandomOptionItem_BatchDelete(DBConnectionManager* manager, const unsigned int* m_ids, int count);

/* Utility Functions */
void CheckPickUpRandomOptionItem_PrintInfo(const CheckPickUpRandomOptionItem* item);

#endif /* CHECK_PICK_UP_RANDOM_OPTION_ITEM_ORM_H */
