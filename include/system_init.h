#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H

#include <cstdint>

// 前置声明
class GameWorld;
class CEnvironment;
class CShutdownManager;
class CLog;

#ifdef __cplusplus
extern "C" {
#endif
    // 代码页设置
    int setCodePageByLocal(void);

    // 全局实例管理
    void* Neof_startupGlobalInstances(void);
    void Neof_shutdownGlobalInstances(void);

    // 服务器初始化和主循环
    char Init(int argc, char **argv);
    void MainLoop(void);

    // 环境管理
    void* G_CEnvironment(void);
    const char* CEnvironment__get_file_name(void* env);

    // 关闭管理
    void* GetInstanceShutdowManager(void);
    void CShutdowManager__WaitUntilExitThread(void* manager);

    // 数据和日志管理
    void GlobalData__Fini(void* data);
    void CLog__destroyInstance(void* log);
#ifdef __cplusplus
}
#endif

#endif // SYSTEM_INIT_H