/*
 * crypto_aes.cpp - AES 加密算法实现
 */

#include "../include/crypto_aes.h"
#include <cstring>

namespace ChannelServiceApp {

/* AES S-Box */
static const uint8_t SBOX[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

/* AES 逆 S-Box */
static const uint8_t INV_SBOX[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38,
    0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87,
    0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d,
    0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2,
    0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16,
    0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda,
    0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a,
    0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02,
    0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea,
    0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85,
    0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89,
    0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20,
    0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31,
    0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d,
    0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0,
    0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26,
    0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

/* Rcon 轮常量 */
static const uint8_t RCON[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

CryptoAES::CryptoAES()
    : m_keylen(0)
    , m_initialized(false)
{
    memset(&m_encKey, 0, sizeof(m_encKey));
    memset(&m_decKey, 0, sizeof(m_decKey));
}

CryptoAES::~CryptoAES() {
    memset(&m_encKey, 0, sizeof(m_encKey));
    memset(&m_decKey, 0, sizeof(m_decKey));
}

bool CryptoAES::init(const uint8_t* key, int keylen) {
    if (!key || (keylen != 16 && keylen != 24 && keylen != 32)) {
        return false;
    }

    m_keylen = keylen;
    keyExpansion(key, keylen);
    m_initialized = true;
    return true;
}

void CryptoAES::keyExpansion(const uint8_t* key, int keylen) {
    int Nk = keylen / 4;
    int Nr = Nk + 6;
    int i;

    m_encKey.rounds = Nr;
    m_decKey.rounds = Nr;

    /* 复制原始密钥 */
    for (i = 0; i < Nk; i++) {
        m_encKey.rd_key[i] = (key[4*i] << 24) | (key[4*i+1] << 16) |
                             (key[4*i+2] << 8) | key[4*i+3];
    }

    /* 扩展密钥 */
    for (i = Nk; i < 4 * (Nr + 1); i++) {
        uint32_t temp = m_encKey.rd_key[i - 1];

        if (i % Nk == 0) {
            /* RotWord + SubWord + Rcon */
            temp = ((temp << 8) | (temp >> 24));
            temp = (SBOX[(temp >> 24) & 0xff] << 24) |
                   (SBOX[(temp >> 16) & 0xff] << 16) |
                   (SBOX[(temp >> 8) & 0xff] << 8) |
                   SBOX[temp & 0xff];
            temp ^= (RCON[i / Nk] << 24);
        }
        else if (Nk > 6 && i % Nk == 4) {
            /* SubWord for AES-256 */
            temp = (SBOX[(temp >> 24) & 0xff] << 24) |
                   (SBOX[(temp >> 16) & 0xff] << 16) |
                   (SBOX[(temp >> 8) & 0xff] << 8) |
                   SBOX[temp & 0xff];
        }

        m_encKey.rd_key[i] = m_encKey.rd_key[i - Nk] ^ temp;
    }

    /* 生成解密密钥 (逆序) */
    for (i = 0; i <= Nr; i++) {
        for (int j = 0; j < 4; j++) {
            m_decKey.rd_key[4*i + j] = m_encKey.rd_key[4*(Nr-i) + j];
        }
    }
}

uint8_t CryptoAES::gfMul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) {
            p ^= a;
        }
        bool hi_bit = (a & 0x80) != 0;
        a <<= 1;
        if (hi_bit) {
            a ^= 0x1b;  /* x^8 + x^4 + x^3 + x + 1 */
        }
        b >>= 1;
    }
    return p;
}

void CryptoAES::subBytes(uint8_t* state) {
    for (int i = 0; i < 16; i++) {
        state[i] = SBOX[state[i]];
    }
}

void CryptoAES::invSubBytes(uint8_t* state) {
    for (int i = 0; i < 16; i++) {
        state[i] = INV_SBOX[state[i]];
    }
}

void CryptoAES::shiftRows(uint8_t* state) {
    uint8_t temp;

    /* Row 1: shift left 1 */
    temp = state[1];
    state[1] = state[5];
    state[5] = state[9];
    state[9] = state[13];
    state[13] = temp;

    /* Row 2: shift left 2 */
    temp = state[2];
    state[2] = state[10];
    state[10] = temp;
    temp = state[6];
    state[6] = state[14];
    state[14] = temp;

    /* Row 3: shift left 3 (= shift right 1) */
    temp = state[15];
    state[15] = state[11];
    state[11] = state[7];
    state[7] = state[3];
    state[3] = temp;
}

void CryptoAES::invShiftRows(uint8_t* state) {
    uint8_t temp;

    /* Row 1: shift right 1 */
    temp = state[13];
    state[13] = state[9];
    state[9] = state[5];
    state[5] = state[1];
    state[1] = temp;

    /* Row 2: shift right 2 */
    temp = state[2];
    state[2] = state[10];
    state[10] = temp;
    temp = state[6];
    state[6] = state[14];
    state[14] = temp;

    /* Row 3: shift right 3 (= shift left 1) */
    temp = state[3];
    state[3] = state[7];
    state[7] = state[11];
    state[11] = state[15];
    state[15] = temp;
}

void CryptoAES::mixColumns(uint8_t* state) {
    for (int i = 0; i < 4; i++) {
        uint8_t s0 = state[i*4 + 0];
        uint8_t s1 = state[i*4 + 1];
        uint8_t s2 = state[i*4 + 2];
        uint8_t s3 = state[i*4 + 3];

        state[i*4 + 0] = gfMul(0x02, s0) ^ gfMul(0x03, s1) ^ s2 ^ s3;
        state[i*4 + 1] = s0 ^ gfMul(0x02, s1) ^ gfMul(0x03, s2) ^ s3;
        state[i*4 + 2] = s0 ^ s1 ^ gfMul(0x02, s2) ^ gfMul(0x03, s3);
        state[i*4 + 3] = gfMul(0x03, s0) ^ s1 ^ s2 ^ gfMul(0x02, s3);
    }
}

void CryptoAES::invMixColumns(uint8_t* state) {
    for (int i = 0; i < 4; i++) {
        uint8_t s0 = state[i*4 + 0];
        uint8_t s1 = state[i*4 + 1];
        uint8_t s2 = state[i*4 + 2];
        uint8_t s3 = state[i*4 + 3];

        state[i*4 + 0] = gfMul(0x0e, s0) ^ gfMul(0x0b, s1) ^ gfMul(0x0d, s2) ^ gfMul(0x09, s3);
        state[i*4 + 1] = gfMul(0x09, s0) ^ gfMul(0x0e, s1) ^ gfMul(0x0b, s2) ^ gfMul(0x0d, s3);
        state[i*4 + 2] = gfMul(0x0d, s0) ^ gfMul(0x09, s1) ^ gfMul(0x0e, s2) ^ gfMul(0x0b, s3);
        state[i*4 + 3] = gfMul(0x0b, s0) ^ gfMul(0x0d, s1) ^ gfMul(0x09, s2) ^ gfMul(0x0e, s3);
    }
}

void CryptoAES::addRoundKey(uint8_t* state, int round) {
    for (int i = 0; i < 4; i++) {
        uint32_t k = m_encKey.rd_key[round * 4 + i];
        state[i*4 + 0] ^= (k >> 24) & 0xff;
        state[i*4 + 1] ^= (k >> 16) & 0xff;
        state[i*4 + 2] ^= (k >> 8) & 0xff;
        state[i*4 + 3] ^= k & 0xff;
    }
}

bool CryptoAES::encryptBlock(const uint8_t* in, uint8_t* out) {
    if (!m_initialized || !in || !out) {
        return false;
    }

    uint8_t state[16];
    memcpy(state, in, 16);

    /* Initial round */
    addRoundKey(state, 0);

    /* Main rounds */
    for (int round = 1; round < m_encKey.rounds; round++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, round);
    }

