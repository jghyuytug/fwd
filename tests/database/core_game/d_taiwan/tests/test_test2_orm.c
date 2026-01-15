#include "test2_orm.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>
", argv[0]);
        return 1;
    }

    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0 ||
        DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("Failed to initialize
");
        return 1;
    }

    printf("=== test2 ORM Test ===
");
    printf("Basic structure test passed
");
    printf("=== All Tests Completed ===
");

    DBConnectionManager_Cleanup(&manager);
    return 0;
}
