/**
 * @file logger.h
 * @brief Logging system with levels and file output
 *
 * Thread-safe logging with configurable output.
 */

#ifndef ENGINE_CORE_LOG_LOGGER_H
#define ENGINE_CORE_LOG_LOGGER_H

#include "../types.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Log levels */
typedef enum {
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG = 1,
    LOG_LEVEL_INFO  = 2,
    LOG_LEVEL_WARN  = 3,
    LOG_LEVEL_ERROR = 4,
    LOG_LEVEL_FATAL = 5,
    LOG_LEVEL_OFF   = 6
} LogLevel;

/* Log output flags */
#define LOG_OUTPUT_NONE     0x00
#define LOG_OUTPUT_CONSOLE  0x01
#define LOG_OUTPUT_FILE     0x02
#define LOG_OUTPUT_CALLBACK 0x04

/* Log format flags */
#define LOG_FORMAT_TIME     0x01
#define LOG_FORMAT_LEVEL    0x02
#define LOG_FORMAT_FILE     0x04
#define LOG_FORMAT_LINE     0x08
#define LOG_FORMAT_FUNC     0x10
#define LOG_FORMAT_DEFAULT  (LOG_FORMAT_TIME | LOG_FORMAT_LEVEL)

/* Logger configuration */
typedef struct {
    LogLevel    level;
    u8          output_flags;
    u8          format_flags;
    char        log_file[256];
    u32         max_file_size;      /* Max size before rotation (0 = no limit) */
    u8          max_files;          /* Max rotated files to keep */
} LogConfig;

/* Log callback */
typedef void (*LogCallback)(LogLevel level, const char* message, void* user_data);

/* Forward declaration */
struct Logger;
typedef struct Logger Logger;

/**
 * Initialize global logger
 * @param config Logger configuration
 * @return ENGINE_OK on success
 */
int logger_init(const LogConfig* config);

/**
 * Cleanup global logger
 */
void logger_cleanup(void);

/**
 * Set log level
 * @param level New log level
 */
void logger_set_level(LogLevel level);

/**
 * Get current log level
 * @return Current log level
 */
LogLevel logger_get_level(void);

/**
 * Set output flags
 * @param flags Output flags (LOG_OUTPUT_*)
 */
void logger_set_output(u8 flags);

/**
 * Set log callback
 * @param callback Callback function
 * @param user_data User context
 */
void logger_set_callback(LogCallback callback, void* user_data);

/**
 * Set log file
 * @param filename Log file path
 * @return ENGINE_OK on success
 */
int logger_set_file(const char* filename);

/**
 * Close log file
 */
void logger_close_file(void);

/**
 * Flush log buffers
 */
void logger_flush(void);

/**
 * Log a message (internal - use macros)
 * @param level Log level
 * @param file Source file
 * @param line Source line
 * @param func Function name
 * @param fmt Format string
 */
void logger_log(LogLevel level, const char* file, int line,
                const char* func, const char* fmt, ...);

/**
 * Log with va_list (internal)
 */
void logger_logv(LogLevel level, const char* file, int line,
                 const char* func, const char* fmt, va_list args);

/**
 * Get level name string
 * @param level Log level
 * @return Level name
 */
const char* logger_level_name(LogLevel level);

/* Logging macros */
#define LOG_TRACE(fmt, ...) \
    logger_log(LOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define LOG_DEBUG(fmt, ...) \
    logger_log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define LOG_INFO(fmt, ...) \
    logger_log(LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define LOG_WARN(fmt, ...) \
    logger_log(LOG_LEVEL_WARN, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define LOG_ERROR(fmt, ...) \
    logger_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define LOG_FATAL(fmt, ...) \
    logger_log(LOG_LEVEL_FATAL, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

/* Conditional logging */
#define LOG_IF(cond, level, fmt, ...) \
    do { if (cond) logger_log(level, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__); } while(0)

/* Assert with logging */
#define LOG_ASSERT(cond, fmt, ...) \
    do { \
        if (!(cond)) { \
            LOG_FATAL("Assertion failed: " #cond " - " fmt, ##__VA_ARGS__); \
        } \
    } while(0)

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_CORE_LOG_LOGGER_H */
