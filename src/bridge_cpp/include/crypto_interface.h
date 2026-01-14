/*
 * crypto_interface.h - 加密库接口定义
 *
 * 基于生产版的 ICryptoGraph 和 IChecksum 接口设计
 */

#ifndef __CRYPTO_INTERFACE_H__
#define __CRYPTO_INTERFACE_H__

#include <stdint.h>
#include <cstring>

namespace ChannelServiceApp {

/* 加密算法类型 */
enum CRYPTO_GRAPH_TYPE {
    CRYPTO_NONE = 0,
    CRYPTO_AES,
    CRYPTO_DES,
    CRYPTO_DES3,
    CRYPTO_SEED,
    CRYPTO_XTEA,
    CRYPTO_CAST5,
    CRYPTO_RC6,
    CRYPTO_ANUBIS,
    CRYPTO_KASUMI,
    CRYPTO_KHAZAD,
    CRYPTO_MULTI2,
    CRYPTO_NOEKEON,
    CRYPTO_TWOFISH,
    CRYPTO_SKIPJACK,
    CRYPTO_MAX
};

/* 校验算法类型 */
enum CRYPTO_CHECKSUM_TYPE {
    CHECKSUM_NONE = 0,
    CHECKSUM_MD5,
    CHECKSUM_SHA1,
    CHECKSUM_SHA256,
    CHECKSUM_SHA384,
    CHECKSUM_SHA512,
    CHECKSUM_CRC32,
    CHECKSUM_MAX
};

/* 加密模式 */
enum CRYPTO_MODE {
    MODE_ECB = 0,   /* 电子密码本 */
    MODE_CBC,       /* 密码块链接 */
    MODE_CFB,       /* 密码反馈 */
    MODE_OFB,       /* 输出反馈 */
    MODE_CTR        /* 计数器 */
};

/* 填充模式 */
enum CRYPTO_PADDING {
    PADDING_NONE = 0,
    PADDING_PKCS7,
    PADDING_ZEROS
};

/* 密钥结构 */
struct symmetric_key {
    uint32_t eK[64];    /* 加密轮密钥 */
    uint32_t dK[64];    /* 解密轮密钥 */
    int rounds;         /* 轮数 */
    int keylen;         /* 密钥长度 */
};

/*
 * ICryptoGraph - 加密算法接口
 */
class ICryptoGraph {
public:
    virtual ~ICryptoGraph() {}

    /* 初始化密钥 */
    virtual bool init(const uint8_t* key, int keylen) = 0;

    /* 加密单个块 */
    virtual bool encryptBlock(const uint8_t* in, uint8_t* out) = 0;

    /* 解密单个块 */
    virtual bool decryptBlock(const uint8_t* in, uint8_t* out) = 0;

    /* 加密数据 (支持填充) */
    virtual int encrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen) = 0;

    /* 解密数据 */
    virtual int decrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen) = 0;

    /* 获取块大小 */
    virtual int getBlockSize() const = 0;

    /* 获取密钥大小 */
    virtual int getKeySize() const = 0;

    /* 获取算法类型 */
    virtual CRYPTO_GRAPH_TYPE getType() const = 0;

    /* 获取算法名称 */
    virtual const char* getName() const = 0;
};

/*
 * IChecksum - 校验算法接口
 */
class IChecksum {
public:
    virtual ~IChecksum() {}

    /* 初始化 */
    virtual void init() = 0;

    /* 更新数据 */
    virtual void update(const uint8_t* data, int len) = 0;

    /* 完成并获取结果 */
    virtual void finalize(uint8_t* digest) = 0;

    /* 一次性计算 */
    virtual void compute(const uint8_t* data, int len, uint8_t* digest) = 0;

    /* 获取摘要长度 */
    virtual int getDigestSize() const = 0;

    /* 获取算法类型 */
    virtual CRYPTO_CHECKSUM_TYPE getType() const = 0;

    /* 获取算法名称 */
    virtual const char* getName() const = 0;
};

/*
 * 工厂函数
 */
ICryptoGraph* CreateCryptoGraph(CRYPTO_GRAPH_TYPE type);
void DestroyCryptoGraph(ICryptoGraph*& crypto);

IChecksum* CreateCryptoChecksum(CRYPTO_CHECKSUM_TYPE type);
void DestroyCryptoChecksum(IChecksum*& checksum);

/* 错误处理 */
uint32_t NCryptoGetLastError();
void NCryptoSetLastError(uint32_t error);

/* 便捷函数 */
bool AES_Encrypt(void* key, uint8_t* data);
bool AES_Decrypt(void* key, uint8_t* data);
bool DES_Encrypt(void* key, uint8_t* data);
bool DES_Decrypt(void* key, uint8_t* data);
bool DES3_Encrypt(void* key, uint8_t* data);
bool DES3_Decrypt(void* key, uint8_t* data);
bool SeedEncrypt(uint8_t* data, uint32_t* key);
bool SeedDecrypt(uint8_t* data, uint32_t* key);

} /* namespace ChannelServiceApp */

#endif /* __CRYPTO_INTERFACE_H__ */
