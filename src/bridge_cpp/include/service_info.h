/*
 * service_info.h - 服务信息
 *
 * 基于生产版的 ServiceInfo 设计
 */

#ifndef __SERVICE_INFO_H__
#define __SERVICE_INFO_H__

#include <cstring>

namespace ChannelServiceApp {

/*
 * ServiceInfo - 服务信息类
 */
class ServiceInfo {
public:
    ServiceInfo()
        : m_runAsService(false)
    {
        m_serviceName[0] = '\0';
        m_displayName[0] = '\0';
        m_configFileName[0] = '\0';
        m_pidFileName[0] = '\0';
    }

    /*
     * setInfo - 设置服务信息
     */
    void setInfo(const char* serviceName,
                 const char* displayName,
                 const char* configFileName,
                 const char* pidFileName) {
        if (serviceName) {
            strncpy(m_serviceName, serviceName, sizeof(m_serviceName) - 1);
        }
        if (displayName) {
            strncpy(m_displayName, displayName, sizeof(m_displayName) - 1);
        }
        if (configFileName) {
            strncpy(m_configFileName, configFileName, sizeof(m_configFileName) - 1);
        }
        if (pidFileName) {
            strncpy(m_pidFileName, pidFileName, sizeof(m_pidFileName) - 1);
        }
    }

    /*
     * setPIDFileName - 设置 PID 文件名
     */
    void setPIDFileName(const char* pidFileName) {
        if (pidFileName) {
            strncpy(m_pidFileName, pidFileName, sizeof(m_pidFileName) - 1);
        }
    }

    /*
     * setRunAsService - 设置是否作为守护进程运行
     */
    void setRunAsService(bool runAsService) {
        m_runAsService = runAsService;
    }

    /* Getters */
    const char* getServiceName() const { return m_serviceName; }
    const char* getDisplayName() const { return m_displayName; }
    const char* getConfigFileName() const { return m_configFileName; }
    const char* getPIDFileName() const { return m_pidFileName; }
    bool isRunAsService() const { return m_runAsService; }

private:
    char m_serviceName[64];
    char m_displayName[128];
    char m_configFileName[256];
    char m_pidFileName[256];
    bool m_runAsService;
};

} /* namespace ChannelServiceApp */

#endif /* __SERVICE_INFO_H__ */
