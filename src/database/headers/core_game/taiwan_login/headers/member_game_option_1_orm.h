#ifndef MEMBER_GAME_OPTION_1_ORM_H
#define MEMBER_GAME_OPTION_1_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned char* data;
    unsigned int length;
} BlobData1;

typedef struct {
    int m_id;
    BlobData1 option_1;
    BlobData1 option_2;
    BlobData1 option_3;
} MemberGameOption1;

/* CRUD Operations */
int MemberGameOption1_Add(DBConnectionManager* manager, const MemberGameOption1* option);
int MemberGameOption1_Get(DBConnectionManager* manager, int m_id, MemberGameOption1* option);
int MemberGameOption1_Update(DBConnectionManager* manager, const MemberGameOption1* option);
int MemberGameOption1_Delete(DBConnectionManager* manager, int m_id);
int MemberGameOption1_Exists(DBConnectionManager* manager, int m_id);

/* Utility */
void MemberGameOption1_PrintInfo(const MemberGameOption1* option);
void MemberGameOption1_Free(MemberGameOption1* option);
void MemberGameOption1_Init(MemberGameOption1* option);

#endif
