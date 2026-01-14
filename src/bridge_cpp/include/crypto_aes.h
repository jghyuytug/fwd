/*
 * crypto_aes.h - AES 加密算法实现
 *
 * AES (Advanced Encryption Standard)
 * - 块大小: 16 字节
 * - 密钥长度: 128/192/256 位
 */

#ifndef __CRYPTO_AES_H__
#define __CRYPTO_AES_H__

#include "crypto_interface.h"

namespace ChannelServiceApp {

/* AES 常量 */
const int AES_BLOCK_SIZE = 16;
const int AES_KEY_SIZE_128 = 16;
const int AES_KEY_SIZE_192 = 24;
const int AES_KEY_SIZE_256 = 32;
const int AES_MAX_ROUNDS = 14;

/*
 * AES 上下文结构
 */
struct AES_KEY {
    uint32_t rd_key[4 * (AES_MAX_ROUNDS + 1)];
    int rounds;
};

/*
 * CryptoAES - AES 加密实现
 */
class CryptoAES : public ICryptoGraph {
public:
    CryptoAES();
    virtual ~CryptoAES();

    /* ICryptoGraph 接口实现 */
    virtual bool init(const uint8_t* key, int keylen);
    virtual bool encryptBlock(const uint8_t* in, uint8_t* out);
    virtual bool decryptBlock(const uint8_t* in, uint8_t* out);
    virtual int encrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen);
    virtual int decrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen);
    virtual int getBlockSize() const { return AES_BLOCK_SIZE; }
    virtual int getKeySize() const { return m_keylen; }
    virtual CRYPTO_GRAPH_TYPE getType() const { return CRYPTO_AES; }
    virtual const char* getName() const { return "AES"; }

private:
    /* 密钥扩展 */
    void keyExpansion(const uint8_t* key, int keylen);

    /* SubBytes 变换 */
    void subBytes(uint8_t* state);
    void invSubBytes(uint8_t* state);

    /* ShiftRows 变换 */
    void shiftRows(uint8_t* state);
    void invShiftRows(uint8_t* state);

    /* MixColumns 变换 */
    void mixColumns(uint8_t* state);
    void invMixColumns(uint8_t* state);

    /* AddRoundKey 变换 */
    void addRoundKey(uint8_t* state, int round);

    /* GF(2^8) 乘法 */
    uint8_t gfMul(uint8_t a, uint8_t b);

private:
    AES_KEY m_encKey;
    AES_KEY m_decKey;
    int m_keylen;
    bool m_initialized;
};

} /* namespace ChannelServiceApp */

#endif /* __CRYPTO_AES_H__ */
