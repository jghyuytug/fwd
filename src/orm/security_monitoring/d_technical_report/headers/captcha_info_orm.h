#ifndef CAPTCHA_INFO_ORM_H
#define CAPTCHA_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    char occ_time[20];
    int total_try_count;
    int success_count;
    int fail_count;
    int block_count;
    int incomplete_request_count;
    int invalid_request_count;
} CaptchaInfo;

/* CRUD Operations */
int CaptchaInfo_Add(DBConnectionManager* manager, const CaptchaInfo* record);
int CaptchaInfo_Get(DBConnectionManager* manager, int id, CaptchaInfo* record);
int CaptchaInfo_Update(DBConnectionManager* manager, const CaptchaInfo* record);
int CaptchaInfo_Delete(DBConnectionManager* manager, int id);
int CaptchaInfo_Exists(DBConnectionManager* manager, int id);
int CaptchaInfo_GetAll(DBConnectionManager* manager, CaptchaInfo* records, int max_count, int* actual_count);

#endif /* CAPTCHA_INFO_ORM_H */