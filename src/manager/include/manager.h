/*
 * manager.h - DNF Manager Service Definitions
 *
 * Process lifecycle management service.
 * Starts, stops, and monitors game service processes.
 *
 * Compatible with GCC 4.1.2 / CentOS 5.8 (VM-131)
 */

#ifndef MANAGER_H
#define MANAGER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
 * Configuration Constants
 *============================================================================*/

#define MANAGER_MAX_PROCESSES   64
#define MANAGER_MAX_NAME_LEN    32
#define MANAGER_MAX_PATH_LEN    256
#define MANAGER_MAX_ARGS_LEN    512

/*==============================================================================
 * Process Types
 *============================================================================*/

typedef enum {
    PROC_TYPE_UNKNOWN = 0,
    PROC_TYPE_BRIDGE,
    PROC_TYPE_CHANNEL,
    PROC_TYPE_GAME,
    PROC_TYPE_GUILD,
    PROC_TYPE_AUCTION,
    PROC_TYPE_POINT,
    PROC_TYPE_RELAY,
    PROC_TYPE_STUN,
    PROC_TYPE_DBMW,
    PROC_TYPE_MONITOR,
    PROC_TYPE_COMMUNITY,
    PROC_TYPE_STATICS
} ProcessType;

/*==============================================================================
 * Process Status
 *============================================================================*/

typedef enum {
    PROC_STATUS_STOPPED = 0,
    PROC_STATUS_STARTING,
    PROC_STATUS_RUNNING,
    PROC_STATUS_STOPPING,
    PROC_STATUS_CRASHED,
    PROC_STATUS_DISABLED
} ProcessStatus;

/*==============================================================================
 * Manager Message Types
 *============================================================================*/

typedef enum {
    MGMT_START_SERVICE = 1,
    MGMT_STOP_SERVICE,
    MGMT_RESTART_SERVICE,
    MGMT_GET_STATUS,
    MGMT_GET_ALL_STATUS,
    MGMT_KILL_PROCESS,
    MGMT_ADD_SERVICE,
    MGMT_REMOVE_SERVICE,
    MGMT_ENABLE_SERVICE,
    MGMT_DISABLE_SERVICE,
    MGMT_SET_AUTO_RESTART,
    MGMT_GET_LOGS
} ManagerMessageType;

/*==============================================================================
 * Data Structures
 *============================================================================*/

/* Process configuration */
typedef struct {
    char     name[MANAGER_MAX_NAME_LEN + 1];
    uint8_t  type;              /* ProcessType */
    char     path[MANAGER_MAX_PATH_LEN + 1];
    char     args[MANAGER_MAX_ARGS_LEN + 1];
    char     work_dir[MANAGER_MAX_PATH_LEN + 1];
    uint8_t  auto_restart;      /* 0 = no, 1 = yes */
    uint8_t  enabled;           /* 0 = disabled, 1 = enabled */
    uint16_t restart_delay;     /* seconds before restart */
    uint8_t  max_restarts;      /* max restarts per hour (0 = unlimited) */
} ProcessConfig;

/* Process runtime info */
typedef struct {
    uint32_t proc_id;
    char     name[MANAGER_MAX_NAME_LEN + 1];
    uint8_t  type;              /* ProcessType */
    uint8_t  status;            /* ProcessStatus */
    pid_t    pid;
    uint32_t start_time;
    uint32_t stop_time;
    uint32_t restart_count;
    uint32_t last_restart;
    int      exit_code;
} ProcessInfo;

/*==============================================================================
 * Manager Service Functions
 *============================================================================*/

/* Server-side */
int manager_service_init(uint16_t port, const char* config_file);
void manager_service_cleanup(void);
int manager_service_run(void);

/* Process management */
int manager_start_service(const char* name);
int manager_stop_service(const char* name);
int manager_restart_service(const char* name);
int manager_kill_process(pid_t pid);

/* Status */
int manager_get_status(const char* name, ProcessInfo* info);
int manager_get_all_status(ProcessInfo* info, int max_count);

/* Configuration */
int manager_add_service(const ProcessConfig* config);
int manager_remove_service(const char* name);
int manager_enable_service(const char* name);
int manager_disable_service(const char* name);
int manager_set_auto_restart(const char* name, int enabled);

#ifdef __cplusplus
}
#endif

#endif /* MANAGER_H */
