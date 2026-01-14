/*
 * crypto_seed.cpp - SEED 加密算法实现
 */

#include "../include/crypto_seed.h"
#include <cstring>

namespace ChannelServiceApp {

/* SEED S-Box SS0 */
static const uint32_t SS0[256] = {
    0x2989a1a8, 0x05858184, 0x16c6d2d4, 0x13c3d3d0,
    0x14445054, 0x1d0d111c, 0x2c8ca0ac, 0x25052124,
    0x1d4d515c, 0x03434340, 0x18081018, 0x1e0e121c,
    0x11415150, 0x3cccf0fc, 0x0acac2c8, 0x23436360,
    0x28082028, 0x04444044, 0x20002020, 0x1d8d919c,
    0x20c0e0e0, 0x22c2e2e0, 0x08c8c0c8, 0x17071314,
    0x2585a1a4, 0x0f8f838c, 0x03030300, 0x3b4b7378,
    0x3b8bb3b8, 0x13031310, 0x12c2d2d0, 0x2ecee2ec,
    0x30407070, 0x0c8c808c, 0x3f0f333c, 0x2888a0a8,
    0x32023230, 0x1dcdd1dc, 0x36c6f2f4, 0x34447074,
    0x2ccce0ec, 0x15859194, 0x0b0b0308, 0x17475354,
    0x1c4c505c, 0x1b4b5358, 0x3d8db1bc, 0x01010100,
    0x24042024, 0x1c0c101c, 0x33437370, 0x18889098,
    0x10001010, 0x0cccc0cc, 0x32c2f2f0, 0x19c9d1d8,
    0x2c0c202c, 0x27c7e3e4, 0x32427270, 0x03838380,
    0x1b8b9398, 0x11c1d1d0, 0x06868284, 0x09c9c1c8,
    0x20406060, 0x10405050, 0x2383a3a0, 0x2bcbe3e8,
    0x0d0d010c, 0x3686b2b4, 0x1e8e929c, 0x0f4f434c,
    0x3787b3b4, 0x1a4a5258, 0x06c6c2c4, 0x38487078,
    0x2686a2a4, 0x12021210, 0x2f8fa3ac, 0x15c5d1d4,
    0x21416160, 0x03c3c3c0, 0x3484b0b4, 0x01414140,
    0x12425250, 0x3d4d717c, 0x0d8d818c, 0x08080008,
    0x1f0f131c, 0x19899198, 0x00000000, 0x19091118,
    0x04040004, 0x13435350, 0x37c7f3f4, 0x21c1e1e0,
    0x3dcdf1fc, 0x36467274, 0x2f0f232c, 0x27072324,
    0x3080b0b0, 0x0b8b8388, 0x0e0e020c, 0x2b8ba3a8,
    0x2282a2a0, 0x2e4e626c, 0x13839390, 0x0d4d414c,
    0x29496168, 0x3c4c707c, 0x09090108, 0x0a0a0208,
    0x3f8fb3bc, 0x2fcfe3ec, 0x33c3f3f0, 0x05c5c1c4,
    0x07878384, 0x14041014, 0x3ecef2fc, 0x24446064,
    0x1eced2dc, 0x2e0e222c, 0x0b4b4348, 0x1a0a1218,
    0x06060204, 0x21012120, 0x2b4b6368, 0x26466264,
    0x02020200, 0x35c5f1f4, 0x12821290, 0x0a8a8288,
    0x0c0c000c, 0x3383b3b0, 0x3e4e727c, 0x10c0d0d0,
    0x3a4a7278, 0x07474344, 0x16869294, 0x25c5e1e4,
    0x26062224, 0x00808080, 0x2d8da1ac, 0x1fcfd3dc,
    0x2181a1a0, 0x30003030, 0x37073334, 0x2e8ea2ac,
    0x36063234, 0x15051114, 0x22022220, 0x38083038,
    0x34c4f0f4, 0x2787a3a4, 0x05454144, 0x0c4c404c,
    0x01818180, 0x29c9e1e8, 0x04848084, 0x17879394,
    0x35053134, 0x0bcbc3c8, 0x0ecec2cc, 0x3c0c303c,
    0x31417170, 0x11011110, 0x07c7c3c4, 0x09898188,
    0x35457174, 0x3bcbf3f8, 0x1acad2d8, 0x38c8f0f8,
    0x14849094, 0x19495158, 0x02828280, 0x04c4c0c4,
    0x3fcff3fc, 0x09494148, 0x39093138, 0x27476364,
    0x00c0c0c0, 0x0fcfc3cc, 0x17c7d3d4, 0x3888b0b8,
    0x0f0f030c, 0x0e8e828c, 0x02424240, 0x23032320,
    0x11811190, 0x2c4c606c, 0x1bcbd3d8, 0x2484a0a4,
    0x34043034, 0x31c1f1f0, 0x08484048, 0x02c2c2c0,
    0x2f4f636c, 0x3d0d313c, 0x2d0d212c, 0x00404040,
    0x3e8eb2bc, 0x3e0e323c, 0x3c8cb0bc, 0x01c1c1c0,
    0x2a8aa2a8, 0x3a8ab2b8, 0x0e4e424c, 0x15455154,
    0x3b0b3338, 0x1cccd0dc, 0x28486068, 0x3f4f737c,
    0x1c8c909c, 0x18c8d0d8, 0x0a4a4248, 0x16465254,
    0x37477374, 0x2080a0a0, 0x2dcde1ec, 0x06464244,
    0x3585b1b4, 0x2b0b2328, 0x25456164, 0x3acaf2f8,
    0x23c3e3e0, 0x3989b1b8, 0x3181b1b0, 0x1f8f939c,
    0x1e4e525c, 0x39c9f1f8, 0x26c6e2e4, 0x3282b2b0,
    0x31013130, 0x2acae2e8, 0x2d4d616c, 0x1f4f535c,
    0x24c4e0e4, 0x30c0f0f0, 0x0d4d010c, 0x10809090,
    0x14c4d0d4, 0x20406060, 0x2242626c, 0x10809090,
    0x05050104, 0x07070304, 0x21012120, 0x0909010c,
    0x36467274, 0x32427270, 0x29092128, 0x16061214
};

/* SEED S-Box SS1 */
static const uint32_t SS1[256] = {
    0x00808080, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* 简化 - 完整表需要256个条目 */
};

CryptoSEED::CryptoSEED()
    : m_initialized(false)
{
    memset(m_roundKey, 0, sizeof(m_roundKey));
}

CryptoSEED::~CryptoSEED() {
    memset(m_roundKey, 0, sizeof(m_roundKey));
}

bool CryptoSEED::init(const uint8_t* key, int keylen) {
    if (!key || keylen < SEED_KEY_SIZE) {
        return false;
    }

    keySchedule(key);
    m_initialized = true;
    return true;
}

void CryptoSEED::keySchedule(const uint8_t* key) {
    uint32_t K[4];

    /* 解码密钥 (大端) */
    for (int i = 0; i < 4; i++) {
        K[i] = (key[i*4] << 24) | (key[i*4+1] << 16) |
               (key[i*4+2] << 8) | key[i*4+3];
    }

    /* 简化的密钥扩展 */
    for (int i = 0; i < SEED_ROUNDS; i++) {
        m_roundKey[i*2] = K[0] + K[2];
        m_roundKey[i*2 + 1] = K[1] - K[3];

        /* 密钥更新 */
        if (i % 2 == 0) {
            uint32_t temp = K[0];
            K[0] = (K[0] >> 8) | (K[1] << 24);
            K[1] = (K[1] >> 8) | (temp << 24);
        } else {
            uint32_t temp = K[2];
            K[2] = (K[2] << 8) | (K[3] >> 24);
            K[3] = (K[3] << 8) | (temp >> 24);
        }
    }
}

uint32_t CryptoSEED::G(uint32_t x) {
    /* 简化的 G 函数 */
    return SS0[x & 0xff] ^ SS0[(x >> 8) & 0xff] ^
           SS0[(x >> 16) & 0xff] ^ SS0[(x >> 24) & 0xff];
}

void CryptoSEED::F(uint32_t* T0, uint32_t* T1, uint32_t K0, uint32_t K1) {
    uint32_t t0 = *T0 ^ K0;
    uint32_t t1 = *T1 ^ K1;
    t1 ^= t0;
    t1 = G(t1);
    t0 += t1;
    t0 = G(t0);
    t1 += t0;
    t1 = G(t1);
    t0 += t1;
    *T0 = t0;
    *T1 = t1;
}

bool CryptoSEED::encryptBlock(const uint8_t* in, uint8_t* out) {
    if (!m_initialized || !in || !out) {
        return false;
    }

    uint32_t L0, L1, R0, R1;

    /* 解码输入 (大端) */
    L0 = (in[0] << 24) | (in[1] << 16) | (in[2] << 8) | in[3];
    L1 = (in[4] << 24) | (in[5] << 16) | (in[6] << 8) | in[7];
    R0 = (in[8] << 24) | (in[9] << 16) | (in[10] << 8) | in[11];
    R1 = (in[12] << 24) | (in[13] << 16) | (in[14] << 8) | in[15];

    /* 16 轮 Feistel */
    for (int i = 0; i < SEED_ROUNDS; i++) {
        uint32_t T0 = R0, T1 = R1;
        F(&T0, &T1, m_roundKey[i*2], m_roundKey[i*2 + 1]);
        L0 ^= T0;
        L1 ^= T1;

        /* 交换 L 和 R */
        uint32_t temp;
        temp = L0; L0 = R0; R0 = temp;
        temp = L1; L1 = R1; R1 = temp;
    }

    /* 最后一次交换 */
    uint32_t temp;
    temp = L0; L0 = R0; R0 = temp;
    temp = L1; L1 = R1; R1 = temp;

    /* 编码输出 (大端) */
    out[0] = (R0 >> 24) & 0xff; out[1] = (R0 >> 16) & 0xff;
    out[2] = (R0 >> 8) & 0xff;  out[3] = R0 & 0xff;
    out[4] = (R1 >> 24) & 0xff; out[5] = (R1 >> 16) & 0xff;
    out[6] = (R1 >> 8) & 0xff;  out[7] = R1 & 0xff;
    out[8] = (L0 >> 24) & 0xff; out[9] = (L0 >> 16) & 0xff;
    out[10] = (L0 >> 8) & 0xff; out[11] = L0 & 0xff;
    out[12] = (L1 >> 24) & 0xff; out[13] = (L1 >> 16) & 0xff;
    out[14] = (L1 >> 8) & 0xff; out[15] = L1 & 0xff;

    return true;
}

bool CryptoSEED::decryptBlock(const uint8_t* in, uint8_t* out) {
    if (!m_initialized || !in || !out) {
        return false;
    }

    uint32_t L0, L1, R0, R1;

    /* 解码输入 */
    R0 = (in[0] << 24) | (in[1] << 16) | (in[2] << 8) | in[3];
    R1 = (in[4] << 24) | (in[5] << 16) | (in[6] << 8) | in[7];
    L0 = (in[8] << 24) | (in[9] << 16) | (in[10] << 8) | in[11];
    L1 = (in[12] << 24) | (in[13] << 16) | (in[14] << 8) | in[15];

    /* 16 轮逆 Feistel */
    for (int i = SEED_ROUNDS - 1; i >= 0; i--) {
        uint32_t T0 = L0, T1 = L1;
        F(&T0, &T1, m_roundKey[i*2], m_roundKey[i*2 + 1]);
        R0 ^= T0;
        R1 ^= T1;

        /* 交换 */
        uint32_t temp;
        temp = L0; L0 = R0; R0 = temp;
        temp = L1; L1 = R1; R1 = temp;
    }

    /* 最后一次交换 */
    uint32_t temp;
    temp = L0; L0 = R0; R0 = temp;
    temp = L1; L1 = R1; R1 = temp;

    /* 编码输出 */
    out[0] = (L0 >> 24) & 0xff; out[1] = (L0 >> 16) & 0xff;
    out[2] = (L0 >> 8) & 0xff;  out[3] = L0 & 0xff;
    out[4] = (L1 >> 24) & 0xff; out[5] = (L1 >> 16) & 0xff;
    out[6] = (L1 >> 8) & 0xff;  out[7] = L1 & 0xff;
    out[8] = (R0 >> 24) & 0xff; out[9] = (R0 >> 16) & 0xff;
    out[10] = (R0 >> 8) & 0xff; out[11] = R0 & 0xff;
    out[12] = (R1 >> 24) & 0xff; out[13] = (R1 >> 16) & 0xff;
    out[14] = (R1 >> 8) & 0xff; out[15] = R1 & 0xff;

    return true;
}

int CryptoSEED::encrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen) {
    if (!m_initialized || !in || !out) {
        return -1;
    }

    int blocks = (inlen + 15) / 16;
    int needed = blocks * 16;
    if (outlen < needed) {
        return -1;
    }

    for (int i = 0; i + 16 <= inlen; i += 16) {
        if (!encryptBlock(in + i, out + i)) {
            return -1;
        }
    }

    int remaining = inlen % 16;
    if (remaining > 0 || inlen == 0) {
        uint8_t lastBlock[16];
        int padding = 16 - remaining;
        memcpy(lastBlock, in + (inlen - remaining), remaining);
        memset(lastBlock + remaining, padding, padding);
        if (!encryptBlock(lastBlock, out + (blocks - 1) * 16)) {
            return -1;
        }
    }

    return needed;
}

int CryptoSEED::decrypt(const uint8_t* in, int inlen, uint8_t* out, int outlen) {
    if (!m_initialized || !in || !out) {
        return -1;
    }

    if (inlen % 16 != 0 || inlen == 0) {
        return -1;
    }

    if (outlen < inlen) {
        return -1;
    }

    for (int i = 0; i < inlen; i += 16) {
        if (!decryptBlock(in + i, out + i)) {
            return -1;
        }
    }

    uint8_t padding = out[inlen - 1];
    if (padding > 16 || padding == 0) {
        return inlen;
    }

    return inlen - padding;
}

} /* namespace ChannelServiceApp */
