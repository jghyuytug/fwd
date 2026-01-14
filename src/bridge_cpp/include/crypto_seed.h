/*
 * crypto_seed.h - SEED 加密算法实现
 *
 * SEED (韩国标准加密算法)
 * - 块大小: 16 字节
 * - 密钥长度: 128 位
 */

#ifndef __CRYPTO_SEED_H__
#define __CRYPTO_SEED_H__

#include "crypto_interface.h"

namespace ChannelServiceApp {

/* SEED 常量 */
const int SEED_BLOCK_SIZE = 16;
const int SEED_KEY_SIZE = 16;
const int SEED_ROUNDS = 16;

/*
 * CryptoSEED - SEED 加密实现
 */
class CryptoSEED : public ICryptoGraph {
public:
    CryptoSEED();
    virtual ~CryptoSEED();

    /* ICryptoGraph 接口实现 */
    virtual bool init(const uint8_t* key, int keylen);
    virtual bool encryptBlock(const uint8_t* in, uint8_t* out);
    virtual bool decryptBlock(const uint8_t* in, uint8_t* out);
    virtual int encrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen);
    virtual int decrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen);
    virtual int getBlockSize() const { return SEED_BLOCK_SIZE; }
    virtual int getKeySize() const { return SEED_KEY_SIZE; }
    virtual CRYPTO_GRAPH_TYPE getType() const { return CRYPTO_SEED; }
    virtual const char* getName() const { return "SEED"; }

private:
    /* 密钥扩展 */
    void keySchedule(const uint8_t* key);

    /* G 函数 */
    uint32_t G(uint32_t x);

    /* F 函数 */
    void F(uint32_t* T0, uint32_t* T1, uint32_t K0, uint32_t K1);

private:
    uint32_t m_roundKey[32];    /* 轮密钥 */
    bool m_initialized;
};

} /* namespace ChannelServiceApp */

#endif /* __CRYPTO_SEED_H__ */
