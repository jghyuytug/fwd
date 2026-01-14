/*
 * file_log.cpp - 文件日志系统实现
 */

#include "../include/file_log.h"
#include <sys/stat.h>
#include <cstdio>

namespace ChannelServiceApp {

/* 全局日志对象定义 */
FileLog g_LogInfo;
FileLog g_LogWarn;
FileLog g_LogError;
FileLog g_LogCri;

FileLog g_FileLogInfo;
FileLog g_FileLogWarn;
FileLog g_FileLogError;
FileLog g_FileLogCri;

/* 确保目录存在 */
static bool ensureDirectory(const char* path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return mkdir(path, 0755) == 0;
}

/* 创建日志 */
void createLogInfo() {
    g_LogInfo.setLevel(LOG_LEVEL_INFO);
    g_LogInfo.setConsoleOutput(true);
}

void createLogWarn() {
    g_LogWarn.setLevel(LOG_LEVEL_WARN);
    g_LogWarn.setConsoleOutput(true);
}

void createLogError() {
    g_LogError.setLevel(LOG_LEVEL_ERROR);
    g_LogError.setConsoleOutput(true);
}

void createLogCri() {
    g_LogCri.setLevel(LOG_LEVEL_CRI);
    g_LogCri.setConsoleOutput(true);
}

void createFileLogInfo() {
    ensureDirectory("log");
    g_FileLogInfo.open("log/info.log", "INFO");
    g_FileLogInfo.setLevel(LOG_LEVEL_INFO);
    g_FileLogInfo.setConsoleOutput(false);
}

void createFileLogWarn() {
    ensureDirectory("log");
    g_FileLogWarn.open("log/warn.log", "WARN");
    g_FileLogWarn.setLevel(LOG_LEVEL_WARN);
    g_FileLogWarn.setConsoleOutput(false);
}

void createFileLogError() {
    ensureDirectory("log");
    g_FileLogError.open("log/error.log", "ERROR");
    g_FileLogError.setLevel(LOG_LEVEL_ERROR);
    g_FileLogError.setConsoleOutput(false);
}

void createFileLogCri() {
    ensureDirectory("log");
    g_FileLogCri.open("log/cri.log", "CRI");
    g_FileLogCri.setLevel(LOG_LEVEL_CRI);
    g_FileLogCri.setConsoleOutput(false);
}

} /* namespace ChannelServiceApp */
