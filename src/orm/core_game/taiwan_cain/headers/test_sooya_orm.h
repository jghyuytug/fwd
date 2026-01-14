#ifndef TEST_SOOYA_ORM_H
#define TEST_SOOYA_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int charac_no;
    unsigned char lev;
} TestSooya;

/* CRUD Operations */
int TestSooya_Add(DBConnectionManager* manager, const TestSooya* record);
int TestSooya_Get(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no, TestSooya* record);
int TestSooya_Update(DBConnectionManager* manager, const TestSooya* record);
int TestSooya_Delete(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no);
int TestSooya_Exists(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no);

/* Business Operations */
int TestSooya_Clear(DBConnectionManager* manager);
int TestSooya_UpdateLevel(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no, unsigned char new_lev);
int TestSooya_IncrementLevel(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no, unsigned char increment);
int TestSooya_Count(DBConnectionManager* manager);
int TestSooya_DeleteByMid(DBConnectionManager* manager, unsigned int m_id);

/* Utility */
void TestSooya_PrintInfo(const TestSooya* record);

#endif /* TEST_SOOYA_ORM_H */
