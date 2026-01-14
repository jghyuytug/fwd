/**
 * twoFactor.util.ts 单元测试
 */

import {
  generateTwoFactorSecret,
  generateQRCode,
  verifyTwoFactorCode,
  generateBackupCodes,
  validateBackupCodeFormat
} from '../twoFactor.util';
import speakeasy from 'speakeasy';

describe('twoFactor.util', () => {
  describe('generateTwoFactorSecret', () => {
    it('应该生成有效的2FA密钥', () => {
      const username = 'test_gm';
      const result = generateTwoFactorSecret(username);

      expect(result).toBeDefined();
      expect(result.secret).toBeDefined();
      expect(result.otpauthUrl).toBeDefined();
      expect(typeof result.secret).toBe('string');
      expect(typeof result.otpauthUrl).toBe('string');
    });

    it('密钥应该是base32格式', () => {
      const username = 'test_gm';
      const result = generateTwoFactorSecret(username);

      // Base32字符集：A-Z, 2-7
      expect(result.secret).toMatch(/^[A-Z2-7]+$/);
      expect(result.secret.length).toBeGreaterThan(0);
    });

    it('otpauthUrl应该包含正确的信息', () => {
      const username = 'test_gm';
      const result = generateTwoFactorSecret(username);

      expect(result.otpauthUrl).toContain('otpauth://totp/');
      expect(result.otpauthUrl).toContain(username);
      expect(result.otpauthUrl).toContain('DNF%20GM%20Tools'); // URL编码的issuer
      expect(result.otpauthUrl).toContain(`secret=${result.secret}`);
    });

    it('每次生成的密钥应该不同', () => {
      const username = 'test_gm';
      const result1 = generateTwoFactorSecret(username);
      const result2 = generateTwoFactorSecret(username);

      expect(result1.secret).not.toBe(result2.secret);
    });
  });

  describe('generateQRCode', () => {
    it('应该生成有效的QR码', async () => {
      const otpauthUrl = 'otpauth://totp/DNF%20GM%20Tools:test_gm?secret=JBSWY3DPEHPK3PXP&issuer=DNF%20GM%20Tools';
      const qrCode = await generateQRCode(otpauthUrl);

      expect(qrCode).toBeDefined();
      expect(typeof qrCode).toBe('string');
      expect(qrCode).toMatch(/^data:image\/png;base64,/);
    });

    it('应该生成base64编码的图片', async () => {
      const otpauthUrl = 'otpauth://totp/DNF%20GM%20Tools:test_gm?secret=JBSWY3DPEHPK3PXP&issuer=DNF%20GM%20Tools';
      const qrCode = await generateQRCode(otpauthUrl);

      const base64Data = qrCode.replace('data:image/png;base64,', '');
      expect(base64Data.length).toBeGreaterThan(0);
      expect(base64Data).toMatch(/^[A-Za-z0-9+/=]+$/);
    });

    it('应该处理不同的URL', async () => {
      const url1 = 'otpauth://totp/Test1:user1?secret=SECRET1';
      const url2 = 'otpauth://totp/Test2:user2?secret=SECRET2';

      const qr1 = await generateQRCode(url1);
      const qr2 = await generateQRCode(url2);

      expect(qr1).not.toBe(qr2);
    });
  });

  describe('verifyTwoFactorCode', () => {
    it('应该验证有效的TOTP代码', () => {
      const secret = speakeasy.generateSecret({ length: 20 });
      const token = speakeasy.totp({
        secret: secret.base32,
        encoding: 'base32'
      });

      const isValid = verifyTwoFactorCode(secret.base32, token);
      expect(isValid).toBe(true);
    });

    it('应该拒绝无效的代码', () => {
      const secret = speakeasy.generateSecret({ length: 20 });
      const invalidToken = '000000';

      const isValid = verifyTwoFactorCode(secret.base32, invalidToken);
      expect(isValid).toBe(false);
    });

    it('应该拒绝空代码', () => {
      const secret = speakeasy.generateSecret({ length: 20 });
      const isValid = verifyTwoFactorCode(secret.base32, '');
      expect(isValid).toBe(false);
    });

    it('应该有时间窗口容错（±1步）', () => {
      const secret = speakeasy.generateSecret({ length: 20 });

      // 生成前一个时间步的token
      const previousToken = speakeasy.totp({
        secret: secret.base32,
        encoding: 'base32',
        step: 30,
        time: Math.floor(Date.now() / 1000) - 30
      });

      // 应该仍然有效（因为有窗口容错）
      const isValid = verifyTwoFactorCode(secret.base32, previousToken);
      expect(isValid).toBe(true);
    });

    it('应该拒绝格式错误的代码', () => {
      const secret = speakeasy.generateSecret({ length: 20 });
      const invalidCodes = ['abc', '12345', '1234567', 'abcdef'];

      invalidCodes.forEach(code => {
        const isValid = verifyTwoFactorCode(secret.base32, code);
        expect(isValid).toBe(false);
      });
    });
  });

  describe('generateBackupCodes', () => {
    it('应该生成指定数量的备份代码', () => {
      const codes = generateBackupCodes(10);
      expect(codes).toHaveLength(10);
    });

    it('每个代码应该是8位字符', () => {
      const codes = generateBackupCodes(5);
      codes.forEach(code => {
        expect(code).toHaveLength(8);
      });
    });

    it('代码应该只包含大写字母和数字', () => {
      const codes = generateBackupCodes(5);
      codes.forEach(code => {
        expect(code).toMatch(/^[A-Z0-9]+$/);
      });
    });

    it('所有代码应该唯一', () => {
      const codes = generateBackupCodes(10);
      const uniqueCodes = new Set(codes);
      expect(uniqueCodes.size).toBe(codes.length);
    });

    it('应该排除易混淆字符（0, O, I, 1）', () => {
      const codes = generateBackupCodes(20);
      const confusingChars = /[0OI1]/;

      codes.forEach(code => {
        expect(code).not.toMatch(confusingChars);
      });
    });

    it('应该处理不同的数量', () => {
      expect(generateBackupCodes(1)).toHaveLength(1);
      expect(generateBackupCodes(5)).toHaveLength(5);
      expect(generateBackupCodes(20)).toHaveLength(20);
    });

    it('默认应该生成10个代码', () => {
      const codes = generateBackupCodes();
      expect(codes).toHaveLength(10);
    });
  });

  describe('validateBackupCodeFormat', () => {
    it('应该验证有效的备份代码格式', () => {
      const validCodes = [
        'ABCDEFGH',
        'A2B3C4D5',
        '23456789',
        'ZYXWVUTS'
      ];

      validCodes.forEach(code => {
        expect(validateBackupCodeFormat(code)).toBe(true);
      });
    });

    it('应该拒绝长度错误的代码', () => {
      const invalidCodes = [
        'ABC',
        'ABCDE',
        'ABCDEFG',
        'ABCDEFGHI',
        'ABCDEFGHIJ'
      ];

      invalidCodes.forEach(code => {
        expect(validateBackupCodeFormat(code)).toBe(false);
      });
    });

    it('应该拒绝包含小写字母的代码', () => {
      expect(validateBackupCodeFormat('abcdefgh')).toBe(false);
      expect(validateBackupCodeFormat('ABCDefgh')).toBe(false);
    });

    it('应该拒绝包含特殊字符的代码', () => {
      const invalidCodes = [
        'ABCD-EFG',
        'ABCD_EFG',
        'ABCD EFG',
        'ABCD@EFG'
      ];

      invalidCodes.forEach(code => {
        expect(validateBackupCodeFormat(code)).toBe(false);
      });
    });

    it('应该拒绝空字符串和null', () => {
      expect(validateBackupCodeFormat('')).toBe(false);
      expect(validateBackupCodeFormat(null as any)).toBe(false);
      expect(validateBackupCodeFormat(undefined as any)).toBe(false);
    });

    it('应该拒绝包含易混淆字符的代码', () => {
      expect(validateBackupCodeFormat('0BCDEFGH')).toBe(false); // 0
      expect(validateBackupCodeFormat('OBCDEFGH')).toBe(false); // O
      expect(validateBackupCodeFormat('IBCDEFGH')).toBe(false); // I
      expect(validateBackupCodeFormat('1BCDEFGH')).toBe(false); // 1
    });
  });

  describe('集成测试', () => {
    it('完整的2FA设置流程应该工作', async () => {
      const username = 'test_gm';

      // 1. 生成密钥
      const { secret, otpauthUrl } = generateTwoFactorSecret(username);
      expect(secret).toBeDefined();

      // 2. 生成QR码
      const qrCode = await generateQRCode(otpauthUrl);
      expect(qrCode).toBeDefined();
      expect(qrCode).toMatch(/^data:image\/png;base64,/);

      // 3. 生成当前有效的TOTP代码
      const token = speakeasy.totp({
        secret: secret,
        encoding: 'base32'
      });

      // 4. 验证代码
      const isValid = verifyTwoFactorCode(secret, token);
      expect(isValid).toBe(true);

      // 5. 生成备份代码
      const backupCodes = generateBackupCodes(10);
      expect(backupCodes).toHaveLength(10);

      // 6. 验证备份代码格式
      backupCodes.forEach(code => {
        expect(validateBackupCodeFormat(code)).toBe(true);
      });
    });
  });
});
