/*
 * memory_pool.h - 静态内存池模板
 *
 * 基于生产版的 TMemoryPoolStatic 设计:
 * - 预分配固定数量的对象
 * - 使用队列管理空闲对象
 * - 线程安全的分配和释放
 */

#ifndef __MEMORY_POOL_H__
#define __MEMORY_POOL_H__

#include <queue>
#include <cstdlib>
#include <cstring>
#include "thread_lock.h"

namespace ChannelServiceApp {

/*
 * TMemoryPoolStatic - 静态内存池模板
 *
 * @T: 对象类型
 * @MaxSize: 池大小
 */
template<typename T, int MaxSize = 1000>
class TMemoryPoolStatic {
public:
    TMemoryPoolStatic()
        : m_pool(NULL)
        , m_initialized(false)
    {}

    ~TMemoryPoolStatic() {
        if (m_pool) {
            /* 调用所有对象的析构函数 */
            for (int i = 0; i < MaxSize; i++) {
                m_pool[i].~T();
            }
            ::free(m_pool);
            m_pool = NULL;
        }
    }

    /*
     * startup - 初始化内存池
     * 预分配所有对象并加入空闲队列
     */
    bool startup() {
        if (m_initialized) {
            return true;
        }

        /* 分配原始内存 */
        m_pool = (T*)::malloc(sizeof(T) * MaxSize);
        if (!m_pool) {
            return false;
        }

        /* 使用 placement new 构造所有对象 */
        for (int i = 0; i < MaxSize; i++) {
            new (&m_pool[i]) T();
            m_freeQueue.push(&m_pool[i]);
        }

        m_initialized = true;
        return true;
    }

    /*
     * alloc - 从池中分配一个对象
     * 返回: 对象指针，或 NULL 如果池已空
     */
    T* alloc() {
        ScopedLinuxLock guard(m_lock);

        if (m_freeQueue.empty()) {
            return NULL;
        }

        T* obj = m_freeQueue.front();
        m_freeQueue.pop();
        return obj;
    }

    /*
     * free - 将对象归还到池中
     */
    void free(T* obj) {
        if (!obj) {
            return;
        }

        ScopedLinuxLock guard(m_lock);
        m_freeQueue.push(obj);
    }

    /*
     * getRemain - 获取剩余可用对象数
     */
    int getRemain() {
        ScopedLinuxLock guard(m_lock);
        return static_cast<int>(m_freeQueue.size());
    }

    /*
     * getMaxSize - 获取池容量
     */
    int getMaxSize() const {
        return MaxSize;
    }

private:
    TMemoryPoolStatic(const TMemoryPoolStatic&);
    TMemoryPoolStatic& operator=(const TMemoryPoolStatic&);

    T* m_pool;                      /* 预分配的对象数组 */
    std::queue<T*> m_freeQueue;     /* 空闲对象队列 */
    LinuxLock m_lock;               /* 线程锁 */
    bool m_initialized;             /* 初始化标志 */
};

} /* namespace ChannelServiceApp */

#endif /* __MEMORY_POOL_H__ */
