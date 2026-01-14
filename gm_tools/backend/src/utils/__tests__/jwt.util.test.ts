/**
 * jwt.util.ts 单元测试
 */

import { generateAccessToken, generateRefreshToken, verifyToken } from '../jwt.util';
import jwt from 'jsonwebtoken';

describe('jwt.util', () => {
  const mockPayload = {
    gm_id: 1,
    username: 'test_gm',
    gm_level: 5,
    permissions: ['*']
  };

  describe('generateAccessToken', () => {
    it('应该生成有效的Access Token', () => {
      const token = generateAccessToken(mockPayload);

      expect(token).toBeDefined();
      expect(typeof token).toBe('string');
      expect(token.split('.')).toHaveLength(3); // JWT格式：header.payload.signature
    });

    it('生成的Token应该包含正确的payload', () => {
      const token = generateAccessToken(mockPayload);
      const decoded = jwt.decode(token) as any;

      expect(decoded.gm_id).toBe(mockPayload.gm_id);
      expect(decoded.username).toBe(mockPayload.username);
      expect(decoded.gm_level).toBe(mockPayload.gm_level);
      expect(decoded.permissions).toEqual(mockPayload.permissions);
      expect(decoded.type).toBe('access');
    });

    it('应该设置正确的过期时间', () => {
      const token = generateAccessToken(mockPayload);
      const decoded = jwt.decode(token) as any;

      expect(decoded.exp).toBeDefined();
      expect(decoded.iat).toBeDefined();
      expect(decoded.exp - decoded.iat).toBe(parseInt(process.env.JWT_EXPIRATION || '1800'));
    });
  });

  describe('generateRefreshToken', () => {
    it('应该生成有效的Refresh Token', () => {
      const token = generateRefreshToken({
        gm_id: mockPayload.gm_id,
        username: mockPayload.username,
        gm_level: mockPayload.gm_level
      });

      expect(token).toBeDefined();
      expect(typeof token).toBe('string');
      expect(token.split('.')).toHaveLength(3);
    });

    it('生成的Token应该包含正确的payload', () => {
      const token = generateRefreshToken({
        gm_id: mockPayload.gm_id,
        username: mockPayload.username,
        gm_level: mockPayload.gm_level
      });
      const decoded = jwt.decode(token) as any;

      expect(decoded.gm_id).toBe(mockPayload.gm_id);
      expect(decoded.username).toBe(mockPayload.username);
      expect(decoded.gm_level).toBe(mockPayload.gm_level);
      expect(decoded.type).toBe('refresh');
      expect(decoded.permissions).toBeUndefined(); // Refresh Token不包含权限
    });

    it('应该设置正确的过期时间', () => {
      const token = generateRefreshToken({
        gm_id: mockPayload.gm_id,
        username: mockPayload.username,
        gm_level: mockPayload.gm_level
      });
      const decoded = jwt.decode(token) as any;

      expect(decoded.exp).toBeDefined();
      expect(decoded.iat).toBeDefined();
      expect(decoded.exp - decoded.iat).toBe(parseInt(process.env.JWT_REFRESH_EXPIRATION || '604800'));
    });
  });

  describe('verifyToken', () => {
    it('应该验证有效的Access Token', () => {
      const token = generateAccessToken(mockPayload);
      const payload = verifyToken(token);

      expect(payload).toBeDefined();
      expect(payload.gm_id).toBe(mockPayload.gm_id);
      expect(payload.username).toBe(mockPayload.username);
      expect(payload.type).toBe('access');
    });

    it('应该验证有效的Refresh Token', () => {
      const token = generateRefreshToken({
        gm_id: mockPayload.gm_id,
        username: mockPayload.username,
        gm_level: mockPayload.gm_level
      });
      const payload = verifyToken(token);

      expect(payload).toBeDefined();
      expect(payload.gm_id).toBe(mockPayload.gm_id);
      expect(payload.type).toBe('refresh');
    });

    it('应该拒绝无效的Token', () => {
      expect(() => verifyToken('invalid.token.here')).toThrow();
    });

    it('应该拒绝过期的Token', () => {
      // 生成一个已经过期的Token
      const expiredToken = jwt.sign(
        { ...mockPayload, type: 'access' },
        process.env.JWT_SECRET!,
        { expiresIn: '-1s' } // 1秒前就过期
      );

      expect(() => verifyToken(expiredToken)).toThrow('jwt expired');
    });

    it('应该拒绝签名错误的Token', () => {
      const wrongToken = jwt.sign(
        { ...mockPayload, type: 'access' },
        'wrong-secret-key',
        { expiresIn: '1h' }
      );

      expect(() => verifyToken(wrongToken)).toThrow('invalid signature');
    });

    it('应该拒绝格式错误的Token', () => {
      expect(() => verifyToken('not-a-jwt-token')).toThrow();
      expect(() => verifyToken('')).toThrow();
      expect(() => verifyToken('a.b')).toThrow(); // 缺少部分
    });
  });

  describe('Token互操作性', () => {
    it('Access Token和Refresh Token应该有不同的类型标识', () => {
      const accessToken = generateAccessToken(mockPayload);
      const refreshToken = generateRefreshToken({
        gm_id: mockPayload.gm_id,
        username: mockPayload.username,
        gm_level: mockPayload.gm_level
      });

      const accessPayload = verifyToken(accessToken);
      const refreshPayload = verifyToken(refreshToken);

      expect(accessPayload.type).toBe('access');
      expect(refreshPayload.type).toBe('refresh');
    });

    it('应该能区分Token类型', () => {
      const accessToken = generateAccessToken(mockPayload);
      const refreshToken = generateRefreshToken({
        gm_id: mockPayload.gm_id,
        username: mockPayload.username,
        gm_level: mockPayload.gm_level
      });

      const accessPayload = verifyToken(accessToken);
      const refreshPayload = verifyToken(refreshToken);

      expect(accessPayload.type).not.toBe(refreshPayload.type);
    });
  });
});
