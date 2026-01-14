#ifndef EVENT_USED_FATIGUE_AT_MAGE_ORM_H
#define EVENT_USED_FATIGUE_AT_MAGE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int charac_no;
    unsigned int fatigue_quantity;
} EventUsedFatigueAtMage;

/* CRUD Operations */
int EventUsedFatigueAtMage_Add(DBConnectionManager* manager, const EventUsedFatigueAtMage* event);
int EventUsedFatigueAtMage_Get(DBConnectionManager* manager, unsigned int charac_no,
                                EventUsedFatigueAtMage* event);
int EventUsedFatigueAtMage_Update(DBConnectionManager* manager, const EventUsedFatigueAtMage* event);
int EventUsedFatigueAtMage_Delete(DBConnectionManager* manager, unsigned int charac_no);
int EventUsedFatigueAtMage_Exists(DBConnectionManager* manager, unsigned int charac_no);

/* Business Operations */
int EventUsedFatigueAtMage_GetAll(DBConnectionManager* manager, EventUsedFatigueAtMage* events,
                                   int max_count, int* actual_count);
int EventUsedFatigueAtMage_GetByFatigueRange(DBConnectionManager* manager,
                                              unsigned int min_fatigue, unsigned int max_fatigue,
                                              EventUsedFatigueAtMage* events,
                                              int max_count, int* actual_count);
int EventUsedFatigueAtMage_IncreaseFatigue(DBConnectionManager* manager,
                                            unsigned int charac_no, unsigned int amount);
int EventUsedFatigueAtMage_GetTotalFatigue(DBConnectionManager* manager, unsigned long long* total);

/* Utility Functions */
void EventUsedFatigueAtMage_PrintInfo(const EventUsedFatigueAtMage* event);

#endif
