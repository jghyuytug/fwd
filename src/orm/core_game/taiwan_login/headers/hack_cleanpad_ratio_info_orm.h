#ifndef HACK_CLEANPAD_RATIO_INFO_ORM_H
#define HACK_CLEANPAD_RATIO_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned short hack_type;
    unsigned int value;
    char reg_date[20];
} HackCleanpadRatioInfo;

/* CRUD Operations */
int HackCleanpadRatioInfo_Add(DBConnectionManager* manager, const HackCleanpadRatioInfo* info);
int HackCleanpadRatioInfo_Get(DBConnectionManager* manager, unsigned short hack_type, HackCleanpadRatioInfo* info);
int HackCleanpadRatioInfo_Update(DBConnectionManager* manager, const HackCleanpadRatioInfo* info);
int HackCleanpadRatioInfo_Delete(DBConnectionManager* manager, unsigned short hack_type);
int HackCleanpadRatioInfo_Exists(DBConnectionManager* manager, unsigned short hack_type);

/* Business Operations */
int HackCleanpadRatioInfo_GetAll(DBConnectionManager* manager,
                                  HackCleanpadRatioInfo* infos,
                                  int max_count,
                                  int* actual_count);
int HackCleanpadRatioInfo_GetCount(DBConnectionManager* manager, int* count);

/* Utility */
void HackCleanpadRatioInfo_PrintInfo(const HackCleanpadRatioInfo* info);

#endif
