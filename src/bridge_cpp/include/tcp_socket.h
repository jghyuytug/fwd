/*
 * tcp_socket.h - TCP 套接字封装
 *
 * 基于生产版的 TCPSocket 设计:
 * - 套接字生命周期管理
 * - 非阻塞 I/O
 * - 套接字选项设置
 */

#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>

namespace ChannelServiceApp {

/*
 * SocketSystemLinux - Linux 套接字系统初始化
 */
class SocketSystemLinux {
public:
    static bool startup() {
        /* Linux 不需要特殊初始化 */
        return true;
    }

    static void shutdown() {
        /* Linux 不需要特殊清理 */
    }
};

/*
 * TCPSocket - TCP 套接字封装
 */
class TCPSocket {
public:
    TCPSocket()
        : m_fd(-1)
        , m_peerPort(0)
    {
        memset(&m_peerAddr, 0, sizeof(m_peerAddr));
    }

    ~TCPSocket() {
        close();
    }

    /*
     * open - 创建套接字
     */
    bool open() {
        m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
        return m_fd >= 0;
    }

    /*
     * close - 关闭套接字
     */
    void close() {
        if (m_fd >= 0) {
            ::close(m_fd);
            m_fd = -1;
        }
    }

    /*
     * bind - 绑定地址和端口
     */
    bool bind(uint16_t port, bool reuseAddr = true) {
        if (reuseAddr) {
            setOptReuseAdrs(true);
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        return ::bind(m_fd, (struct sockaddr*)&addr, sizeof(addr)) == 0;
    }

    /*
     * bind - 绑定到指定 IP 和端口
     */
    bool bind(const char* ip, uint16_t port, bool reuseAddr = true) {
        if (reuseAddr) {
            setOptReuseAdrs(true);
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip);
        addr.sin_port = htons(port);

        return ::bind(m_fd, (struct sockaddr*)&addr, sizeof(addr)) == 0;
    }

    /*
     * listen - 开始监听
     */
    bool listen(int backlog = 128) {
        return ::listen(m_fd, backlog) == 0;
    }

    /*
     * accept - 接受连接
     */
    int accept(struct sockaddr_in* clientAddr = NULL) {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);

        int clientFd = ::accept(m_fd, (struct sockaddr*)&addr, &len);
        if (clientFd >= 0 && clientAddr) {
            *clientAddr = addr;
        }
        return clientFd;
    }

    /*
     * connect - 连接到服务器
     */
    bool connect(const char* ip, uint16_t port) {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip);
        addr.sin_port = htons(port);

        return ::connect(m_fd, (struct sockaddr*)&addr, sizeof(addr)) == 0;
    }

    /*
     * recv - 接收数据
     * 返回: 接收的字节数，-1 错误，0 连接关闭
     */
    int recv(char* buf, int len) {
        return ::recv(m_fd, buf, len, 0);
    }

    /*
     * send - 发送数据
     */
    int send(const char* buf, int len) {
        return ::send(m_fd, buf, len, MSG_NOSIGNAL);
    }

    /*
     * setOptNonBlock - 设置非阻塞模式
     */
    bool setOptNonBlock() {
        int flags = fcntl(m_fd, F_GETFL, 0);
        if (flags < 0) {
            return false;
        }
        return fcntl(m_fd, F_SETFL, flags | O_NONBLOCK) == 0;
    }

    /*
     * setOptReuseAdrs - 设置地址重用
     */
    bool setOptReuseAdrs(bool enable) {
        int optval = enable ? 1 : 0;
        return setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR,
                         &optval, sizeof(optval)) == 0;
    }

    /*
     * setOptLinger - 设置 linger 选项
     */
    bool setOptLinger(bool enable) {
        struct linger ling;
        ling.l_onoff = enable ? 1 : 0;
        ling.l_linger = 0;
        return setsockopt(m_fd, SOL_SOCKET, SO_LINGER,
                         &ling, sizeof(ling)) == 0;
    }

    /*
     * setOptResizeRecvBuf - 调整接收缓冲区大小
     */
    bool setOptResizeRecvBuf(int size) {
        return setsockopt(m_fd, SOL_SOCKET, SO_RCVBUF,
                         &size, sizeof(size)) == 0;
    }

    /*
     * setOptResizeSendBuf - 调整发送缓冲区大小
     */
    bool setOptResizeSendBuf(int size) {
        return setsockopt(m_fd, SOL_SOCKET, SO_SNDBUF,
                         &size, sizeof(size)) == 0;
    }

    /*
     * setOptNoDelay - 设置 TCP_NODELAY (禁用 Nagle 算法)
     */
    bool setOptNoDelay(bool enable) {
        int optval = enable ? 1 : 0;
        return setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY,
                         &optval, sizeof(optval)) == 0;
    }

    /*
     * setOptKeepAlive - 设置 TCP keepalive
     */
    bool setOptKeepAlive(bool enable) {
        int optval = enable ? 1 : 0;
        return setsockopt(m_fd, SOL_SOCKET, SO_KEEPALIVE,
                         &optval, sizeof(optval)) == 0;
    }

    /*
     * getSocket - 获取套接字 (别名 getFd)
     */
    int getSocket() const {
        return m_fd;
    }

    /*
     * setFd - 设置文件描述符(用于 accept 后的套接字)
     */
    void setFd(int fd) {
        m_fd = fd;
    }

    /*
     * getFd - 获取文件描述符
     */
    int getFd() const {
        return m_fd;
    }

    /*
     * setPeerInfo - 设置对端信息
     */
    void setPeerInfo(const struct sockaddr_in& addr) {
        m_peerAddr = addr;
        m_peerPort = ntohs(addr.sin_port);
    }

    /*
     * getPeerAdrs - 获取对端地址
     */
    const char* getPeerAdrs() {
        return inet_ntoa(m_peerAddr.sin_addr);
    }

    /*
     * getPeerPort - 获取对端端口
     */
    uint16_t getPeerPort() const {
        return m_peerPort;
    }

    /*
     * isValid - 检查套接字是否有效
     */
    bool isValid() const {
        return m_fd >= 0;
    }

    /*
     * reset - 重置套接字状态(用于内存池复用)
     */
    void reset() {
        m_fd = -1;
        m_peerPort = 0;
        memset(&m_peerAddr, 0, sizeof(m_peerAddr));
    }

private:
    TCPSocket(const TCPSocket&);
    TCPSocket& operator=(const TCPSocket&);

    int m_fd;                       /* 文件描述符 */
    struct sockaddr_in m_peerAddr;  /* 对端地址 */
    uint16_t m_peerPort;            /* 对端端口 */
};

/*
 * TSocketSystem - 套接字系统模板
 */
template<typename SystemType>
class TSocketSystem {
public:
    static bool startup() {
        return SystemType::startup();
    }

    static void shutdown() {
        SystemType::shutdown();
    }
};

} /* namespace ChannelServiceApp */

#endif /* __TCP_SOCKET_H__ */
