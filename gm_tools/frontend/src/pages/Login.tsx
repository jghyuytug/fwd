/**
 * 登录页面
 */

import { FC, useState } from 'react';
import { Card, Form, Input, Button, Checkbox, Typography, Space, Divider } from 'antd';
import { UserOutlined, LockOutlined } from '@ant-design/icons';
import { useForm, Controller } from 'react-hook-form';
import { zodResolver } from '@hookform/resolvers/zod';
import { z } from 'zod';
import { useLogin } from '@/hooks/useLogin';
import TwoFactorModal from '@/components/common/TwoFactorModal';
import type { LoginRequest } from '@/types';

const { Title, Text } = Typography;

// 表单验证 Schema
const loginSchema = z.object({
  username: z
    .string()
    .min(3, '用户名至少 3 个字符')
    .max(50, '用户名最多 50 个字符')
    .regex(/^[a-zA-Z0-9_]+$/, '用户名只能包含字母、数字和下划线'),
  password: z.string().min(6, '密码至少 6 个字符').max(100, '密码最多 100 个字符'),
  remember: z.boolean().optional(),
});

type LoginFormData = z.infer<typeof loginSchema>;

const Login: FC = () => {
  const [twoFactorToken, setTwoFactorToken] = useState<string>('');
  const [show2FAModal, setShow2FAModal] = useState(false);
  const { login, isLoading } = useLogin();

  const {
    control,
    handleSubmit,
    formState: { errors },
  } = useForm<LoginFormData>({
    resolver: zodResolver(loginSchema),
    defaultValues: {
      username: '',
      password: '',
      remember: false,
    },
  });

  const onSubmit = async (data: LoginFormData) => {
    const response = await login(data as LoginRequest);

    // 如果需要 2FA 验证
    if (response?.two_factor_required && response.two_factor_token) {
      setTwoFactorToken(response.two_factor_token);
      setShow2FAModal(true);
    }
  };

  return (
    <div
      style={{
        display: 'flex',
        justifyContent: 'center',
        alignItems: 'center',
        minHeight: '100vh',
        background: 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)',
      }}
    >
      <Card
        style={{
          width: 450,
          boxShadow: '0 10px 40px rgba(0, 0, 0, 0.15)',
          borderRadius: 8,
        }}
      >
        <Space direction="vertical" size="large" style={{ width: '100%' }}>
          {/* 标题 */}
          <div style={{ textAlign: 'center' }}>
            <Title level={2} style={{ marginBottom: 8 }}>
              DNF GM Tools
            </Title>
            <Text type="secondary">游戏管理员操作平台</Text>
          </div>

          <Divider />

          {/* 登录表单 */}
          <Form layout="vertical" onFinish={handleSubmit(onSubmit)}>
            {/* 用户名 */}
            <Form.Item
              label="用户名"
              validateStatus={errors.username ? 'error' : ''}
              help={errors.username?.message}
            >
              <Controller
                name="username"
                control={control}
                render={({ field }) => (
                  <Input
                    {...field}
                    prefix={<UserOutlined />}
                    placeholder="请输入用户名"
                    size="large"
                    autoComplete="username"
                  />
                )}
              />
            </Form.Item>

            {/* 密码 */}
            <Form.Item
              label="密码"
              validateStatus={errors.password ? 'error' : ''}
              help={errors.password?.message}
            >
              <Controller
                name="password"
                control={control}
                render={({ field }) => (
                  <Input.Password
                    {...field}
                    prefix={<LockOutlined />}
                    placeholder="请输入密码"
                    size="large"
                    autoComplete="current-password"
                  />
                )}
              />
            </Form.Item>

            {/* 记住我 */}
            <Form.Item>
              <Controller
                name="remember"
                control={control}
                render={({ field }) => (
                  <Checkbox {...field} checked={field.value}>
                    记住我（7 天内自动登录）
                  </Checkbox>
                )}
              />
            </Form.Item>

            {/* 登录按钮 */}
            <Form.Item>
              <Button
                type="primary"
                htmlType="submit"
                size="large"
                block
                loading={isLoading}
              >
                登录
              </Button>
            </Form.Item>
          </Form>

          {/* 提示信息 */}
          <div style={{ textAlign: 'center' }}>
            <Text type="secondary" style={{ fontSize: 12 }}>
              首次登录可能需要进行两步验证
            </Text>
          </div>
        </Space>
      </Card>

      {/* 2FA 验证 Modal */}
      <TwoFactorModal
        open={show2FAModal}
        twoFactorToken={twoFactorToken}
        onClose={() => setShow2FAModal(false)}
      />
    </div>
  );
};

export default Login;
