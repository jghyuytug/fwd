/**
 * 2FA 验证相关 Hook
 */

import { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { message } from 'antd';
import { authApi } from '@/services/auth';
import { useAuthStore } from '@/stores/useAuthStore';
import type { TwoFactorRequest } from '@/types';

interface Use2FAReturn {
  verify: (data: TwoFactorRequest) => Promise<boolean>;
  isLoading: boolean;
  error: string | null;
}

export const use2FA = (): Use2FAReturn => {
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const navigate = useNavigate();
  const { setUser, setTokens } = useAuthStore();

  const verify = async (data: TwoFactorRequest): Promise<boolean> => {
    setIsLoading(true);
    setError(null);

    try {
      const response = await authApi.verifyTwoFactor(data);

      // 2FA 验证成功，保存用户信息和 Token
      setUser(response.user);
      setTokens(response.access_token, response.refresh_token, response.expires_in);

      message.success('验证成功');
      setIsLoading(false);

      // 跳转到首页
      navigate('/dashboard');

      return true;
    } catch (err: any) {
      const errorMsg = err.response?.data?.message || err.message || '验证失败';
      setError(errorMsg);
      message.error(errorMsg);
      setIsLoading(false);
      return false;
    }
  };

  return { verify, isLoading, error };
};
