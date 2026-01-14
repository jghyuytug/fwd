#ifndef CAPTCHA_INVALID_REQUEST_ORM_H
#define CAPTCHA_INVALID_REQUEST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    int m_id;
    short request_type;
    char occ_time[20];
} CaptchaInvalidRequest;

/* CRUD Operations */
int CaptchaInvalidRequest_Add(DBConnectionManager* manager, const CaptchaInvalidRequest* record);
int CaptchaInvalidRequest_Get(DBConnectionManager* manager, int id, CaptchaInvalidRequest* record);
int CaptchaInvalidRequest_Update(DBConnectionManager* manager, const CaptchaInvalidRequest* record);
int CaptchaInvalidRequest_Delete(DBConnectionManager* manager, int id);
int CaptchaInvalidRequest_Exists(DBConnectionManager* manager, int id);
int CaptchaInvalidRequest_GetAll(DBConnectionManager* manager, CaptchaInvalidRequest* records, int max_count, int* actual_count);

#endif /* CAPTCHA_INVALID_REQUEST_ORM_H */