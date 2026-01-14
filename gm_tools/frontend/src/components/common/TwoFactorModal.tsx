/**
 * 2FA 验证 Modal
 */

import { FC, useState, useEffect, useRef } from 'react';
import { Modal, Input, Checkbox, Space, Typography, Alert } from 'antd';
import { use2FA } from '@/hooks/use2FA';

const { Text } = Typography;

interface TwoFactorModalProps {
  open: boolean;
  twoFactorToken: string;
  onClose: () => void;
}

const TwoFactorModal: FC<TwoFactorModalProps> = ({ open, twoFactorToken, onClose }) => {
  const [code, setCode] = useState('');
  const [rememberDevice, setRememberDevice] = useState(false);
  const { verify, isLoading, error } = use2FA();
  const inputRef = useRef<any>(null);

  // Modal 打开时自动聚焦输入框
  useEffect(() => {
    if (open && inputRef.current) {
      setTimeout(() => {
        inputRef.current?.focus();
      }, 100);
    }
  }, [open]);

  // 重置状态
  useEffect(() => {
    if (open) {
      setCode('');
      setRememberDevice(false);
    }
  }, [open]);

  const handleOk = async () => {
    if (!code || code.length !== 6) {
      return;
    }

    const success = await verify({
      two_factor_token: twoFactorToken,
      code,
      remember_device: rememberDevice,
    });

    if (success) {
      onClose();
    }
  };

  const handleCodeChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const value = e.target.value.replace(/\D/g, '').slice(0, 6);
    setCode(value);

    // 自动提交（输入 6 位后）
    if (value.length === 6) {
      setTimeout(() => {
        handleOk();
      }, 300);
    }
  };

  return (
    <Modal
      title="两步验证"
      open={open}
      onOk={handleOk}
      onCancel={onClose}
      okText="验证"
      cancelText="取消"
      confirmLoading={isLoading}
      okButtonProps={{ disabled: code.length !== 6 }}
      maskClosable={false}
    >
      <Space direction="vertical" style={{ width: '100%' }} size="large">
        <Text>请输入您的身份验证器应用中的 6 位验证码</Text>

        {error && <Alert message={error} type="error" showIcon />}

        <Input
          ref={inputRef}
          placeholder="6 位验证码"
          value={code}
          onChange={handleCodeChange}
          maxLength={6}
          size="large"
          style={{
            fontSize: 24,
            textAlign: 'center',
            letterSpacing: 8,
          }}
          autoComplete="off"
        />

        <Checkbox checked={rememberDevice} onChange={(e) => setRememberDevice(e.target.checked)}>
          信任此设备 30 天
        </Checkbox>

        <Text type="secondary" style={{ fontSize: 12 }}>
          提示：打开您的身份验证器应用（如 Google Authenticator），查看 DNF GM Tools
          对应的验证码
        </Text>
      </Space>
    </Modal>
  );
};

export default TwoFactorModal;
