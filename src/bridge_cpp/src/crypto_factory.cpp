/*
 * crypto_factory.cpp - 加密库工厂函数实现
 */

#include "../include/crypto_interface.h"
#include "../include/crypto_aes.h"
#include "../include/crypto_des.h"
#include "../include/crypto_seed.h"
#include "../include/crypto_checksum.h"

namespace ChannelServiceApp {

/* 全局错误码 */
static uint32_t g_cryptoError = 0;

/*
 * CreateCryptoGraph - 创建加密算法实例
 */
ICryptoGraph* CreateCryptoGraph(CRYPTO_GRAPH_TYPE type) {
    switch (type) {
        case CRYPTO_AES:
            return new CryptoAES();
        case CRYPTO_DES:
            return new CryptoDES();
        case CRYPTO_DES3:
            return new CryptoDES3();
        case CRYPTO_SEED:
            return new CryptoSEED();
        /* 其他算法待实现 */
        case CRYPTO_XTEA:
        case CRYPTO_CAST5:
        case CRYPTO_RC6:
        case CRYPTO_ANUBIS:
        case CRYPTO_KASUMI:
        case CRYPTO_KHAZAD:
        case CRYPTO_MULTI2:
        case CRYPTO_NOEKEON:
        case CRYPTO_TWOFISH:
        case CRYPTO_SKIPJACK:
        default:
            NCryptoSetLastError(1);  /* 不支持的算法 */
            return NULL;
    }
}

/*
 * DestroyCryptoGraph - 销毁加密算法实例
 */
void DestroyCryptoGraph(ICryptoGraph*& crypto) {
    if (crypto) {
        delete crypto;
        crypto = NULL;
    }
}

/*
 * CreateCryptoChecksum - 创建校验算法实例
 */
IChecksum* CreateCryptoChecksum(CRYPTO_CHECKSUM_TYPE type) {
    switch (type) {
        case CHECKSUM_MD5:
            return new ChecksumMD5();
        case CHECKSUM_SHA256:
            return new ChecksumSHA256();
        case CHECKSUM_SHA512:
            return new ChecksumSHA512();
        case CHECKSUM_CRC32:
            return new ChecksumCRC32();
        /* 其他算法待实现 */
        case CHECKSUM_SHA1:
        case CHECKSUM_SHA384:
        default:
            NCryptoSetLastError(1);
            return NULL;
    }
}

/*
 * DestroyCryptoChecksum - 销毁校验算法实例
 */
void DestroyCryptoChecksum(IChecksum*& checksum) {
    if (checksum) {
        delete checksum;
        checksum = NULL;
    }
}

/*
 * 错误处理
 */
uint32_t NCryptoGetLastError() {
    return g_cryptoError;
}

void NCryptoSetLastError(uint32_t error) {
    g_cryptoError = error;
}

/*
 * 便捷加密函数
 */
bool AES_Encrypt(void* key, uint8_t* data) {
    CryptoAES aes;
    if (!aes.init(static_cast<uint8_t*>(key), 16)) {
        return false;
    }
    return aes.encryptBlock(data, data);
}

bool AES_Decrypt(void* key, uint8_t* data) {
    CryptoAES aes;
    if (!aes.init(static_cast<uint8_t*>(key), 16)) {
        return false;
    }
    return aes.decryptBlock(data, data);
}

bool DES_Encrypt(void* key, uint8_t* data) {
    CryptoDES des;
    if (!des.init(static_cast<uint8_t*>(key), 8)) {
        return false;
    }
    return des.encryptBlock(data, data);
}

bool DES_Decrypt(void* key, uint8_t* data) {
    CryptoDES des;
    if (!des.init(static_cast<uint8_t*>(key), 8)) {
        return false;
    }
    return des.decryptBlock(data, data);
}

bool DES3_Encrypt(void* key, uint8_t* data) {
    CryptoDES3 des3;
    if (!des3.init(static_cast<uint8_t*>(key), 24)) {
        return false;
    }
    return des3.encryptBlock(data, data);
}

bool DES3_Decrypt(void* key, uint8_t* data) {
    CryptoDES3 des3;
    if (!des3.init(static_cast<uint8_t*>(key), 24)) {
        return false;
    }
    return des3.decryptBlock(data, data);
}

bool SeedEncrypt(uint8_t* data, uint32_t* key) {
    CryptoSEED seed;
    if (!seed.init(reinterpret_cast<uint8_t*>(key), 16)) {
        return false;
    }
    return seed.encryptBlock(data, data);
}

bool SeedDecrypt(uint8_t* data, uint32_t* key) {
    CryptoSEED seed;
    if (!seed.init(reinterpret_cast<uint8_t*>(key), 16)) {
        return false;
    }
    return seed.decryptBlock(data, data);
}

} /* namespace ChannelServiceApp */
