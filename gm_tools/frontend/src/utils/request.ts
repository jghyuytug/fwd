/**
 * Axios HTTP 客户端封装
 */

import axios, { AxiosInstance, AxiosRequestConfig, AxiosResponse, AxiosError } from 'axios';
import { message } from 'antd';
import { tokenStorage } from './storage';
import type { ApiResponse } from '@/types';

// 创建 axios 实例
const request: AxiosInstance = axios.create({
  baseURL: import.meta.env.VITE_API_BASE_URL || '/api',
  timeout: parseInt(import.meta.env.VITE_API_TIMEOUT) || 30000,
  headers: {
    'Content-Type': 'application/json',
  },
});

// 请求拦截器
request.interceptors.request.use(
  (config) => {
    // 添加 token
    const token = tokenStorage.getAccessToken();
    if (token) {
      config.headers.Authorization = `Bearer ${token}`;
    }
    return config;
  },
  (error) => {
    console.error('Request error:', error);
    return Promise.reject(error);
  }
);

// 响应拦截器
request.interceptors.response.use(
  (response: AxiosResponse<ApiResponse>) => {
    const { code, message: msg, data } = response.data;

    // 成功响应
    if (code === 0 || code === 200) {
      return response;
    }

    // 业务错误
    message.error(msg || '请求失败');
    return Promise.reject(new Error(msg || '请求失败'));
  },
  async (error: AxiosError<ApiResponse>) => {
    const { response } = error;

    if (!response) {
      message.error('网络错误，请检查网络连接');
      return Promise.reject(error);
    }

    const { status, data } = response;

    switch (status) {
      case 401:
        // Token 过期，静默清除并跳转登录
        tokenStorage.clearTokens();
        // 不显示错误提示，直接跳转
        window.location.href = '/login';
        break;

      case 403:
        message.error('权限不足，无法访问');
        break;

      case 404:
        message.error('请求的资源不存在');
        break;

      case 500:
        message.error('服务器错误，请稍后重试');
        break;

      default:
        message.error(data?.message || '请求失败');
    }

    return Promise.reject(error);
  }
);

// 封装常用方法
export const http = {
  get<T = any>(url: string, config?: AxiosRequestConfig): Promise<T> {
    return request.get<ApiResponse<T>>(url, config).then((res) => res.data.data);
  },

  post<T = any>(url: string, data?: any, config?: AxiosRequestConfig): Promise<T> {
    return request.post<ApiResponse<T>>(url, data, config).then((res) => res.data.data);
  },

  put<T = any>(url: string, data?: any, config?: AxiosRequestConfig): Promise<T> {
    return request.put<ApiResponse<T>>(url, data, config).then((res) => res.data.data);
  },

  delete<T = any>(url: string, config?: AxiosRequestConfig): Promise<T> {
    return request.delete<ApiResponse<T>>(url, config).then((res) => res.data.data);
  },

  patch<T = any>(url: string, data?: any, config?: AxiosRequestConfig): Promise<T> {
    return request.patch<ApiResponse<T>>(url, data, config).then((res) => res.data.data);
  },
};

export default request;
