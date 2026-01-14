/**
 * crypto.util.ts 单元测试
 */

import {
  hashPassword,
  verifyPassword,
  generateRandomToken,
  generateDeviceToken,
  sha256,
  validateDeviceFingerprint
} from '../crypto.util';

describe('crypto.util', () => {
  describe('hashPassword', () => {
    it('应该成功哈希密码', async () => {
      const password = 'TestPassword123!';
      const hash = await hashPassword(password);

      expect(hash).toBeDefined();
      expect(hash).not.toBe(password);
      expect(hash.length).toBeGreaterThan(0);
      expect(hash).toMatch(/^\$2[aby]\$\d{2}\$/); // bcrypt格式
    });

    it('相同密码应该生成不同的哈希值（盐值不同）', async () => {
      const password = 'TestPassword123!';
      const hash1 = await hashPassword(password);
      const hash2 = await hashPassword(password);

      expect(hash1).not.toBe(hash2);
    });

    it('空密码应该抛出错误', async () => {
      await expect(hashPassword('')).rejects.toThrow();
    });
  });

  describe('verifyPassword', () => {
    it('应该验证正确的密码', async () => {
      const password = 'TestPassword123!';
      const hash = await hashPassword(password);
      const isValid = await verifyPassword(password, hash);

      expect(isValid).toBe(true);
    });

    it('应该拒绝错误的密码', async () => {
      const password = 'TestPassword123!';
      const wrongPassword = 'WrongPassword123!';
      const hash = await hashPassword(password);
      const isValid = await verifyPassword(wrongPassword, hash);

      expect(isValid).toBe(false);
    });

    it('应该处理空密码', async () => {
      const hash = await hashPassword('test');
      const isValid = await verifyPassword('', hash);

      expect(isValid).toBe(false);
    });
  });

  describe('generateRandomToken', () => {
    it('应该生成指定长度的随机令牌', () => {
      const token1 = generateRandomToken(32);
      const token2 = generateRandomToken(64);

      expect(token1).toHaveLength(64); // 32字节 = 64十六进制字符
      expect(token2).toHaveLength(128); // 64字节 = 128十六进制字符
    });

    it('应该生成唯一的令牌', () => {
      const token1 = generateRandomToken();
      const token2 = generateRandomToken();

      expect(token1).not.toBe(token2);
    });

    it('应该只包含十六进制字符', () => {
      const token = generateRandomToken(16);
      expect(token).toMatch(/^[0-9a-f]+$/);
    });
  });

  describe('generateDeviceToken', () => {
    it('应该生成64字符的设备令牌', () => {
      const token = generateDeviceToken();
      expect(token).toHaveLength(64);
      expect(token).toMatch(/^[0-9a-f]+$/);
    });

    it('应该生成唯一的设备令牌', () => {
      const token1 = generateDeviceToken();
      const token2 = generateDeviceToken();
      expect(token1).not.toBe(token2);
    });
  });

  describe('sha256', () => {
    it('应该生成正确的SHA256哈希', () => {
      const data = 'test data';
      const hash = sha256(data);

      expect(hash).toHaveLength(64); // SHA256 = 256位 = 64十六进制字符
      expect(hash).toMatch(/^[0-9a-f]+$/);
    });

    it('相同数据应该生成相同的哈希', () => {
      const data = 'test data';
      const hash1 = sha256(data);
      const hash2 = sha256(data);

      expect(hash1).toBe(hash2);
    });

    it('不同数据应该生成不同的哈希', () => {
      const hash1 = sha256('test1');
      const hash2 = sha256('test2');

      expect(hash1).not.toBe(hash2);
    });

    it('应该处理空字符串', () => {
      const hash = sha256('');
      expect(hash).toHaveLength(64);
      expect(hash).toBe('e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855');
    });

    it('应该处理中文字符', () => {
      const hash = sha256('测试数据');
      expect(hash).toHaveLength(64);
      expect(hash).toMatch(/^[0-9a-f]+$/);
    });
  });

  describe('validateDeviceFingerprint', () => {
    it('应该验证有效的设备指纹', () => {
      const validFingerprints = [
        '1234567890abcdef',
        'ABCDEF1234567890',
        'a1b2c3d4e5f6',
        '0000000000000000'
      ];

      validFingerprints.forEach(fp => {
        expect(validateDeviceFingerprint(fp)).toBe(true);
      });
    });

    it('应该拒绝无效的设备指纹', () => {
      const invalidFingerprints = [
        '',
        'abc',
        '12345',
        'invalid-fingerprint',
        'abcdefg!@#$%',
        null,
        undefined
      ];

      invalidFingerprints.forEach(fp => {
        expect(validateDeviceFingerprint(fp as any)).toBe(false);
      });
    });

    it('应该拒绝太短的指纹', () => {
      expect(validateDeviceFingerprint('123')).toBe(false);
    });

    it('应该拒绝包含非十六进制字符的指纹', () => {
      expect(validateDeviceFingerprint('abcdefg12345')).toBe(false);
      expect(validateDeviceFingerprint('test-device-fingerprint')).toBe(false);
    });
  });
});
