#ifndef LOG_GROWTH_ORM_H
#define LOG_GROWTH_ORM_H

#include "db_connection_manager.h"

/**
 * LogGrowth - Character growth log record
 * Maps to taiwan_login.log_growth table
 * Primary Key: (m_id, server_info, charac_no) - 3-field composite key
 */
typedef struct {
    int m_id;
    unsigned char server_info;
    int charac_no;
    char charac_name[26];      /* VARCHAR(25) */
    unsigned char job;
    unsigned char grow_type;
    char occ_time[20];         /* DATETIME */
} LogGrowth;

/* ========== CRUD Operations (Composite Primary Key) ========== */

/**
 * Add a new log_growth record
 * @return 0 on success, -1 on failure
 */
int LogGrowth_Add(DBConnectionManager* manager, const LogGrowth* record);

/**
 * Get a log_growth record by composite primary key
 * @param m_id Member ID (key 1/3)
 * @param server_info Server info (key 2/3)
 * @param charac_no Character number (key 3/3)
 * @return 0 on success, -1 on failure
 */
int LogGrowth_Get(DBConnectionManager* manager,
                   int m_id,
                   unsigned char server_info,
                   int charac_no,
                   LogGrowth* record);

/**
 * Update a log_growth record
 * @return 0 on success, -1 on failure
 */
int LogGrowth_Update(DBConnectionManager* manager, const LogGrowth* record);

/**
 * Delete a log_growth record by composite primary key
 * @param m_id Member ID (key 1/3)
 * @param server_info Server info (key 2/3)
 * @param charac_no Character number (key 3/3)
 * @return 0 on success, -1 on failure
 */
int LogGrowth_Delete(DBConnectionManager* manager,
                      int m_id,
                      unsigned char server_info,
                      int charac_no);

/**
 * Check if a log_growth record exists
 * @param m_id Member ID (key 1/3)
 * @param server_info Server info (key 2/3)
 * @param charac_no Character number (key 3/3)
 * @return 1 if exists, 0 if not exists, -1 on error
 */
int LogGrowth_Exists(DBConnectionManager* manager,
                      int m_id,
                      unsigned char server_info,
                      int charac_no);

/* ========== Business Operations ========== */

/**
 * Get all growth logs for a specific member
 * @param m_id Member ID
 * @param records Output array
 * @param max_count Maximum records to fetch
 * @param actual_count Actual number of records fetched
 * @return 0 on success, -1 on failure
 */
int LogGrowth_GetByMemberId(DBConnectionManager* manager,
                             int m_id,
                             LogGrowth* records,
                             int max_count,
                             int* actual_count);

/**
 * Get all growth logs for a specific character
 * @param m_id Member ID
 * @param server_info Server info
 * @param charac_no Character number
 * @param records Output array
 * @param max_count Maximum records to fetch
 * @param actual_count Actual number of records fetched
 * @return 0 on success, -1 on failure
 */
int LogGrowth_GetByCharacter(DBConnectionManager* manager,
                               int m_id,
                               unsigned char server_info,
                               int charac_no,
                               LogGrowth* records,
                               int max_count,
                               int* actual_count);

/**
 * Get growth logs by job class
 * @param job Job class ID
 * @param records Output array
 * @param max_count Maximum records to fetch
 * @param actual_count Actual number of records fetched
 * @return 0 on success, -1 on failure
 */
int LogGrowth_GetByJob(DBConnectionManager* manager,
                        unsigned char job,
                        LogGrowth* records,
                        int max_count,
                        int* actual_count);

/**
 * Get growth logs by grow type
 * @param grow_type Grow type ID
 * @param records Output array
 * @param max_count Maximum records to fetch
 * @param actual_count Actual number of records fetched
 * @return 0 on success, -1 on failure
 */
int LogGrowth_GetByGrowType(DBConnectionManager* manager,
                              unsigned char grow_type,
                              LogGrowth* records,
                              int max_count,
                              int* actual_count);

/* ========== Utility Functions ========== */

/**
 * Print log_growth record information
 */
void LogGrowth_PrintInfo(const LogGrowth* record);

#endif /* LOG_GROWTH_ORM_H */
