/*
 * tcp_accept_thread.cpp - TCP 接受线程实现
 */

#include "../include/tcp_accept_thread.h"
#include "../include/file_log.h"
#include <errno.h>

namespace ChannelServiceApp {

TCPAcceptThread::TCPAcceptThread()
    : m_thread(0)
    , m_listenSocket(NULL)
    , m_running(false)
{
}

TCPAcceptThread::~TCPAcceptThread() {
    stop();
}

bool TCPAcceptThread::start(TCPSocket* listenSocket) {
    if (!listenSocket || !listenSocket->isValid()) {
        LOG_ERROR("Invalid listen socket");
        return false;
    }

    m_listenSocket = listenSocket;
    m_running = true;

    int ret = pthread_create(&m_thread, NULL, threadFunc, this);
    if (ret != 0) {
        LOG_ERROR("Failed to create accept thread: %d", ret);
        m_running = false;
        return false;
    }

    LOG_INFO("Accept thread started");
    return true;
}

void TCPAcceptThread::stop() {
    if (!m_running) {
        return;
    }

    m_running = false;

    /* 等待线程结束 */
    if (m_thread) {
        pthread_join(m_thread, NULL);
        m_thread = 0;
    }

    LOG_INFO("Accept thread stopped");
}

void TCPAcceptThread::lockPushAcceptedUser(TCPUser* user) {
    ScopedLinuxLock guard(m_queueLock);
    m_acceptQueue.push(user);
}

TCPUser* TCPAcceptThread::lockPopAcceptedUser() {
    ScopedLinuxLock guard(m_queueLock);

    if (m_acceptQueue.empty()) {
        return NULL;
    }

    TCPUser* user = m_acceptQueue.front();
    m_acceptQueue.pop();
    return user;
}

int TCPAcceptThread::getQueueSize() {
    ScopedLinuxLock guard(m_queueLock);
    return static_cast<int>(m_acceptQueue.size());
}

void TCPAcceptThread::notifyCannotCreateUser(TCPSocket& socket) {
    LOG_WARN("Cannot create user for socket: %s:%u",
            socket.getPeerAdrs(), socket.getPeerPort());
    socket.close();
}

void TCPAcceptThread::notifyCannotLoginByMaxUserCount(TCPSocket& socket) {
    LOG_WARN("Max user count reached, rejecting: %s:%u",
            socket.getPeerAdrs(), socket.getPeerPort());
    socket.close();
}

void* TCPAcceptThread::threadFunc(void* arg) {
    TCPAcceptThread* self = static_cast<TCPAcceptThread*>(arg);
    self->run();
    return NULL;
}

void TCPAcceptThread::run() {
    LOG_INFO("Accept thread running");

    while (m_running) {
        if (!m_listenSocket || !m_listenSocket->isValid()) {
            break;
        }

        /* 接受连接 */
        struct sockaddr_in clientAddr;
        int clientFd = m_listenSocket->accept(&clientAddr);

        if (clientFd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* 非阻塞模式下没有连接 */
                usleep(10000);  /* 10ms */
                continue;
            }
            if (errno == EINTR) {
                continue;
            }
            LOG_ERROR("Accept failed: %s", strerror(errno));
            break;
        }

        LOG_INFO("Accepted connection from %s:%u",
                inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        /* 创建套接字对象 */
        TCPSocket* socket = new TCPSocket();
        socket->setFd(clientFd);
        socket->setPeerInfo(clientAddr);
        socket->setOptNonBlock();

        /* 创建用户对象 */
        TCPUser* user = new TCPUser();
        user->setSocket(socket);
        user->startupAfterSetSocket();
        user->onAccept();

        /* 加入接受队列 */
        lockPushAcceptedUser(user);
    }

    LOG_INFO("Accept thread exiting");
}

} /* namespace ChannelServiceApp */
