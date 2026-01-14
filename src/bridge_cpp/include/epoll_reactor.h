/*
 * epoll_reactor.h - Epoll 事件反应器
 *
 * 基于生产版的 EpollReactor 和 TReactor 模板设计:
 * - 高性能 epoll 事件处理
 * - 模板化以支持不同的用户类型
 */

#ifndef __EPOLL_REACTOR_H__
#define __EPOLL_REACTOR_H__

#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include "thread_lock.h"

namespace ChannelServiceApp {

/* 事件类型定义 */
enum EventType {
    EVENT_READ   = EPOLLIN,
    EVENT_WRITE  = EPOLLOUT,
    EVENT_ERROR  = EPOLLERR,
    EVENT_HUP    = EPOLLHUP,
    EVENT_ET     = EPOLLET
};

/*
 * EpollReactor - Epoll 事件反应器模板
 *
 * @UserType: 用户连接类型，必须有 getHandle() 方法
 */
template<typename UserType>
class EpollReactor {
public:
    static const int DEFAULT_MAX_EVENTS = 256;

    EpollReactor()
        : m_epollFd(-1)
        , m_maxEvents(DEFAULT_MAX_EVENTS)
        , m_events(NULL)
    {}

    ~EpollReactor() {
        shutdown();
    }

    /*
     * init - 初始化 epoll
     * @maxEvents: 最大事件数
     */
    bool init(int maxEvents = DEFAULT_MAX_EVENTS) {
        m_maxEvents = maxEvents;

        m_epollFd = epoll_create1(0);
        if (m_epollFd < 0) {
            return false;
        }

        m_events = new struct epoll_event[m_maxEvents];
        if (!m_events) {
            close(m_epollFd);
            m_epollFd = -1;
            return false;
        }

        memset(m_events, 0, sizeof(struct epoll_event) * m_maxEvents);
        return true;
    }

    /*
     * startup - 启动反应器
     */
    bool startup() {
        return m_epollFd >= 0;
    }

    /*
     * shutdown - 关闭反应器
     */
    void shutdown() {
        if (m_epollFd >= 0) {
            ::close(m_epollFd);
            m_epollFd = -1;
        }
        if (m_events) {
            delete[] m_events;
            m_events = NULL;
        }
    }

    /*
     * registHandle - 注册句柄
     * @user: 用户对象指针
     * @events: 事件掩码
     */
    bool registHandle(UserType* user, unsigned int events) {
        if (!user || m_epollFd < 0) {
            return false;
        }

        struct epoll_event ev;
        memset(&ev, 0, sizeof(ev));
        ev.events = events;
        ev.data.ptr = user;

        return epoll_ctl(m_epollFd, EPOLL_CTL_ADD,
                        user->getHandle(), &ev) == 0;
    }

    /*
     * modifyHandle - 修改句柄事件
     */
    bool modifyHandle(UserType* user, unsigned int events) {
        if (!user || m_epollFd < 0) {
            return false;
        }

        struct epoll_event ev;
        memset(&ev, 0, sizeof(ev));
        ev.events = events;
        ev.data.ptr = user;

        return epoll_ctl(m_epollFd, EPOLL_CTL_MOD,
                        user->getHandle(), &ev) == 0;
    }

    /*
     * unregistHandle - 注销句柄
     */
    bool unregistHandle(UserType* user) {
        if (!user || m_epollFd < 0) {
            return false;
        }

        return epoll_ctl(m_epollFd, EPOLL_CTL_DEL,
                        user->getHandle(), NULL) == 0;
    }

    /*
     * handleEvents - 处理事件
     * @timeout: 超时毫秒数
     * 返回: 就绪事件数，-1 表示错误
     */
    int handleEvents(unsigned int timeout) {
        if (m_epollFd < 0 || !m_events) {
            return -1;
        }

        return epoll_wait(m_epollFd, m_events, m_maxEvents, timeout);
    }

    /*
     * getEvent - 获取指定索引的事件
     */
    struct epoll_event* getEvent(int index) {
        if (index < 0 || index >= m_maxEvents) {
            return NULL;
        }
        return &m_events[index];
    }

    /*
     * getEventUser - 获取事件对应的用户对象
     */
    UserType* getEventUser(int index) {
        struct epoll_event* ev = getEvent(index);
        if (!ev) {
            return NULL;
        }
        return static_cast<UserType*>(ev->data.ptr);
    }

    /*
     * getEventMask - 获取事件掩码
     */
    unsigned int getEventMask(int index) {
        struct epoll_event* ev = getEvent(index);
        if (!ev) {
            return 0;
        }
        return ev->events;
    }

    /*
     * getNativeEventFilter - 获取原生事件过滤器
     */
    unsigned int getNativeEventFilter(unsigned int events) {
        return events;
    }

    /*
     * getEpollFd - 获取 epoll 文件描述符
     */
    int getEpollFd() const {
        return m_epollFd;
    }

private:
    EpollReactor(const EpollReactor&);
    EpollReactor& operator=(const EpollReactor&);

    int m_epollFd;                  /* epoll 文件描述符 */
    int m_maxEvents;                /* 最大事件数 */
    struct epoll_event* m_events;   /* 事件数组 */
};

/*
 * TReactor - 反应器模板包装
 */
template<typename ReactorImpl, typename UserType>
class TReactor : public ReactorImpl {
public:
    TReactor() : ReactorImpl() {}

    bool init(int maxEvents) {
        return ReactorImpl::init(maxEvents);
    }

    bool registHandle(UserType* user, unsigned int events) {
        return ReactorImpl::registHandle(user, events);
    }

    bool unregistHandle(UserType* user) {
        return ReactorImpl::unregistHandle(user);
    }

    int handleEvents(unsigned int timeout) {
        return ReactorImpl::handleEvents(timeout);
    }
};

} /* namespace ChannelServiceApp */

#endif /* __EPOLL_REACTOR_H__ */
