/**
 * 认证相关 API
 */

import { http } from '@/utils/request';
import type {
  LoginRequest,
  LoginResponse,
  TwoFactorRequest,
  RefreshTokenResponse,
  User,
  TrustedDevice,
} from '@/types';

export const authApi = {
  // 登录
  login: (data: LoginRequest) => {
    return http.post<LoginResponse>('/auth/login', data);
  },

  // 2FA 验证
  verifyTwoFactor: (data: TwoFactorRequest) => {
    return http.post<LoginResponse>('/auth/2fa/verify', data);
  },

  // 刷新 token
  refreshToken: (refreshToken: string) => {
    return http.post<RefreshTokenResponse>('/auth/refresh', {
      refresh_token: refreshToken,
    });
  },

  // 登出
  logout: () => {
    return http.post('/auth/logout');
  },

  // 获取当前用户信息
  getCurrentUser: () => {
    return http.get<User>('/auth/me');
  },

  // 修改密码
  changePassword: (oldPassword: string, newPassword: string) => {
    return http.post('/auth/password/change', {
      old_password: oldPassword,
      new_password: newPassword,
    });
  },

  // 获取受信任设备列表
  getTrustedDevices: () => {
    return http.get<TrustedDevice[]>('/auth/devices');
  },

  // 撤销设备信任
  revokeDevice: (deviceId: string) => {
    return http.delete(`/auth/devices/${deviceId}`);
  },
};
