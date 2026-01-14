/*
 * tcp_handler.cpp - TCP 协议处理器实现
 */

#include "../include/tcp_handler.h"
#include "../include/file_log.h"
#include <cstring>

namespace ChannelServiceApp {

/*
 * TCPHandler
 */
TCPHandler::TCPHandler() {
    memset(m_handlers, 0, sizeof(m_handlers));
    memset(m_handlersExtra, 0, sizeof(m_handlersExtra));
}

TCPHandler::~TCPHandler() {
}

void TCPHandler::registerHandler(int msgId, MessageHandler handler) {
    if (msgId >= 0 && msgId < MAX_MESSAGE_HANDLERS) {
        m_handlers[msgId] = handler;
    }
}

void TCPHandler::registerHandlerExtra(int msgId, MessageHandlerExtra handler) {
    if (msgId >= 0 && msgId < MAX_MESSAGE_HANDLERS) {
        m_handlersExtra[msgId] = handler;
    }
}

MessageHandler TCPHandler::GetMessageHandler(int msgId) {
    if (msgId >= 0 && msgId < MAX_MESSAGE_HANDLERS) {
        return m_handlers[msgId];
    }
    return NULL;
}

MessageHandlerExtra TCPHandler::GetMessageHandlerExtra(int msgId) {
    if (msgId >= 0 && msgId < MAX_MESSAGE_HANDLERS) {
        return m_handlersExtra[msgId];
    }
    return NULL;
}

/*
 * TCPHandlerRelay
 */
TCPHandlerRelay::TCPHandlerRelay()
    : m_handler(NULL)
{
}

TCPHandlerRelay::~TCPHandlerRelay() {
}

int TCPHandlerRelay::dispatch(TCPUser* user, char* data, int len, int offset) {
    if (!user || !data || len < PACKET_HEADER_SIZE) {
        return -1;
    }

    /* 解析数据包头 */
    tagPacketHeader header;
    if (!header.parse(reinterpret_cast<uint8_t*>(data + offset), len - offset)) {
        LOG_WARN("Failed to parse packet header");
        return -1;
    }

    /* 验证头部 */
    if (!header.validate()) {
        LOG_WARN("Invalid packet header");
        return -1;
    }

    LOG_INFO("Dispatch: class=%d msg=%d len=%d seq=%u",
            header.getCategory(), header.getPacketID(),
            header.getSize(), header.getSequence());

    /* 查找处理器 */
    if (!m_handler) {
        LOG_WARN("No handler registered");
        return -1;
    }

    int msgId = header.getPacketID();
    int category = header.getCategory();

    /* 根据分类选择处理方式 */
    if (category == 0) {
        /* 内部消息，使用 Extra 处理器 */
        MessageHandlerExtra handlerExtra = m_handler->GetMessageHandlerExtra(msgId);
        if (handlerExtra) {
            /* 传递原始数据指针和数据部分长度 */
            const uint8_t* rawData = reinterpret_cast<const uint8_t*>(data + offset);
            int dataLen = header.getSize() - PACKET_HEADER_SIZE;
            return handlerExtra(&header, rawData + PACKET_HEADER_SIZE, dataLen);
        }
    }

    /* 普通消息 */
    MessageHandler handler = m_handler->GetMessageHandler(msgId);
    if (handler) {
        return handler(&header, user);
    }

    LOG_WARN("No handler for message: class=%d msg=%d", category, msgId);
    return -1;
}

/*
 * UDPHandler
 */
UDPHandler::UDPHandler() {
}

UDPHandler::~UDPHandler() {
}

} /* namespace ChannelServiceApp */
