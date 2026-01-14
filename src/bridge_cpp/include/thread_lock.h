/*
 * thread_lock.h - 线程同步原语
 *
 * 基于生产版的模板化线程锁设计:
 * - ThreadLock_linux: 基于 pthread_mutex 的锁
 * - ThreadLock_Zero: 空锁(无操作)
 * - TThreadLock<T>: 线程锁模板包装
 * - TScopedLock<T>: RAII 作用域锁
 * - TThreadStateControl<T>: 线程状态控制
 */

#ifndef __THREAD_LOCK_H__
#define __THREAD_LOCK_H__

#include <pthread.h>

namespace ChannelServiceApp {

/*
 * ThreadLock_Zero - 空锁实现
 * 用于单线程场景，无实际锁操作
 */
class ThreadLock_Zero {
public:
    ThreadLock_Zero() {}
    ~ThreadLock_Zero() {}

    void lock() {}
    void unlock() {}
};

/*
 * ThreadLock_linux - Linux pthread_mutex 锁
 */
class ThreadLock_linux {
public:
    ThreadLock_linux() {
        pthread_mutex_init(&m_mutex, NULL);
    }

    ~ThreadLock_linux() {
        pthread_mutex_destroy(&m_mutex);
    }

    void lock() {
        _lock();
    }

    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }

private:
    void _lock() {
        pthread_mutex_lock(&m_mutex);
    }

    pthread_mutex_t m_mutex;
};

/*
 * TThreadLock<T> - 线程锁模板包装
 */
template<typename LockType>
class TThreadLock : public LockType {
public:
    TThreadLock() : LockType() {}
    ~TThreadLock() {}

    void lock() {
        LockType::lock();
    }

    void unlock() {
        LockType::unlock();
    }
};

/*
 * TScopedLock<T> - RAII 作用域锁
 * 构造时加锁，析构时解锁
 */
template<typename LockType>
class TScopedLock {
public:
    explicit TScopedLock(LockType& lock) : m_lock(lock) {
        m_lock.lock();
    }

    ~TScopedLock() {
        m_lock.unlock();
    }

private:
    TScopedLock(const TScopedLock&);
    TScopedLock& operator=(const TScopedLock&);

    LockType& m_lock;
};

/*
 * TThreadStateControl<T> - 线程状态控制
 * 管理线程的启动/停止/终止状态
 */
template<typename LockType>
class TThreadStateControl {
public:
    TThreadStateControl()
        : m_started(false)
        , m_terminating(false)
        , m_terminated(false)
    {}

    ~TThreadStateControl() {}

    void setStarted() {
        TScopedLock<LockType> guard(m_lock);
        m_started = true;
    }

    bool isStarted() {
        TScopedLock<LockType> guard(m_lock);
        return m_started;
    }

    void setTerminate() {
        TScopedLock<LockType> guard(m_lock);
        m_terminating = true;
    }

    bool isTerminating() {
        TScopedLock<LockType> guard(m_lock);
        return m_terminating;
    }

    void setTerminated() {
        TScopedLock<LockType> guard(m_lock);
        m_terminated = true;
    }

    bool isTerminated() {
        TScopedLock<LockType> guard(m_lock);
        return m_terminated;
    }

    bool isStop() {
        TScopedLock<LockType> guard(m_lock);
        return m_terminating || m_terminated;
    }

private:
    LockType m_lock;
    bool m_started;
    bool m_terminating;
    bool m_terminated;
};

/* 常用类型别名 */
typedef TThreadLock<ThreadLock_linux> LinuxLock;
typedef TThreadLock<ThreadLock_Zero> NoLock;
typedef TScopedLock<LinuxLock> ScopedLinuxLock;

} /* namespace ChannelServiceApp */

#endif /* __THREAD_LOCK_H__ */