    /* Final round */
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, m_encKey.rounds);

    memcpy(out, state, 16);
    return true;
}

bool CryptoAES::decryptBlock(const uint8_t* in, uint8_t* out) {
    if (!m_initialized || !in || !out) {
        return false;
    }

    uint8_t state[16];
    memcpy(state, in, 16);

    /* Initial round */
    addRoundKey(state, m_encKey.rounds);

    /* Main rounds */
    for (int round = m_encKey.rounds - 1; round > 0; round--) {
        invShiftRows(state);
        invSubBytes(state);
        addRoundKey(state, round);
        invMixColumns(state);
    }

    /* Final round */
    invShiftRows(state);
    invSubBytes(state);
    addRoundKey(state, 0);

    memcpy(out, state, 16);
    return true;
}

int CryptoAES::encrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen) {
    if (!m_initialized || !in || !out) {
        return -1;
    }

    /* 计算需要的输出长度 (含填充) */
    int blocks = (inlen + 15) / 16;
    int needed = blocks * 16;
    if (outlen < needed) {
        return -1;
    }

    /* 处理完整块 */
    int i;
    for (i = 0; i + 16 <= inlen; i += 16) {
        if (!encryptBlock(in + i, out + i)) {
            return -1;
        }
    }

    /* 处理最后一个块 (PKCS7 填充) */
    if (i < inlen || inlen == 0) {
        uint8_t lastBlock[16];
        int remaining = inlen - i;
        int padding = 16 - remaining;

        memcpy(lastBlock, in + i, remaining);
        memset(lastBlock + remaining, padding, padding);

        if (!encryptBlock(lastBlock, out + i)) {
            return -1;
        }
    }

    return needed;
}

int CryptoAES::decrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen) {
    if (!m_initialized || !in || !out) {
        return -1;
    }

    /* 输入必须是块大小的倍数 */
    if (inlen % 16 != 0 || inlen == 0) {
        return -1;
    }

    if (outlen < inlen) {
        return -1;
    }

    /* 解密所有块 */
    for (int i = 0; i < inlen; i += 16) {
        if (!decryptBlock(in + i, out + i)) {
            return -1;
        }
    }

    /* 移除 PKCS7 填充 */
    uint8_t padding = out[inlen - 1];
    if (padding > 16 || padding == 0) {
        return inlen;  /* 无效填充，返回原始长度 */
    }

    /* 验证填充 */
    for (int i = 0; i < padding; i++) {
        if (out[inlen - 1 - i] != padding) {
            return inlen;  /* 填充不正确 */
        }
    }

    return inlen - padding;
}

} /* namespace ChannelServiceApp */
