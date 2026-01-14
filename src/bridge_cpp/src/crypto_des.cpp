/*
 * crypto_des.cpp - DES/3DES 加密算法实现
 */

#include "../include/crypto_des.h"
#include <cstring>

namespace ChannelServiceApp {

/* DES 初始置换表 */
static const uint8_t IP[64] = {
    58, 50, 42, 34, 26, 18, 10,  2,
    60, 52, 44, 36, 28, 20, 12,  4,
    62, 54, 46, 38, 30, 22, 14,  6,
    64, 56, 48, 40, 32, 24, 16,  8,
    57, 49, 41, 33, 25, 17,  9,  1,
    59, 51, 43, 35, 27, 19, 11,  3,
    61, 53, 45, 37, 29, 21, 13,  5,
    63, 55, 47, 39, 31, 23, 15,  7
};

/* DES 逆初始置换表 */
static const uint8_t FP[64] = {
    40,  8, 48, 16, 56, 24, 64, 32,
    39,  7, 47, 15, 55, 23, 63, 31,
    38,  6, 46, 14, 54, 22, 62, 30,
    37,  5, 45, 13, 53, 21, 61, 29,
    36,  4, 44, 12, 52, 20, 60, 28,
    35,  3, 43, 11, 51, 19, 59, 27,
    34,  2, 42, 10, 50, 18, 58, 26,
    33,  1, 41,  9, 49, 17, 57, 25
};

/* 扩展置换表 E */
static const uint8_t E[48] = {
    32,  1,  2,  3,  4,  5,
     4,  5,  6,  7,  8,  9,
     8,  9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32,  1
};

/* P 置换表 */
static const uint8_t P[32] = {
    16,  7, 20, 21, 29, 12, 28, 17,
     1, 15, 23, 26,  5, 18, 31, 10,
     2,  8, 24, 14, 32, 27,  3,  9,
    19, 13, 30,  6, 22, 11,  4, 25
};

/* S-Box */
static const uint8_t SBOX[8][64] = {
    /* S1 */
    {
        14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
         0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
         4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
        15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
    },
    /* S2 */
    {
        15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
         3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
         0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
        13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
    },
    /* S3 */
    {
        10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
        13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
        13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
         1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
    },
    /* S4 */
    {
         7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
        13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
        10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
         3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
    },
    /* S5 */
    {
         2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
        14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
         4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
        11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
    },
    /* S6 */
    {
        12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
        10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
         9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
         4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13
    },
    /* S7 */
    {
         4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
        13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
         1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
         6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12
    },
    /* S8 */
    {
        13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
         1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
         7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
         2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
    }
};

/* PC1 置换选择表 */
static const uint8_t PC1[56] = {
    57, 49, 41, 33, 25, 17,  9,
     1, 58, 50, 42, 34, 26, 18,
    10,  2, 59, 51, 43, 35, 27,
    19, 11,  3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
     7, 62, 54, 46, 38, 30, 22,
    14,  6, 61, 53, 45, 37, 29,
    21, 13,  5, 28, 20, 12,  4
};

/* PC2 置换选择表 */
static const uint8_t PC2[48] = {
    14, 17, 11, 24,  1,  5,
     3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8,
    16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};

/* 左移位数 */
static const uint8_t SHIFTS[16] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

/*
 * CryptoDES 实现
 */
CryptoDES::CryptoDES()
    : m_initialized(false)
{
    memset(&m_key, 0, sizeof(m_key));
}

CryptoDES::~CryptoDES() {
    memset(&m_key, 0, sizeof(m_key));
}

bool CryptoDES::init(const uint8_t* key, int keylen) {
    if (!key || keylen < DES_KEY_SIZE) {
        return false;
    }

    setKey(key);
    m_initialized = true;
    return true;
}

void CryptoDES::setKey(const uint8_t* key) {
    uint32_t c = 0, d = 0;

    /* PC1 置换 */
    for (int i = 0; i < 28; i++) {
        int bit = PC1[i] - 1;
        if (key[bit / 8] & (0x80 >> (bit % 8))) {
            c |= (1 << (27 - i));
        }
    }
    for (int i = 0; i < 28; i++) {
        int bit = PC1[28 + i] - 1;
        if (key[bit / 8] & (0x80 >> (bit % 8))) {
            d |= (1 << (27 - i));
        }
    }

    /* 生成 16 轮子密钥 */
    for (int round = 0; round < 16; round++) {
        /* 左移 */
        for (int i = 0; i < SHIFTS[round]; i++) {
            c = ((c << 1) | (c >> 27)) & 0x0FFFFFFF;
            d = ((d << 1) | (d >> 27)) & 0x0FFFFFFF;
        }

        /* PC2 置换生成子密钥 */
        uint32_t k1 = 0, k2 = 0;
        uint64_t cd = ((uint64_t)c << 28) | d;

        for (int i = 0; i < 24; i++) {
            int bit = PC2[i] - 1;
            if (cd & (1ULL << (55 - bit))) {
                k1 |= (1 << (23 - i));
            }
        }
        for (int i = 0; i < 24; i++) {
            int bit = PC2[24 + i] - 1;
            if (cd & (1ULL << (55 - bit))) {
                k2 |= (1 << (23 - i));
            }
        }

        m_key.sk[round * 2] = k1;
        m_key.sk[round * 2 + 1] = k2;
    }
}

void CryptoDES::initialPermutation(const uint8_t* in, uint32_t* out) {
    uint32_t l = 0, r = 0;

    for (int i = 0; i < 32; i++) {
        int bit = IP[i] - 1;
        if (in[bit / 8] & (0x80 >> (bit % 8))) {
            l |= (1 << (31 - i));
        }
    }
    for (int i = 0; i < 32; i++) {
        int bit = IP[32 + i] - 1;
        if (in[bit / 8] & (0x80 >> (bit % 8))) {
            r |= (1 << (31 - i));
        }
    }

    out[0] = l;
    out[1] = r;
}

void CryptoDES::finalPermutation(const uint32_t* in, uint8_t* out) {
    uint64_t rl = ((uint64_t)in[1] << 32) | in[0];

    memset(out, 0, 8);
    for (int i = 0; i < 64; i++) {
        int bit = FP[i] - 1;
        if (rl & (1ULL << (63 - bit))) {
            out[i / 8] |= (0x80 >> (i % 8));
        }
    }
}

uint32_t CryptoDES::feistel(uint32_t r, int round) {
    uint64_t expanded = 0;

    /* 扩展置换 E */
    for (int i = 0; i < 48; i++) {
        int bit = E[i] - 1;
        if (r & (1 << (31 - bit))) {
            expanded |= (1ULL << (47 - i));
        }
    }

    /* XOR with subkey */
    uint64_t subkey = ((uint64_t)m_key.sk[round * 2] << 24) | m_key.sk[round * 2 + 1];
    expanded ^= subkey;

    /* S-Box 替换 */
    uint32_t sbox_out = 0;
    for (int i = 0; i < 8; i++) {
        int bits = (expanded >> (42 - i * 6)) & 0x3F;
        int row = ((bits & 0x20) >> 4) | (bits & 0x01);
        int col = (bits >> 1) & 0x0F;
        sbox_out |= (SBOX[i][row * 16 + col] << (28 - i * 4));
    }

    /* P 置换 */
    uint32_t result = 0;
    for (int i = 0; i < 32; i++) {
        int bit = P[i] - 1;
        if (sbox_out & (1 << (31 - bit))) {
            result |= (1 << (31 - i));
        }
    }

    return result;
}

void CryptoDES::desCore(uint32_t* block, int encrypt) {
    uint32_t l = block[0];
    uint32_t r = block[1];

    for (int i = 0; i < 16; i++) {
        int round = encrypt ? i : (15 - i);
        uint32_t f = feistel(r, round);
        uint32_t temp = r;
        r = l ^ f;
        l = temp;
    }

    block[0] = r;
    block[1] = l;
}

bool CryptoDES::encryptBlock(const uint8_t* in, uint8_t* out) {
    if (!m_initialized || !in || !out) {
        return false;
    }

    uint32_t block[2];
    initialPermutation(in, block);
    desCore(block, 1);
    finalPermutation(block, out);

    return true;
}

bool CryptoDES::decryptBlock(const uint8_t* in, uint8_t* out) {
    if (!m_initialized || !in || !out) {
        return false;
    }

    uint32_t block[2];
    initialPermutation(in, block);
    desCore(block, 0);
    finalPermutation(block, out);

    return true;
}

int CryptoDES::encrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen) {
    if (!m_initialized || !in || !out) {
        return -1;
    }

    int blocks = (inlen + 7) / 8;
    int needed = blocks * 8;
    if (outlen < needed) {
        return -1;
    }

    for (int i = 0; i + 8 <= inlen; i += 8) {
        if (!encryptBlock(in + i, out + i)) {
            return -1;
        }
    }

    /* 处理最后一个块 */
    int remaining = inlen % 8;
    if (remaining > 0 || inlen == 0) {
        uint8_t lastBlock[8];
        int padding = 8 - remaining;
        memcpy(lastBlock, in + (inlen - remaining), remaining);
        memset(lastBlock + remaining, padding, padding);
        if (!encryptBlock(lastBlock, out + (blocks - 1) * 8)) {
            return -1;
        }
    }

    return needed;
}

