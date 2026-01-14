/**
 * @file logger.c
 * @brief Logging system implementation
 */

#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define LOG_BUFFER_SIZE 4096

/* Global logger instance */
static struct {
    LogLevel    level;
    u8          output_flags;
    u8          format_flags;
    char        log_file[256];
    FILE*       file_handle;
    u32         max_file_size;
    u8          max_files;
    u32         current_file_size;
    LogCallback callback;
    void*       callback_data;
    pthread_mutex_t mutex;
    u8          initialized;
} g_logger;

/* Level names */
static const char* level_names[] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
    "OFF"
};

/* Level colors (ANSI) */
static const char* level_colors[] = {
    "\033[37m",     /* TRACE: white */
    "\033[36m",     /* DEBUG: cyan */
    "\033[32m",     /* INFO: green */
    "\033[33m",     /* WARN: yellow */
    "\033[31m",     /* ERROR: red */
    "\033[35m",     /* FATAL: magenta */
    ""
};

static const char* color_reset = "\033[0m";

/* Get basename from path */
static const char* get_basename(const char* path)
{
    const char* p;

    if (!path) return "";

    p = strrchr(path, '/');
    if (!p) p = strrchr(path, '\\');

    return p ? p + 1 : path;
}

/* Format timestamp */
static void format_time(char* buf, size_t size)
{
    time_t now;
    struct tm* tm_info;

    time(&now);
    tm_info = localtime(&now);

    strftime(buf, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

/* Rotate log file */
static void rotate_log_file(void)
{
    char old_name[280];
    char new_name[280];
    int i;

    if (!g_logger.file_handle || g_logger.log_file[0] == '\0') {
        return;
    }

    fclose(g_logger.file_handle);
    g_logger.file_handle = NULL;

    /* Rotate: log.4 -> delete, log.3 -> log.4, ... log -> log.1 */
    for (i = g_logger.max_files - 1; i >= 0; i--) {
        if (i == 0) {
            snprintf(old_name, sizeof(old_name), "%s", g_logger.log_file);
        } else {
            snprintf(old_name, sizeof(old_name), "%s.%d", g_logger.log_file, i);
        }

        snprintf(new_name, sizeof(new_name), "%s.%d", g_logger.log_file, i + 1);

        if (i == g_logger.max_files - 1) {
            remove(old_name);
        } else {
            rename(old_name, new_name);
        }
    }

    /* Reopen log file */
    g_logger.file_handle = fopen(g_logger.log_file, "a");
    g_logger.current_file_size = 0;
}

int logger_init(const LogConfig* config)
{
    if (g_logger.initialized) {
        return ENGINE_OK;
    }

    memset(&g_logger, 0, sizeof(g_logger));

    if (config) {
        g_logger.level = config->level;
        g_logger.output_flags = config->output_flags;
        g_logger.format_flags = config->format_flags;
        g_logger.max_file_size = config->max_file_size;
        g_logger.max_files = config->max_files > 0 ? config->max_files : 5;

        if (config->log_file[0] != '\0' &&
            (config->output_flags & LOG_OUTPUT_FILE)) {
            strncpy(g_logger.log_file, config->log_file,
                    sizeof(g_logger.log_file) - 1);
            g_logger.file_handle = fopen(config->log_file, "a");
        }
    } else {
        g_logger.level = LOG_LEVEL_INFO;
        g_logger.output_flags = LOG_OUTPUT_CONSOLE;
        g_logger.format_flags = LOG_FORMAT_DEFAULT;
        g_logger.max_files = 5;
    }

    pthread_mutex_init(&g_logger.mutex, NULL);
    g_logger.initialized = 1;

    return ENGINE_OK;
}

void logger_cleanup(void)
{
    if (!g_logger.initialized) {
        return;
    }

    if (g_logger.file_handle) {
        fclose(g_logger.file_handle);
        g_logger.file_handle = NULL;
    }

    pthread_mutex_destroy(&g_logger.mutex);
    g_logger.initialized = 0;
}

void logger_set_level(LogLevel level)
{
    g_logger.level = level;
}

LogLevel logger_get_level(void)
{
    return g_logger.level;
}

void logger_set_output(u8 flags)
{
    g_logger.output_flags = flags;
}

void logger_set_callback(LogCallback callback, void* user_data)
{
    g_logger.callback = callback;
    g_logger.callback_data = user_data;
}

int logger_set_file(const char* filename)
{
    if (!filename || filename[0] == '\0') {
        return ENGINE_ERROR_PARAM;
    }

    pthread_mutex_lock(&g_logger.mutex);

    if (g_logger.file_handle) {
        fclose(g_logger.file_handle);
    }

    strncpy(g_logger.log_file, filename, sizeof(g_logger.log_file) - 1);
    g_logger.file_handle = fopen(filename, "a");
    g_logger.current_file_size = 0;
    g_logger.output_flags |= LOG_OUTPUT_FILE;

    pthread_mutex_unlock(&g_logger.mutex);

    return g_logger.file_handle ? ENGINE_OK : ENGINE_ERROR_IO;
}

void logger_close_file(void)
{
    pthread_mutex_lock(&g_logger.mutex);

    if (g_logger.file_handle) {
        fclose(g_logger.file_handle);
        g_logger.file_handle = NULL;
    }

    g_logger.output_flags &= ~LOG_OUTPUT_FILE;

    pthread_mutex_unlock(&g_logger.mutex);
}

void logger_flush(void)
{
    pthread_mutex_lock(&g_logger.mutex);

    if (g_logger.file_handle) {
        fflush(g_logger.file_handle);
    }

    fflush(stdout);
    fflush(stderr);

    pthread_mutex_unlock(&g_logger.mutex);
}

void logger_logv(LogLevel level, const char* file, int line,
                 const char* func, const char* fmt, va_list args)
{
    char time_buf[32];
    char msg_buf[LOG_BUFFER_SIZE];
    char full_buf[LOG_BUFFER_SIZE + 256];
    int len;
    int msg_len;

    /* Check level */
    if (level < g_logger.level) {
        return;
    }

    /* Format message */
    msg_len = vsnprintf(msg_buf, sizeof(msg_buf), fmt, args);
    if (msg_len < 0) {
        return;
    }

    /* Build full log line */
    len = 0;

    if (g_logger.format_flags & LOG_FORMAT_TIME) {
        format_time(time_buf, sizeof(time_buf));
        len += snprintf(full_buf + len, sizeof(full_buf) - len,
                        "[%s] ", time_buf);
    }

    if (g_logger.format_flags & LOG_FORMAT_LEVEL) {
        len += snprintf(full_buf + len, sizeof(full_buf) - len,
                        "[%-5s] ", level_names[level]);
    }

    if ((g_logger.format_flags & LOG_FORMAT_FILE) && file) {
        len += snprintf(full_buf + len, sizeof(full_buf) - len,
                        "[%s", get_basename(file));

        if (g_logger.format_flags & LOG_FORMAT_LINE) {
            len += snprintf(full_buf + len, sizeof(full_buf) - len,
                            ":%d", line);
        }

        len += snprintf(full_buf + len, sizeof(full_buf) - len, "] ");
    }

    if ((g_logger.format_flags & LOG_FORMAT_FUNC) && func) {
        len += snprintf(full_buf + len, sizeof(full_buf) - len,
                        "[%s] ", func);
    }

    /* Append message */
    len += snprintf(full_buf + len, sizeof(full_buf) - len, "%s", msg_buf);

    /* Ensure newline */
    if (len > 0 && full_buf[len - 1] != '\n') {
        if (len < (int)sizeof(full_buf) - 1) {
            full_buf[len++] = '\n';
            full_buf[len] = '\0';
        }
    }

    pthread_mutex_lock(&g_logger.mutex);

    /* Output to console */
    if (g_logger.output_flags & LOG_OUTPUT_CONSOLE) {
        FILE* out = (level >= LOG_LEVEL_ERROR) ? stderr : stdout;

        fprintf(out, "%s%s%s", level_colors[level], full_buf, color_reset);
    }

    /* Output to file */
    if ((g_logger.output_flags & LOG_OUTPUT_FILE) && g_logger.file_handle) {
        /* Check rotation */
        if (g_logger.max_file_size > 0 &&
            g_logger.current_file_size + len > g_logger.max_file_size) {
            rotate_log_file();
        }

        if (g_logger.file_handle) {
            fprintf(g_logger.file_handle, "%s", full_buf);
            g_logger.current_file_size += len;
        }
    }

    /* Output to callback */
    if ((g_logger.output_flags & LOG_OUTPUT_CALLBACK) && g_logger.callback) {
        g_logger.callback(level, full_buf, g_logger.callback_data);
    }

    pthread_mutex_unlock(&g_logger.mutex);
}

void logger_log(LogLevel level, const char* file, int line,
                const char* func, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    logger_logv(level, file, line, func, fmt, args);
    va_end(args);
}

const char* logger_level_name(LogLevel level)
{
    if (level >= 0 && level <= LOG_LEVEL_OFF) {
        return level_names[level];
    }
    return "UNKNOWN";
}
