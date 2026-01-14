#include "event_reserving_charac_name_20130328_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test functions */
void test_basic_crud(DBConnectionManager* manager);

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize connection manager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&manager, DB_TYPE_GAME_EVENT) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== event_reserving_charac_name_20130328 ORM Test ===\n\n");

    test_basic_crud(&manager);

    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void test_basic_crud(DBConnectionManager* manager) {
    printf("Test: Basic CRUD operations\n");
    EventReservingCharacName20130328 records[10];
    int count = 0;

    if (EventReservingCharacName20130328_GetAll(manager, records, 10, &count) == 0) {
        printf("  GetAll: Retrieved %d records\n", count);
    } else {
        printf("  GetAll: Failed\n");
    }
}