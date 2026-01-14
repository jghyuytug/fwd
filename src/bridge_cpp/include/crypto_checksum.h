/*
 * crypto_checksum.h - 校验和算法实现
 *
 * MD5, SHA1, SHA256, SHA384, SHA512, CRC32
 */

#ifndef __CRYPTO_CHECKSUM_H__
#define __CRYPTO_CHECKSUM_H__

#include "crypto_interface.h"

namespace ChannelServiceApp {

/* 摘要长度常量 */
const int MD5_DIGEST_SIZE = 16;
const int SHA1_DIGEST_SIZE = 20;
const int SHA256_DIGEST_SIZE = 32;
const int SHA384_DIGEST_SIZE = 48;
const int SHA512_DIGEST_SIZE = 64;
const int CRC32_DIGEST_SIZE = 4;

/*
 * ChecksumMD5 - MD5 实现
 */
class ChecksumMD5 : public IChecksum {
public:
    ChecksumMD5();
    virtual ~ChecksumMD5();

    virtual void init();
    virtual void update(const uint8_t* data, int len);
    virtual void finalize(uint8_t* digest);
    virtual void compute(const uint8_t* data, int len, uint8_t* digest);
    virtual int getDigestSize() const { return MD5_DIGEST_SIZE; }
    virtual CRYPTO_CHECKSUM_TYPE getType() const { return CHECKSUM_MD5; }
    virtual const char* getName() const { return "MD5"; }

private:
    void transform(const uint8_t* block);

    uint32_t m_state[4];
    uint64_t m_count;
    uint8_t m_buffer[64];
};

/*
 * ChecksumSHA256 - SHA256 实现
 */
class ChecksumSHA256 : public IChecksum {
public:
    ChecksumSHA256();
    virtual ~ChecksumSHA256();

    virtual void init();
    virtual void update(const uint8_t* data, int len);
    virtual void finalize(uint8_t* digest);
    virtual void compute(const uint8_t* data, int len, uint8_t* digest);
    virtual int getDigestSize() const { return SHA256_DIGEST_SIZE; }
    virtual CRYPTO_CHECKSUM_TYPE getType() const { return CHECKSUM_SHA256; }
    virtual const char* getName() const { return "SHA256"; }

private:
    void transform(const uint8_t* block);

    uint32_t m_state[8];
    uint64_t m_count;
    uint8_t m_buffer[64];
};

/*
 * ChecksumSHA512 - SHA512 实现
 */
class ChecksumSHA512 : public IChecksum {
public:
    ChecksumSHA512();
    virtual ~ChecksumSHA512();

    virtual void init();
    virtual void update(const uint8_t* data, int len);
    virtual void finalize(uint8_t* digest);
    virtual void compute(const uint8_t* data, int len, uint8_t* digest);
    virtual int getDigestSize() const { return SHA512_DIGEST_SIZE; }
    virtual CRYPTO_CHECKSUM_TYPE getType() const { return CHECKSUM_SHA512; }
    virtual const char* getName() const { return "SHA512"; }

private:
    void transform(const uint8_t* block);

    uint64_t m_state[8];
    uint64_t m_count[2];
    uint8_t m_buffer[128];
};

/*
 * ChecksumCRC32 - CRC32 实现
 */
class ChecksumCRC32 : public IChecksum {
public:
    ChecksumCRC32();
    virtual ~ChecksumCRC32();

    virtual void init();
    virtual void update(const uint8_t* data, int len);
    virtual void finalize(uint8_t* digest);
    virtual void compute(const uint8_t* data, int len, uint8_t* digest);
    virtual int getDigestSize() const { return CRC32_DIGEST_SIZE; }
    virtual CRYPTO_CHECKSUM_TYPE getType() const { return CHECKSUM_CRC32; }
    virtual const char* getName() const { return "CRC32"; }

    /* 获取 CRC32 值 */
    uint32_t getValue() const { return ~m_crc; }

private:
    static void initTable();
    static uint32_t s_table[256];
    static bool s_tableInit;

    uint32_t m_crc;
};

} /* namespace ChannelServiceApp */

#endif /* __CRYPTO_CHECKSUM_H__ */
