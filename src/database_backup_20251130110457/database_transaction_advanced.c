#include "headers/database_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Configuration: Set transaction timeout
void CDBConnector_SetTransactionTimeout(CDBConnector* self, int timeout_seconds) {
    self->transaction_timeout_seconds = timeout_seconds;
    printf("[Database] Transaction timeout set to %d seconds\n", timeout_seconds);
}

// Enhanced transaction: Begin transaction with timeout
int CDBConnector_BeginTransactionWithTimeout(CDBConnector* self, int db_type, int timeout_seconds) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        fprintf(stderr, "[Database] Invalid db_type: %d\n", db_type);
        return -1;
    }

    // Check if already in transaction
    TransactionState* trans = &self->active_transactions[db_type];
    if (trans->is_active) {
        fprintf(stderr, "[Database] Transaction already active for db_type %d\n", db_type);
        return -1;
    }

    // Get a connection from pool
    DBConnectionPool* pool = &self->pools[db_type];
    DBConnection* conn = NULL;

    for (int i = 0; i < pool->pool_size; i++) {
        if (pool->connections[i].is_connected && !pool->connections[i].in_use) {
            conn = &pool->connections[i];
            conn->in_use = true;
            pool->in_use++;
            pool->available--;
            break;
        }
    }

    if (!conn) {
        fprintf(stderr, "[Database] No available connection for transaction (db_type %d)\n", db_type);
        return -1;
    }

    // Start transaction
    int ret = mysql_query(conn->mysql_conn, "START TRANSACTION");
    if (ret != 0) {
        fprintf(stderr, "[Database] Failed to start transaction: %s\n",
                mysql_error(conn->mysql_conn));
        conn->in_use = false;
        pool->in_use--;
        pool->available++;
        return -1;
    }

    // Initialize transaction state
    trans->db_type = db_type;
    trans->connection = conn;
    trans->is_active = true;
    trans->started_at = time(NULL);
    trans->savepoint_depth = 0;
    trans->timeout_seconds = timeout_seconds;
    memset(trans->savepoint_name, 0, sizeof(trans->savepoint_name));

    // Update statistics
    self->transactions_started++;

    printf("[Database] Transaction started for db_type %d (timeout: %d seconds)\n",
           db_type, timeout_seconds);

    return 0;
}

// Create savepoint (for nested transaction support)
int CDBConnector_CreateSavepoint(CDBConnector* self, int db_type, const char* savepoint_name) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        return -1;
    }

    TransactionState* trans = &self->active_transactions[db_type];
    if (!trans->is_active) {
        fprintf(stderr, "[Database] No active transaction for db_type %d\n", db_type);
        return -1;
    }

    // Build savepoint command
    char query[256];
    snprintf(query, sizeof(query), "SAVEPOINT %s", savepoint_name);

    int ret = mysql_query(trans->connection->mysql_conn, query);
    if (ret != 0) {
        fprintf(stderr, "[Database] Failed to create savepoint '%s': %s\n",
                savepoint_name, mysql_error(trans->connection->mysql_conn));
        return -1;
    }

    // Update transaction state
    trans->savepoint_depth++;
    strncpy(trans->savepoint_name, savepoint_name, sizeof(trans->savepoint_name) - 1);

    printf("[Database] Savepoint '%s' created for db_type %d (depth: %d)\n",
           savepoint_name, db_type, trans->savepoint_depth);

    return 0;
}

// Rollback to savepoint
int CDBConnector_RollbackToSavepoint(CDBConnector* self, int db_type, const char* savepoint_name) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        return -1;
    }

    TransactionState* trans = &self->active_transactions[db_type];
    if (!trans->is_active) {
        fprintf(stderr, "[Database] No active transaction for db_type %d\n", db_type);
        return -1;
    }

    // Build rollback command
    char query[256];
    snprintf(query, sizeof(query), "ROLLBACK TO SAVEPOINT %s", savepoint_name);

    int ret = mysql_query(trans->connection->mysql_conn, query);
    if (ret != 0) {
        fprintf(stderr, "[Database] Failed to rollback to savepoint '%s': %s\n",
                savepoint_name, mysql_error(trans->connection->mysql_conn));
        return -1;
    }

    printf("[Database] Rolled back to savepoint '%s' for db_type %d\n",
           savepoint_name, db_type);

    return 0;
}

