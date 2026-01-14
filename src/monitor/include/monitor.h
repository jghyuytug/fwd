/*
 * monitor.h - DNF Monitor Service Definitions
 *
 * Central monitoring service for all game services.
 * Collects heartbeats, statistics, and health status.
 *
 * Compatible with GCC 4.1.2 / CentOS 5.8 (VM-131)
 */

#ifndef MONITOR_H
#define MONITOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
 * Configuration Constants
 *============================================================================*/

#define MONITOR_MAX_SERVICES    64
#define MONITOR_HEARTBEAT_INTERVAL  5   /* seconds */
#define MONITOR_TIMEOUT             15  /* seconds before marking service dead */
#define MONITOR_MAX_NAME_LEN        32

/*==============================================================================
 * Service Types
 *============================================================================*/

typedef enum {
    SERVICE_TYPE_UNKNOWN = 0,
    SERVICE_TYPE_BRIDGE,
    SERVICE_TYPE_CHANNEL,
    SERVICE_TYPE_GAME,
    SERVICE_TYPE_GUILD,
    SERVICE_TYPE_AUCTION,
    SERVICE_TYPE_POINT,
    SERVICE_TYPE_RELAY,
    SERVICE_TYPE_STUN,
    SERVICE_TYPE_DBMW,
    SERVICE_TYPE_MANAGER,
    SERVICE_TYPE_COMMUNITY,
    SERVICE_TYPE_STATICS
} ServiceType;

/*==============================================================================
 * Service Status
 *============================================================================*/

typedef enum {
    SERVICE_STATUS_UNKNOWN = 0,
    SERVICE_STATUS_STARTING,
    SERVICE_STATUS_RUNNING,
    SERVICE_STATUS_STOPPING,
    SERVICE_STATUS_STOPPED,
    SERVICE_STATUS_DEAD,
    SERVICE_STATUS_ERROR
} ServiceStatus;

/*==============================================================================
 * Monitor Message Types
 *============================================================================*/

typedef enum {
    MMSG_REGISTER = 1,          /* Service registering with monitor */
    MMSG_HEARTBEAT,             /* Periodic heartbeat */
    MMSG_STATUS_REPORT,         /* Detailed status report */
    MMSG_SHUTDOWN,              /* Service shutting down */
    MMSG_ALERT,                 /* Alert notification */
    MMSG_QUERY_STATUS,          /* Query service status */
    MMSG_QUERY_ALL,             /* Query all services */
    MMSG_RESTART_REQUEST,       /* Request service restart */
    MMSG_STOP_REQUEST,          /* Request service stop */
    MMSG_BROADCAST              /* Broadcast to all services */
} MonitorMessageType;

/*==============================================================================
 * Data Structures
 *============================================================================*/

/* Service registration info */
typedef struct {
    uint32_t service_id;
    uint8_t  service_type;      /* ServiceType */
    char     service_name[MONITOR_MAX_NAME_LEN + 1];
    uint32_t ip;
    uint16_t port;
    uint32_t pid;
    uint32_t start_time;
} ServiceRegistration;

/* Service heartbeat */
typedef struct {
    uint32_t service_id;
    uint32_t timestamp;
    uint32_t player_count;      /* For game servers */
    uint8_t  cpu_usage;         /* 0-100 */
    uint8_t  memory_usage;      /* 0-100 */
    uint32_t connections;       /* Active connections */
    uint32_t requests_per_sec;  /* RPS */
} ServiceHeartbeat;

/* Service status entry */
typedef struct {
    uint32_t service_id;
    uint8_t  service_type;
    char     service_name[MONITOR_MAX_NAME_LEN + 1];
    uint32_t ip;
    uint16_t port;
    uint32_t pid;
    uint8_t  status;            /* ServiceStatus */
    uint32_t start_time;
    uint32_t last_heartbeat;
    uint32_t player_count;
    uint8_t  cpu_usage;
    uint8_t  memory_usage;
    uint32_t connections;
    uint32_t uptime;            /* seconds */
} ServiceStatusEntry;

/*==============================================================================
 * Monitor Service Functions
 *============================================================================*/

/* Server-side (Monitor Service) */
int monitor_service_init(uint16_t port);
void monitor_service_cleanup(void);
int monitor_service_run(void);

/* Get service status */
int monitor_get_service(uint32_t service_id, ServiceStatusEntry* entry);
int monitor_get_all_services(ServiceStatusEntry* entries, int max_count);
int monitor_get_services_by_type(uint8_t type, ServiceStatusEntry* entries, int max_count);

/* Service management */
int monitor_request_restart(uint32_t service_id);
int monitor_request_stop(uint32_t service_id);

/*==============================================================================
 * Monitor Client Functions (for other services to use)
 *============================================================================*/

/* Client-side (Game/Guild/etc services) */
int monitor_client_init(const char* monitor_host, uint16_t monitor_port);
void monitor_client_cleanup(void);

/* Register this service with monitor */
int monitor_client_register(uint8_t service_type, const char* service_name,
                           uint16_t port, uint32_t* out_service_id);

/* Send heartbeat */
int monitor_client_heartbeat(uint32_t player_count, uint8_t cpu_usage,
                            uint8_t memory_usage, uint32_t connections);

/* Send shutdown notification */
int monitor_client_shutdown(void);

/* Alert notification */
int monitor_client_alert(const char* message);

#ifdef __cplusplus
}
#endif

#endif /* MONITOR_H */
