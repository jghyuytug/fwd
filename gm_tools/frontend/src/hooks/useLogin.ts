/**
 * 登录相关 Hook
 */

import { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { message } from 'antd';
import { authApi } from '@/services/auth';
import { useAuthStore } from '@/stores/useAuthStore';
import type { LoginRequest, LoginResponse } from '@/types';

interface UseLoginReturn {
  login: (data: LoginRequest) => Promise<LoginResponse | null>;
  isLoading: boolean;
  error: string | null;
}

export const useLogin = (): UseLoginReturn => {
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const navigate = useNavigate();
  const { setUser, setTokens } = useAuthStore();

  const login = async (data: LoginRequest): Promise<LoginResponse | null> => {
    setIsLoading(true);
    setError(null);

    try {
      const response = await authApi.login(data);

      // 如果需要 2FA 验证
      if (response.two_factor_required) {
        setIsLoading(false);
        return response; // 返回包含 two_factor_token 的响应
      }

      // 不需要 2FA，直接登录成功
      setUser(response.user);
      setTokens(response.access_token, response.refresh_token, response.expires_in);

      message.success('登录成功');
      setIsLoading(false);

      // 跳转到首页
      navigate('/dashboard');

      return response;
    } catch (err: any) {
      const errorMsg = err.response?.data?.message || err.message || '登录失败';
      setError(errorMsg);
      message.error(errorMsg);
      setIsLoading(false);
      return null;
    }
  };

  return { login, isLoading, error };
};
