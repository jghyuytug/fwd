/**
 * 修改密码页面
 */

import { FC } from 'react';
import { Card, Form, Input, Button, message, Space, Typography, Alert } from 'antd';
import { LockOutlined } from '@ant-design/icons';
import { useForm, Controller } from 'react-hook-form';
import { zodResolver } from '@hookform/resolvers/zod';
import { z } from 'zod';
import { PageHeader } from '@/components/common';
import { authApi } from '@/services/auth';
import { useNavigate } from 'react-router-dom';

const { Text } = Typography;

// 密码强度验证
const passwordStrength = z
  .string()
  .min(8, '密码至少 8 个字符')
  .max(100, '密码最多 100 个字符')
  .regex(/[a-z]/, '密码必须包含小写字母')
  .regex(/[A-Z]/, '密码必须包含大写字母')
  .regex(/[0-9]/, '密码必须包含数字')
  .regex(/[@$!%*?&]/, '密码必须包含特殊字符 (@$!%*?&)');

// 表单验证 Schema
const changePasswordSchema = z
  .object({
    oldPassword: z.string().min(1, '请输入当前密码'),
    newPassword: passwordStrength,
    confirmPassword: z.string().min(1, '请确认新密码'),
  })
  .refine((data) => data.newPassword === data.confirmPassword, {
    message: '两次输入的密码不一致',
    path: ['confirmPassword'],
  })
  .refine((data) => data.oldPassword !== data.newPassword, {
    message: '新密码不能与当前密码相同',
    path: ['newPassword'],
  });

type ChangePasswordFormData = z.infer<typeof changePasswordSchema>;

const ChangePassword: FC = () => {
  const navigate = useNavigate();

  const {
    control,
    handleSubmit,
    formState: { errors, isSubmitting },
    reset,
  } = useForm<ChangePasswordFormData>({
    resolver: zodResolver(changePasswordSchema),
    defaultValues: {
      oldPassword: '',
      newPassword: '',
      confirmPassword: '',
    },
  });

  const onSubmit = async (data: ChangePasswordFormData) => {
    try {
      await authApi.changePassword(data.oldPassword, data.newPassword);
      message.success('密码修改成功，请重新登录');
      reset();
      // 跳转到登录页
      setTimeout(() => {
        navigate('/login');
      }, 1500);
    } catch (error: any) {
      message.error(error.response?.data?.message || '密码修改失败');
    }
  };

  return (
    <div>
      <PageHeader title="修改密码" subTitle="修改您的登录密码" />

      <Card style={{ maxWidth: 600 }}>
        <Space direction="vertical" size="large" style={{ width: '100%' }}>
          <Alert
            message="密码要求"
            description={
              <ul style={{ marginBottom: 0, paddingLeft: 20 }}>
                <li>至少 8 个字符</li>
                <li>必须包含大写字母、小写字母、数字和特殊字符</li>
                <li>不能与当前密码相同</li>
              </ul>
            }
            type="info"
            showIcon
          />

          <Form layout="vertical" onFinish={handleSubmit(onSubmit)}>
            {/* 当前密码 */}
            <Form.Item
              label="当前密码"
              validateStatus={errors.oldPassword ? 'error' : ''}
              help={errors.oldPassword?.message}
              required
            >
              <Controller
                name="oldPassword"
                control={control}
                render={({ field }) => (
                  <Input.Password
                    {...field}
                    prefix={<LockOutlined />}
                    placeholder="请输入当前密码"
                    size="large"
                    autoComplete="current-password"
                  />
                )}
              />
            </Form.Item>

            {/* 新密码 */}
            <Form.Item
              label="新密码"
              validateStatus={errors.newPassword ? 'error' : ''}
              help={errors.newPassword?.message}
              required
            >
              <Controller
                name="newPassword"
                control={control}
                render={({ field }) => (
                  <Input.Password
                    {...field}
                    prefix={<LockOutlined />}
                    placeholder="请输入新密码"
                    size="large"
                    autoComplete="new-password"
                  />
                )}
              />
            </Form.Item>

            {/* 确认新密码 */}
            <Form.Item
              label="确认新密码"
              validateStatus={errors.confirmPassword ? 'error' : ''}
              help={errors.confirmPassword?.message}
              required
            >
              <Controller
                name="confirmPassword"
                control={control}
                render={({ field }) => (
                  <Input.Password
                    {...field}
                    prefix={<LockOutlined />}
                    placeholder="请再次输入新密码"
                    size="large"
                    autoComplete="new-password"
                  />
                )}
              />
            </Form.Item>

            {/* 按钮 */}
            <Form.Item style={{ marginBottom: 0 }}>
              <Space>
                <Button
                  type="primary"
                  htmlType="submit"
                  size="large"
                  loading={isSubmitting}
                >
                  修改密码
                </Button>
                <Button size="large" onClick={() => navigate(-1)}>
                  取消
                </Button>
              </Space>
            </Form.Item>
          </Form>

          <Text type="secondary" style={{ fontSize: 12 }}>
            提示：修改密码后需要重新登录
          </Text>
        </Space>
      </Card>
    </div>
  );
};

export default ChangePassword;
