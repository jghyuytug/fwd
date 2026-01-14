/*
 * tcp_handler.h - TCP 协议处理器
 *
 * 基于生产版的 TCPHandler 和 TCPHandlerRelay 设计:
 * - 协议分发
 * - 消息处理器注册
 */

#ifndef __TCP_HANDLER_H__
#define __TCP_HANDLER_H__

#include "tcp_user.h"
#include "packet_header.h"

namespace ChannelServiceApp {

/* 消息处理器函数类型 */
typedef int (*MessageHandler)(tagPacketHeader* header, TCPUser* user);
typedef int (*MessageHandlerExtra)(tagPacketHeader* header, const uint8_t* data, int dataLen);

/* 最大消息类型数 */
const int MAX_MESSAGE_HANDLERS = 256;

/*
 * TCPHandler - TCP 协议处理器基类
 */
class TCPHandler {
public:
    TCPHandler();
    virtual ~TCPHandler();

    /*
     * registerHandler - 注册消息处理器
     * @msgId: 消息 ID
     * @handler: 处理函数
     */
    void registerHandler(int msgId, MessageHandler handler);
    void registerHandlerExtra(int msgId, MessageHandlerExtra handler);

    /*
     * GetMessageHandler - 获取消息处理器
     */
    MessageHandler GetMessageHandler(int msgId);
    MessageHandlerExtra GetMessageHandlerExtra(int msgId);

protected:
    MessageHandler m_handlers[MAX_MESSAGE_HANDLERS];
    MessageHandlerExtra m_handlersExtra[MAX_MESSAGE_HANDLERS];
};

/*
 * TCPHandlerRelay - TCP 消息分发器
 *
 * 负责解析消息头并分发到对应的处理器
 */
class TCPHandlerRelay {
public:
    TCPHandlerRelay();
    virtual ~TCPHandlerRelay();

    /*
     * dispatch - 分发消息
     * @user: 用户对象
     * @data: 消息数据
     * @len: 数据长度
     * @offset: 数据偏移
     * 返回: 处理结果
     */
    int dispatch(TCPUser* user, char* data, int len, int offset);

    /*
     * setHandler - 设置协议处理器
     */
    void setHandler(TCPHandler* handler) { m_handler = handler; }

protected:
    TCPHandler* m_handler;
};

/*
 * UDPHandler - UDP 协议处理器
 */
class UDPHandler {
public:
    UDPHandler();
    virtual ~UDPHandler();
};

} /* namespace ChannelServiceApp */

#endif /* __TCP_HANDLER_H__ */
