#ifndef MEMBER_GAME_OPTION_ORM_H
#define MEMBER_GAME_OPTION_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned char* data;
    unsigned int length;
} BlobData;

typedef struct {
    int m_id;
    BlobData option_1;
    BlobData option_2;
    BlobData option_3;
    BlobData shortcut_emoticon;
} MemberGameOption;

/* CRUD Operations */
int MemberGameOption_Add(DBConnectionManager* manager, const MemberGameOption* option);
int MemberGameOption_Get(DBConnectionManager* manager, int m_id, MemberGameOption* option);
int MemberGameOption_Update(DBConnectionManager* manager, const MemberGameOption* option);
int MemberGameOption_Delete(DBConnectionManager* manager, int m_id);
int MemberGameOption_Exists(DBConnectionManager* manager, int m_id);

/* Utility */
void MemberGameOption_PrintInfo(const MemberGameOption* option);
void MemberGameOption_Free(MemberGameOption* option);
void MemberGameOption_Init(MemberGameOption* option);

#endif
