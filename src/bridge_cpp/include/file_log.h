/*
 * file_log.h - 文件日志系统
 *
 * 基于生产版的日志系统设计:
 * - 多级别日志: Info, Warn, Error, Cri
 * - 文件日志和控制台日志
 * - 线程安全
 */

#ifndef __FILE_LOG_H__
#define __FILE_LOG_H__

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include "thread_lock.h"

namespace ChannelServiceApp {

/* 日志级别 */
enum LogLevel {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO  = 1,
    LOG_LEVEL_WARN  = 2,
    LOG_LEVEL_ERROR = 3,
    LOG_LEVEL_CRI   = 4
};

/* 日志级别名称 */
static const char* LOG_LEVEL_NAMES[] = {
    "DEBUG", "INFO", "WARN", "ERROR", "CRI"
};

/*
 * FileLog - 文件日志类
 */
class FileLog {
public:
    FileLog()
        : m_fp(NULL)
        , m_level(LOG_LEVEL_INFO)
        , m_consoleOutput(true)
    {
        m_filename[0] = '\0';
        m_prefix[0] = '\0';
    }

    ~FileLog() {
        close();
    }

    /*
     * open - 打开日志文件
     * @filename: 文件名
     * @prefix: 日志前缀
     */
    bool open(const char* filename, const char* prefix = "") {
        ScopedLinuxLock guard(m_lock);

        if (m_fp) {
            fclose(m_fp);
        }

        strncpy(m_filename, filename, sizeof(m_filename) - 1);
        strncpy(m_prefix, prefix, sizeof(m_prefix) - 1);

        m_fp = fopen(filename, "a");
        return m_fp != NULL;
    }

    /*
     * close - 关闭日志文件
     */
    void close() {
        ScopedLinuxLock guard(m_lock);

        if (m_fp) {
            fclose(m_fp);
            m_fp = NULL;
        }
    }

    /*
     * setLevel - 设置日志级别
     */
    void setLevel(LogLevel level) {
        m_level = level;
    }

    /*
     * setConsoleOutput - 设置是否输出到控制台
     */
    void setConsoleOutput(bool enable) {
        m_consoleOutput = enable;
    }

    /*
     * log - 写入日志
     */
    void log(LogLevel level, const char* fmt, ...) {
        if (level < m_level) {
            return;
        }

        ScopedLinuxLock guard(m_lock);

        /* 格式化时间 */
        time_t now = time(NULL);
        struct tm* tm = localtime(&now);
        char timeBuf[32];
        strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", tm);

        /* 格式化消息 */
        char msgBuf[4096];
        va_list args;
        va_start(args, fmt);
        vsnprintf(msgBuf, sizeof(msgBuf), fmt, args);
        va_end(args);

        /* 输出到文件 */
        if (m_fp) {
            if (m_prefix[0]) {
                fprintf(m_fp, "[%s] [%s] [%s] %s\n",
                       timeBuf, LOG_LEVEL_NAMES[level], m_prefix, msgBuf);
            } else {
                fprintf(m_fp, "[%s] [%s] %s\n",
                       timeBuf, LOG_LEVEL_NAMES[level], msgBuf);
            }
            fflush(m_fp);
        }

        /* 输出到控制台 */
        if (m_consoleOutput) {
            if (m_prefix[0]) {
                fprintf(stdout, "[%s] [%s] [%s] %s\n",
                       timeBuf, LOG_LEVEL_NAMES[level], m_prefix, msgBuf);
            } else {
                fprintf(stdout, "[%s] [%s] %s\n",
                       timeBuf, LOG_LEVEL_NAMES[level], msgBuf);
            }
            fflush(stdout);
        }
    }

    /* 便捷方法 */
    void debug(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        logv(LOG_LEVEL_DEBUG, fmt, args);
        va_end(args);
    }

    void info(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        logv(LOG_LEVEL_INFO, fmt, args);
        va_end(args);
    }

    void warn(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        logv(LOG_LEVEL_WARN, fmt, args);
        va_end(args);
    }

    void error(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        logv(LOG_LEVEL_ERROR, fmt, args);
        va_end(args);
    }

    void cri(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        logv(LOG_LEVEL_CRI, fmt, args);
        va_end(args);
    }

private:
    void logv(LogLevel level, const char* fmt, va_list args) {
        if (level < m_level) {
            return;
        }

        char msgBuf[4096];
        vsnprintf(msgBuf, sizeof(msgBuf), fmt, args);
        log(level, "%s", msgBuf);
    }

    FILE* m_fp;                 /* 文件指针 */
    char m_filename[256];       /* 文件名 */
    char m_prefix[64];          /* 日志前缀 */
    LogLevel m_level;           /* 日志级别 */
    bool m_consoleOutput;       /* 是否输出到控制台 */
    LinuxLock m_lock;           /* 线程锁 */
};

/* 全局日志对象声明 */
extern FileLog g_LogInfo;
extern FileLog g_LogWarn;
extern FileLog g_LogError;
extern FileLog g_LogCri;

extern FileLog g_FileLogInfo;
extern FileLog g_FileLogWarn;
extern FileLog g_FileLogError;
extern FileLog g_FileLogCri;

/* 便捷宏 */
#define LOG_INFO(fmt, ...)  g_LogInfo.info(fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  g_LogWarn.warn(fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) g_LogError.error(fmt, ##__VA_ARGS__)
#define LOG_CRI(fmt, ...)   g_LogCri.cri(fmt, ##__VA_ARGS__)

/* 日志工厂函数 */
inline FileLog& getLogInfo() { return g_LogInfo; }
inline FileLog& getLogWarn() { return g_LogWarn; }
inline FileLog& getLogError() { return g_LogError; }
inline FileLog& getLogCri() { return g_LogCri; }

inline FileLog& getFileLogInfo() { return g_FileLogInfo; }
inline FileLog& getFileLogWarn() { return g_FileLogWarn; }
inline FileLog& getFileLogError() { return g_FileLogError; }
inline FileLog& getFileLogCri() { return g_FileLogCri; }

/* 创建日志 */
void createLogInfo();
void createLogWarn();
void createLogError();
void createLogCri();

void createFileLogInfo();
void createFileLogWarn();
void createFileLogError();
void createFileLogCri();

} /* namespace ChannelServiceApp */

#endif /* __FILE_LOG_H__ */
