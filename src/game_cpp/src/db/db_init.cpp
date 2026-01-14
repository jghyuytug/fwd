#include "db/db_init.h"

#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>

#include "db/db_config.h"
#include "game_server_context.h"

static void close_mysql_handles(void) {
    if (g_db_account) {
        mysql_close(g_db_account);
        g_db_account = NULL;
    }
    if (g_db_charac) {
        mysql_close(g_db_charac);
        g_db_charac = NULL;
    }
    if (g_db_charac2) {
        mysql_close(g_db_charac2);
        g_db_charac2 = NULL;
    }
}

int db_init(const char* config_file) {
    printf("[DB] Initializing database connections...\n");

    unsigned int timeout = 5;

    /* Account database */
    g_db_account = mysql_init(NULL);
    if (!g_db_account) {
        printf("[DB] Failed to init account DB handle\n");
        return -1;
    }
    mysql_options(g_db_account, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&timeout);
    mysql_options(g_db_account, MYSQL_OPT_READ_TIMEOUT, (const char*)&timeout);
    mysql_options(g_db_account, MYSQL_OPT_WRITE_TIMEOUT, (const char*)&timeout);

    if (!mysql_real_connect(g_db_account, DB_HOST, DB_USER, DB_PASS,
                            DB_ACCOUNT, DB_PORT, NULL, 0)) {
        printf("[DB] Account DB connect failed: %s\n", mysql_error(g_db_account));
        close_mysql_handles();
        return -1;
    }

    /* Character database */
    g_db_charac = mysql_init(NULL);
    if (!g_db_charac) {
        printf("[DB] Failed to init charac DB handle\n");
        close_mysql_handles();
        return -1;
    }
    mysql_options(g_db_charac, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&timeout);
    mysql_options(g_db_charac, MYSQL_OPT_READ_TIMEOUT, (const char*)&timeout);
    mysql_options(g_db_charac, MYSQL_OPT_WRITE_TIMEOUT, (const char*)&timeout);

    if (!mysql_real_connect(g_db_charac, DB_HOST, DB_USER, DB_PASS,
                            DB_CHARAC, DB_PORT, NULL, 0)) {
        printf("[DB] Character DB connect failed: %s\n", mysql_error(g_db_charac));
        close_mysql_handles();
        return -1;
    }

    /* Inventory database */
    g_db_charac2 = mysql_init(NULL);
    if (!g_db_charac2) {
        printf("[DB] Failed to init charac2 DB handle\n");
        close_mysql_handles();
        return -1;
    }
    mysql_options(g_db_charac2, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&timeout);
    mysql_options(g_db_charac2, MYSQL_OPT_READ_TIMEOUT, (const char*)&timeout);
    mysql_options(g_db_charac2, MYSQL_OPT_WRITE_TIMEOUT, (const char*)&timeout);

    if (!mysql_real_connect(g_db_charac2, DB_HOST, DB_USER, DB_PASS,
                            DB_CHARAC2, DB_PORT, NULL, 0)) {
        printf("[DB] Charac2 DB connect failed: %s\n", mysql_error(g_db_charac2));
        close_mysql_handles();
        return -1;
    }

    mysql_set_character_set(g_db_account, "utf8");
    mysql_set_character_set(g_db_charac, "utf8");
    mysql_set_character_set(g_db_charac2, "utf8");

    printf("[DB] Database initialization complete\n");
    return 0;
}

void db_cleanup(void) {
    close_mysql_handles();
    DBConnectionManager_Cleanup(&g_db_manager);
    printf("[DB] Database connections closed\n");
}