// Release savepoint
int CDBConnector_ReleaseSavepoint(CDBConnector* self, int db_type, const char* savepoint_name) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        return -1;
    }

    TransactionState* trans = &self->active_transactions[db_type];
    if (!trans->is_active) {
        fprintf(stderr, "[Database] No active transaction for db_type %d\n", db_type);
        return -1;
    }

    // Build release command
    char query[256];
    snprintf(query, sizeof(query), "RELEASE SAVEPOINT %s", savepoint_name);

    int ret = mysql_query(trans->connection->mysql_conn, query);
    if (ret != 0) {
        fprintf(stderr, "[Database] Failed to release savepoint '%s': %s\n",
                savepoint_name, mysql_error(trans->connection->mysql_conn));
        return -1;
    }

    // Update transaction state
    if (trans->savepoint_depth > 0) {
        trans->savepoint_depth--;
    }

    printf("[Database] Savepoint '%s' released for db_type %d (depth: %d)\n",
           savepoint_name, db_type, trans->savepoint_depth);

    return 0;
}

// Check transaction timeout
int CDBConnector_CheckTransactionTimeout(CDBConnector* self, int db_type) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        return 0;
    }

    TransactionState* trans = &self->active_transactions[db_type];
    if (!trans->is_active) {
        return 0; // No active transaction
    }

    // Check if transaction has timed out
    int current_time = time(NULL);
    int elapsed_time = current_time - trans->started_at;

    if (trans->timeout_seconds > 0 && elapsed_time > trans->timeout_seconds) {
        fprintf(stderr, "[Database] Transaction timeout detected for db_type %d "
                       "(elapsed: %d seconds, limit: %d seconds)\n",
                db_type, elapsed_time, trans->timeout_seconds);

        // Automatically rollback timed-out transaction
        int ret = mysql_query(trans->connection->mysql_conn, "ROLLBACK");
        if (ret != 0) {
            fprintf(stderr, "[Database] Failed to rollback timed-out transaction: %s\n",
                    mysql_error(trans->connection->mysql_conn));
        }

        // Release connection
        DBConnectionPool* pool = &self->pools[db_type];
        trans->connection->in_use = false;
        trans->connection->last_used = time(NULL);
        pool->in_use--;
        pool->available++;

        // Clear transaction state
        trans->is_active = false;
        trans->connection = NULL;
        trans->savepoint_depth = 0;

        // Update statistics
        self->transactions_timed_out++;
        self->transactions_rolled_back++;

        printf("[Database] Transaction automatically rolled back due to timeout\n");
        return 1; // Timeout occurred
    }

    return 0; // No timeout
}

// Enhanced commit transaction (with timeout check)
int CDBConnector_CommitTransaction(CDBConnector* self, int db_type) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        return -1;
    }

    TransactionState* trans = &self->active_transactions[db_type];
    if (!trans->is_active) {
        fprintf(stderr, "[Database] No active transaction to commit for db_type %d\n", db_type);
        return -1;
    }

    // Check timeout before commit
    if (CDBConnector_CheckTransactionTimeout(self, db_type)) {
        fprintf(stderr, "[Database] Cannot commit - transaction has timed out\n");
        return -1;
    }

    // Commit transaction
    int ret = mysql_query(trans->connection->mysql_conn, "COMMIT");
    if (ret != 0) {
        fprintf(stderr, "[Database] Failed to commit transaction: %s\n",
                mysql_error(trans->connection->mysql_conn));

        // Rollback on commit failure
        mysql_query(trans->connection->mysql_conn, "ROLLBACK");
        self->transactions_rolled_back++;

        // Release connection
        DBConnectionPool* pool = &self->pools[db_type];
        trans->connection->in_use = false;
        trans->connection->last_used = time(NULL);
        pool->in_use--;
        pool->available++;

        trans->is_active = false;
        return -1;
    }

    // Calculate transaction duration
    int duration = time(NULL) - trans->started_at;

    // Release connection
    DBConnectionPool* pool = &self->pools[db_type];
    trans->connection->in_use = false;
    trans->connection->last_used = time(NULL);
    pool->in_use--;
    pool->available++;

    // Clear transaction state
    trans->is_active = false;
    trans->connection = NULL;
    trans->savepoint_depth = 0;

    // Update statistics
    self->transactions_committed++;

    printf("[Database] Transaction committed for db_type %d (duration: %d seconds)\n",
           db_type, duration);

    return 0;
}

