/*
 * script_thread.cpp - 脚本线程实现
 */

#include "../include/script_thread.h"
#include "../include/file_log.h"
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>

namespace ChannelServiceApp {

/*
 * ChannelScript 实现
 */
ChannelScript::ChannelScript()
    : m_scriptData(NULL)
    , m_scriptSize(0)
{
    m_scriptPath[0] = '\0';
}

ChannelScript::~ChannelScript() {
    ScopedLinuxLock guard(m_lock);
    if (m_scriptData) {
        free(m_scriptData);
        m_scriptData = NULL;
    }
}

void ChannelScript::setScriptPath(const char* path) {
    if (path) {
        strncpy(m_scriptPath, path, sizeof(m_scriptPath) - 1);
    }
}

bool ChannelScript::ReloadScript() {
    if (m_scriptPath[0] == '\0') {
        LOG_WARN("ChannelScript: No script path set");
        return false;
    }

    FILE* fp = fopen(m_scriptPath, "rb");
    if (!fp) {
        LOG_ERROR("ChannelScript: Cannot open script file: %s", m_scriptPath);
        return false;
    }

    /* 获取文件大小 */
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size <= 0) {
        fclose(fp);
        LOG_WARN("ChannelScript: Empty script file");
        return false;
    }

    /* 分配内存 */
    char* newData = static_cast<char*>(malloc(size + 1));
    if (!newData) {
        fclose(fp);
        LOG_ERROR("ChannelScript: Memory allocation failed");
        return false;
    }

    /* 读取文件 */
    size_t read = fread(newData, 1, size, fp);
    fclose(fp);

    if (static_cast<long>(read) != size) {
        free(newData);
        LOG_ERROR("ChannelScript: Read error");
        return false;
    }

    newData[size] = '\0';

    /* 更新脚本数据 */
    ScopedLinuxLock guard(m_lock);
    if (m_scriptData) {
        free(m_scriptData);
    }
    m_scriptData = newData;
    m_scriptSize = static_cast<int>(size);

    LOG_INFO("ChannelScript: Loaded script (%d bytes)", m_scriptSize);
    return true;
}

const char* ChannelScript::getScriptFromFile() {
    ScopedLinuxLock guard(m_lock);
    return m_scriptData;
}

int ChannelScript::getScriptFileSize() {
    ScopedLinuxLock guard(m_lock);
    return m_scriptSize;
}

/*
 * ScriptThread 实现
 */
ScriptThread::ScriptThread()
    : m_thread(0)
    , m_running(false)
    , m_checkInterval(60000)    /* 默认 1 分钟 */
{
}

ScriptThread::~ScriptThread() {
    stop();
}

bool ScriptThread::start() {
    if (m_running) {
        return true;
    }

    m_running = true;
    int ret = pthread_create(&m_thread, NULL, loop, this);
    if (ret != 0) {
        LOG_ERROR("ScriptThread: Failed to create thread: %d", ret);
        m_running = false;
        return false;
    }

    m_stateControl.setStarted();
    LOG_INFO("ScriptThread started");
    return true;
}

void ScriptThread::stop() {
    if (!m_running) {
        return;
    }

    m_running = false;
    m_stateControl.setTerminate();

    if (m_thread) {
        pthread_join(m_thread, NULL);
        m_thread = 0;
    }

    m_stateControl.setTerminated();
    LOG_INFO("ScriptThread stopped");
}

void* ScriptThread::loop(void* arg) {
    ScriptThread* self = static_cast<ScriptThread*>(arg);

    LOG_INFO("ScriptThread::loop() entering");

    while (self->m_running) {
        /* 执行检查 */
        self->onCheck();

        /* 等待 */
        usleep(self->m_checkInterval * 1000);
    }

    LOG_INFO("ScriptThread::loop() exiting");
    return NULL;
}

void ScriptThread::onCheck() {
    /* TODO: 检查脚本文件是否更新，如果更新则重新加载 */
}

} /* namespace ChannelServiceApp */
