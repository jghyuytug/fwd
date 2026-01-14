#ifndef SYSTEM_SYMBOLS_H
#define SYSTEM_SYMBOLS_H

#include <cstdint>

// 系统初始化和关闭相关符号
namespace SystemSymbols {
    // 代码页和本地化设置
    extern "C" {
        // 设置本地代码页
        int setCodePageByLocal(void);

        // 全局实例管理
        void* Neof_startupGlobalInstances(void);
        void Neof_shutdownGlobalInstances(void);

        // 服务器生命周期管理
        char Init(int argc, char **argv);
        void MainLoop(void);

        // 环境和上下文管理
        void* G_CEnvironment(void);
        const char* CEnvironment__get_file_name(void* env);

        // 关闭管理
        void* GetInstanceShutdowManager(void);
        void CShutdowManager__WaitUntilExitThread(void* manager);

        // 全局数据和日志管理
        void GlobalData__Fini(void* data);
        void CLog__destroyInstance(void* log);
    }

    // 额外的系统级函数声明
    extern "C" {
        // 日志和跟踪相关
        void* cMyTrace__cMyTrace(void* self, const char* func, int line, int flags);
        void cMyTrace__operator(int trace_obj, const char* message, int flags);

        // 网络和通信相关
        void* PacketBuf__Create(void);
        void PacketBuf__Destroy(void* packet_buf);

        // 调试和追踪
        void ShowDebugMessage(const char* message);
    }
}

// 特殊系统类型定义
struct SystemContext {
    void* environment;
    void* shutdown_manager;
    void* global_data;
    void* logging_context;
};

#endif // SYSTEM_SYMBOLS_H