int CryptoDES::decrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen) {
    if (!m_initialized || !in || !out) {
        return -1;
    }

    if (inlen % 8 != 0 || inlen == 0) {
        return -1;
    }

    if (outlen < inlen) {
        return -1;
    }

    for (int i = 0; i < inlen; i += 8) {
        if (!decryptBlock(in + i, out + i)) {
            return -1;
        }
    }

    /* 移除填充 */
    uint8_t padding = out[inlen - 1];
    if (padding > 8 || padding == 0) {
        return inlen;
    }

    return inlen - padding;
}

/*
 * CryptoDES3 实现
 */
CryptoDES3::CryptoDES3()
    : m_initialized(false)
{
}

CryptoDES3::~CryptoDES3() {
}

bool CryptoDES3::init(const uint8_t* key, int keylen) {
    if (!key || keylen < DES3_KEY_SIZE) {
        return false;
    }

    /* 三个独立的 DES 密钥 */
    if (!m_des1.init(key, 8)) return false;
    if (!m_des2.init(key + 8, 8)) return false;
    if (!m_des3.init(key + 16, 8)) return false;

    m_initialized = true;
    return true;
}

bool CryptoDES3::encryptBlock(const uint8_t* in, uint8_t* out) {
    if (!m_initialized || !in || !out) {
        return false;
    }

    uint8_t temp1[8], temp2[8];

    /* 加密-解密-加密 (EDE) */
    m_des1.encryptBlock(in, temp1);
    m_des2.decryptBlock(temp1, temp2);
    m_des3.encryptBlock(temp2, out);

    return true;
}

