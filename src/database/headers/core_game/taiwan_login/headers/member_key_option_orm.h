#ifndef MEMBER_KEY_OPTION_ORM_H
#define MEMBER_KEY_OPTION_ORM_H

#include "db_connection_manager.h"
#include <stddef.h>

typedef struct {
    unsigned long long m_id;
    unsigned char key_type;
    unsigned char* key_option;
    size_t key_option_len;
} MemberKeyOption;

/* CRUD Operations */
int MemberKeyOption_Add(DBConnectionManager* manager, const MemberKeyOption* option);
int MemberKeyOption_Get(DBConnectionManager* manager, unsigned long long m_id,
                        unsigned char key_type, MemberKeyOption* option);
int MemberKeyOption_Update(DBConnectionManager* manager, const MemberKeyOption* option);
int MemberKeyOption_Delete(DBConnectionManager* manager, unsigned long long m_id,
                           unsigned char key_type);
int MemberKeyOption_Exists(DBConnectionManager* manager, unsigned long long m_id,
                           unsigned char key_type);

/* Business Operations */
int MemberKeyOption_GetByMemberId(DBConnectionManager* manager, unsigned long long m_id,
                                   MemberKeyOption* options, int max_count, int* actual_count);
int MemberKeyOption_DeleteByMemberId(DBConnectionManager* manager, unsigned long long m_id);

/* Utility Functions */
void MemberKeyOption_Free(MemberKeyOption* option);
void MemberKeyOption_FreeArray(MemberKeyOption* options, int count);
void MemberKeyOption_PrintInfo(const MemberKeyOption* option);

#endif
