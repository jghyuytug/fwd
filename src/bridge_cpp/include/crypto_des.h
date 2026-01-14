/*
 * crypto_des.h - DES/3DES 加密算法实现
 *
 * DES (Data Encryption Standard)
 * - 块大小: 8 字节
 * - 密钥长度: 56 位 (64位含奇偶校验)
 *
 * 3DES (Triple DES)
 * - 块大小: 8 字节
 * - 密钥长度: 168 位 (192位含奇偶校验)
 */

#ifndef __CRYPTO_DES_H__
#define __CRYPTO_DES_H__

#include "crypto_interface.h"

namespace ChannelServiceApp {

/* DES 常量 */
const int DES_BLOCK_SIZE = 8;
const int DES_KEY_SIZE = 8;
const int DES3_KEY_SIZE = 24;

/*
 * DES 密钥结构
 */
struct DES_KEY {
    uint32_t sk[32];    /* 子密钥 */
};

/*
 * CryptoDES - DES 加密实现
 */
class CryptoDES : public ICryptoGraph {
public:
    CryptoDES();
    virtual ~CryptoDES();

    /* ICryptoGraph 接口实现 */
    virtual bool init(const uint8_t* key, int keylen);
    virtual bool encryptBlock(const uint8_t* in, uint8_t* out);
    virtual bool decryptBlock(const uint8_t* in, uint8_t* out);
    virtual int encrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen);
    virtual int decrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen);
    virtual int getBlockSize() const { return DES_BLOCK_SIZE; }
    virtual int getKeySize() const { return DES_KEY_SIZE; }
    virtual CRYPTO_GRAPH_TYPE getType() const { return CRYPTO_DES; }
    virtual const char* getName() const { return "DES"; }

protected:
    /* 密钥调度 */
    void setKey(const uint8_t* key);

    /* DES 核心函数 */
    void desCore(uint32_t* block, int encrypt);

    /* 初始/逆初始置换 */
    void initialPermutation(const uint8_t* in, uint32_t* out);
    void finalPermutation(const uint32_t* in, uint8_t* out);

    /* Feistel 函数 */
    uint32_t feistel(uint32_t r, int round);

protected:
    DES_KEY m_key;
    bool m_initialized;
};

/*
 * CryptoDES3 - 3DES 加密实现
 */
class CryptoDES3 : public ICryptoGraph {
public:
    CryptoDES3();
    virtual ~CryptoDES3();

    /* ICryptoGraph 接口实现 */
    virtual bool init(const uint8_t* key, int keylen);
    virtual bool encryptBlock(const uint8_t* in, uint8_t* out);
    virtual bool decryptBlock(const uint8_t* in, uint8_t* out);
    virtual int encrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen);
    virtual int decrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen);
    virtual int getBlockSize() const { return DES_BLOCK_SIZE; }
    virtual int getKeySize() const { return DES3_KEY_SIZE; }
    virtual CRYPTO_GRAPH_TYPE getType() const { return CRYPTO_DES3; }
    virtual const char* getName() const { return "3DES"; }

private:
    CryptoDES m_des1;
    CryptoDES m_des2;
    CryptoDES m_des3;
    bool m_initialized;
};

} /* namespace ChannelServiceApp */

#endif /* __CRYPTO_DES_H__ */