bool CryptoDES3::decryptBlock(const uint8_t* in, uint8_t* out) {
    if (!m_initialized || !in || !out) {
        return false;
    }

    uint8_t temp1[8], temp2[8];

    /* 解密-加密-解密 (DED) */
    m_des3.decryptBlock(in, temp1);
    m_des2.encryptBlock(temp1, temp2);
    m_des1.decryptBlock(temp2, out);

    return true;
}

int CryptoDES3::encrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen) {
    if (!m_initialized || !in || !out) {
        return -1;
    }

    int blocks = (inlen + 7) / 8;
    int needed = blocks * 8;
    if (outlen < needed) {
        return -1;
    }

    for (int i = 0; i + 8 <= inlen; i += 8) {
        if (!encryptBlock(in + i, out + i)) {
            return -1;
        }
    }

    int remaining = inlen % 8;
    if (remaining > 0 || inlen == 0) {
        uint8_t lastBlock[8];
        int padding = 8 - remaining;
        memcpy(lastBlock, in + (inlen - remaining), remaining);
        memset(lastBlock + remaining, padding, padding);
        if (!encryptBlock(lastBlock, out + (blocks - 1) * 8)) {
            return -1;
        }
    }

    return needed;
}

int CryptoDES3::decrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen) {
    if (!m_initialized || !in || !out) {
        return -1;
    }

    if (inlen % 8 != 0 || inlen == 0) {
        return -1;
    }

    if (outlen < inlen) {
        return -1;
    }

    for (int i = 0; i < inlen; i += 8) {
        if (!decryptBlock(in + i, out + i)) {
            return -1;
        }
    }

    uint8_t padding = out[inlen - 1];
    if (padding > 8 || padding == 0) {
        return inlen;
    }

    return inlen - padding;
}

} /* namespace ChannelServiceApp */
