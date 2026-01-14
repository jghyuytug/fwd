/**
 * 受信任设备管理 Hook
 */

import { useState, useEffect } from 'react';
import { message } from 'antd';
import { authApi } from '@/services/auth';
import type { TrustedDevice } from '@/types';

interface UseTrustedDevicesReturn {
  devices: TrustedDevice[];
  isLoading: boolean;
  error: string | null;
  refetch: () => Promise<void>;
  revokeDevice: (deviceId: string) => Promise<boolean>;
}

export const useTrustedDevices = (): UseTrustedDevicesReturn => {
  const [devices, setDevices] = useState<TrustedDevice[]>([]);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const fetchDevices = async () => {
    setIsLoading(true);
    setError(null);

    try {
      const response = await authApi.getTrustedDevices();
      setDevices(response);
      setIsLoading(false);
    } catch (err: any) {
      const errorMsg = err.response?.data?.message || err.message || '获取设备列表失败';
      setError(errorMsg);
      message.error(errorMsg);
      setIsLoading(false);
    }
  };

  const revokeDevice = async (deviceId: string): Promise<boolean> => {
    try {
      await authApi.revokeDevice(deviceId);
      message.success('设备信任已撤销');

      // 从列表中移除该设备
      setDevices((prev) => prev.filter((d) => d.device_id !== deviceId));

      return true;
    } catch (err: any) {
      const errorMsg = err.response?.data?.message || err.message || '撤销设备信任失败';
      message.error(errorMsg);
      return false;
    }
  };

  useEffect(() => {
    fetchDevices();
  }, []);

  return {
    devices,
    isLoading,
    error,
    refetch: fetchDevices,
    revokeDevice,
  };
};
