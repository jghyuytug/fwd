/**
 * @file mysql_stubs.h
 * @brief Minimal MySQL C library type stubs for compilation
 *
 * This file provides forward declarations and minimal type definitions
 * for MySQL C library types to allow the MySQL wrapper class to compile
 * without requiring the full MySQL development libraries.
 *
 * For production use, link against real libmysqlclient.
 */

#ifndef MYSQL_STUBS_H
#define MYSQL_STUBS_H

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations of MySQL C library types
typedef struct st_mysql MYSQL;
typedef struct st_mysql_res MYSQL_RES;
typedef char** MYSQL_ROW;
typedef struct st_mysql_field MYSQL_FIELD;

// MySQL option enums (subset)
enum mysql_option {
    MYSQL_OPT_COMPRESS = 1,
    MYSQL_OPT_NAMED_PIPE = 2,
    MYSQL_OPT_PROTOCOL = 9,
    MYSQL_READ_DEFAULT_GROUP = 5,
    MYSQL_OPT_RECONNECT = 20
};

// Function stubs for MySQL C API
// These will need to be either:
// 1. Linked against real libmysqlclient, OR
// 2. Implemented as test mocks

MYSQL* mysql_init(MYSQL* mysql);
MYSQL* mysql_real_connect(MYSQL* mysql, const char* host, const char* user,
                          const char* passwd, const char* db,
                          unsigned int port, const char* unix_socket,
                          unsigned long clientflag);
int mysql_options(MYSQL* mysql, enum mysql_option option, const void* arg);
void mysql_close(MYSQL* mysql);
int mysql_ping(MYSQL* mysql);
int mysql_real_query(MYSQL* mysql, const char* stmt_str, unsigned long length);
int mysql_query(MYSQL* mysql, const char* stmt_str);
MYSQL_RES* mysql_store_result(MYSQL* mysql);
MYSQL_RES* mysql_use_result(MYSQL* mysql);
void mysql_free_result(MYSQL_RES* result);
MYSQL_ROW mysql_fetch_row(MYSQL_RES* result);
unsigned long* mysql_fetch_lengths(MYSQL_RES* result);
unsigned int mysql_num_fields(MYSQL_RES* result);
unsigned long long mysql_num_rows(MYSQL_RES* result);
MYSQL_FIELD* mysql_fetch_fields(MYSQL_RES* result);
MYSQL_FIELD* mysql_fetch_field_direct(MYSQL_RES* result, unsigned int fieldnr);
unsigned long long mysql_affected_rows(MYSQL* mysql);
const char* mysql_error(MYSQL* mysql);
unsigned int mysql_errno(MYSQL* mysql);
const char* mysql_get_server_info(MYSQL* mysql);
unsigned long mysql_get_server_version(MYSQL* mysql);
void mysql_data_seek(MYSQL_RES* result, unsigned long long offset);
MYSQL_ROW mysql_fetch_row(MYSQL_RES* result);
int mysql_set_character_set(MYSQL* mysql, const char* csname);

#ifdef __cplusplus
}
#endif

#endif // MYSQL_STUBS_H
