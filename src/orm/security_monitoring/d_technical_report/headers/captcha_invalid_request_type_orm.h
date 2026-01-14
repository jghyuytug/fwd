#ifndef CAPTCHA_INVALID_REQUEST_TYPE_ORM_H
#define CAPTCHA_INVALID_REQUEST_TYPE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    short type;
    char type_desc[21];
} CaptchaInvalidRequestType;

/* CRUD Operations */
int CaptchaInvalidRequestType_Add(DBConnectionManager* manager, const CaptchaInvalidRequestType* record);
int CaptchaInvalidRequestType_Get(DBConnectionManager* manager, short type, CaptchaInvalidRequestType* record);
int CaptchaInvalidRequestType_Update(DBConnectionManager* manager, const CaptchaInvalidRequestType* record);
int CaptchaInvalidRequestType_Delete(DBConnectionManager* manager, short type);
int CaptchaInvalidRequestType_Exists(DBConnectionManager* manager, short type);
int CaptchaInvalidRequestType_GetAll(DBConnectionManager* manager, CaptchaInvalidRequestType* records, int max_count, int* actual_count);

#endif /* CAPTCHA_INVALID_REQUEST_TYPE_ORM_H */