// Enhanced rollback transaction (with recovery)
int CDBConnector_RollbackTransaction(CDBConnector* self, int db_type) {
    if (db_type < 0 || db_type > DB_TYPE_MAX_INDEX) {
        return -1;
    }

    TransactionState* trans = &self->active_transactions[db_type];
    if (!trans->is_active) {
        fprintf(stderr, "[Database] No active transaction to rollback for db_type %d\n", db_type);
        return -1;
    }

    // Rollback transaction
    int ret = mysql_query(trans->connection->mysql_conn, "ROLLBACK");
    if (ret != 0) {
        fprintf(stderr, "[Database] Failed to rollback transaction: %s\n",
                mysql_error(trans->connection->mysql_conn));
        // Even if rollback fails, we should clean up state
    }

    // Calculate transaction duration
    int duration = time(NULL) - trans->started_at;

    // Release connection
    DBConnectionPool* pool = &self->pools[db_type];
    trans->connection->in_use = false;
    trans->connection->last_used = time(NULL);
    pool->in_use--;
    pool->available++;

    // Clear transaction state
    trans->is_active = false;
    trans->connection = NULL;
    trans->savepoint_depth = 0;

    // Update statistics
    self->transactions_rolled_back++;

    printf("[Database] Transaction rolled back for db_type %d (duration: %d seconds)\n",
           db_type, duration);

    return 0;
}

// Print transaction statistics
void CDBConnector_PrintTransactionStats(CDBConnector* self) {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║     Transaction Statistics Report     ║\n");
    printf("╚════════════════════════════════════════╝\n\n");

    printf("Configuration:\n");
    printf("  Default transaction timeout: %d seconds\n", self->transaction_timeout_seconds);

    printf("\nTransaction Statistics:\n");
    printf("  Total started: %d\n", self->transactions_started);
    printf("  Total committed: %d\n", self->transactions_committed);
    printf("  Total rolled back: %d\n", self->transactions_rolled_back);
    printf("  Total timed out: %d\n", self->transactions_timed_out);

    if (self->transactions_started > 0) {
        float commit_rate = (float)self->transactions_committed / self->transactions_started * 100.0f;
        float rollback_rate = (float)self->transactions_rolled_back / self->transactions_started * 100.0f;
        float timeout_rate = (float)self->transactions_timed_out / self->transactions_started * 100.0f;

        printf("\nSuccess Rates:\n");
        printf("  Commit rate: %.2f%%\n", commit_rate);
        printf("  Rollback rate: %.2f%%\n", rollback_rate);
        printf("  Timeout rate: %.2f%%\n", timeout_rate);
    }

    // Check for active transactions
    int active_count = 0;
    for (int i = 0; i <= DB_TYPE_MAX_INDEX; i++) {
        if (self->active_transactions[i].is_active) {
            active_count++;
        }
    }

    printf("\nActive Transactions: %d\n", active_count);
    if (active_count > 0) {
        printf("  Warning: There are %d active transactions that have not been committed/rolled back\n",
               active_count);
    }

    printf("\n");
}

// Get transaction statistics (programmatic interface)
int CDBConnector_GetTransactionStats(CDBConnector* self, int* started, int* committed,
                                      int* rolled_back, int* timed_out) {
    if (started) *started = self->transactions_started;
    if (committed) *committed = self->transactions_committed;
    if (rolled_back) *rolled_back = self->transactions_rolled_back;
    if (timed_out) *timed_out = self->transactions_timed_out;

    return 0;
}
