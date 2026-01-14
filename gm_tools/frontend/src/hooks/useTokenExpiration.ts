/**
 * Token过期检查Hook
 * 定时检查token是否过期，过期则重定向到登录页
 */

import { useEffect } from 'react';
import { useNavigate, useLocation } from 'react-router-dom';
import { tokenStorage } from '@/utils/storage';
import { useAuthStore } from '@/stores/useAuthStore';

export function useTokenExpiration() {
  const navigate = useNavigate();
  const location = useLocation();
  const { clearAuth } = useAuthStore();

  useEffect(() => {
    const checkTokenExpiration = () => {
      const pathname = location.pathname;

      // 如果已经在登录页，不需要检查
      if (pathname === '/login') {
        return;
      }

      const hasToken = tokenStorage.getAccessToken();

      // 如果没有 token 或 token 已过期，静默清除并跳转
      if (!hasToken || tokenStorage.isTokenExpired()) {
        // 静默清除所有认证信息，不显示任何提示
        tokenStorage.clearTokens();
        clearAuth();
        // 只在需要登录的页面才跳转
        if (pathname !== '/login' && pathname !== '/403') {
          navigate('/login', { replace: true });
        }
      }
    };

    // 立即检查一次
    checkTokenExpiration();

    // 每60秒检查一次
    const intervalId = setInterval(checkTokenExpiration, 60000);

    return () => {
      clearInterval(intervalId);
    };
  }, [navigate, location.pathname, clearAuth]);
}
