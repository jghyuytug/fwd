/**
 * 受信任设备管理页面
 */

import { FC } from 'react';
import { Card, Table, Button, Space, Tag, Typography, Modal, Alert, Tooltip } from 'antd';
import {
  DesktopOutlined,
  MobileOutlined,
  TabletOutlined,
  GlobalOutlined,
  DeleteOutlined,
  ReloadOutlined,
  CheckCircleOutlined,
} from '@ant-design/icons';
import { PageHeader } from '@/components/common';
import { useTrustedDevices } from '@/hooks/useTrustedDevices';
import type { TrustedDevice } from '@/types';
import type { ColumnsType } from 'antd/es/table';
import dayjs from 'dayjs';
import relativeTime from 'dayjs/plugin/relativeTime';
import 'dayjs/locale/zh-cn';

dayjs.extend(relativeTime);
dayjs.locale('zh-cn');

const { Text } = Typography;

const TrustedDevices: FC = () => {
  const { devices, isLoading, refetch, revokeDevice } = useTrustedDevices();

  // 获取设备类型图标
  const getDeviceIcon = (device: TrustedDevice) => {
    const ua = device.user_agent.toLowerCase();
    if (ua.includes('mobile') || ua.includes('android') || ua.includes('iphone')) {
      return <MobileOutlined style={{ fontSize: 20, color: '#1890ff' }} />;
    }
    if (ua.includes('ipad') || ua.includes('tablet')) {
      return <TabletOutlined style={{ fontSize: 20, color: '#52c41a' }} />;
    }
    return <DesktopOutlined style={{ fontSize: 20, color: '#722ed1' }} />;
  };

  // 撤销设备确认
  const handleRevokeDevice = (device: TrustedDevice) => {
    Modal.confirm({
      title: '撤销设备信任',
      content: (
        <div>
          <p>确定要撤销以下设备的信任吗？</p>
          <p>
            <Text strong>{device.device_name || device.browser || '未知设备'}</Text>
          </p>
          <p>
            <Text type="secondary">IP: {device.ip_address}</Text>
          </p>
          <Alert
            message="撤销后，该设备下次登录时需要重新进行两步验证"
            type="warning"
            showIcon
            style={{ marginTop: 12 }}
          />
        </div>
      ),
      okText: '确定撤销',
      cancelText: '取消',
      okButtonProps: { danger: true },
      onOk: async () => {
        await revokeDevice(device.device_id);
      },
    });
  };

  // 表格列定义
  const columns: ColumnsType<TrustedDevice> = [
    {
      title: '设备',
      key: 'device',
      width: 300,
      render: (_, record) => (
        <Space>
          {getDeviceIcon(record)}
          <div>
            <div>
              <Text strong>
                {record.device_name || record.browser || '未知设备'}
                {record.is_current && (
                  <Tag color="green" style={{ marginLeft: 8 }}>
                    <CheckCircleOutlined /> 当前设备
                  </Tag>
                )}
              </Text>
            </div>
            <div>
              <Text type="secondary" style={{ fontSize: 12 }}>
                {record.os || '未知系统'}
              </Text>
            </div>
          </div>
        </Space>
      ),
    },
    {
      title: '浏览器',
      dataIndex: 'browser',
      key: 'browser',
      width: 150,
      render: (browser: string) => (
        <Space>
          <GlobalOutlined />
          <Text>{browser || '未知浏览器'}</Text>
        </Space>
      ),
    },
    {
      title: 'IP 地址',
      dataIndex: 'ip_address',
      key: 'ip_address',
      width: 150,
    },
    {
      title: '信任时间',
      dataIndex: 'trusted_at',
      key: 'trusted_at',
      width: 180,
      render: (date: string) => (
        <Tooltip title={dayjs(date).format('YYYY-MM-DD HH:mm:ss')}>
          <Text>{dayjs(date).fromNow()}</Text>
        </Tooltip>
      ),
    },
    {
      title: '最后使用',
      dataIndex: 'last_used_at',
      key: 'last_used_at',
      width: 180,
      render: (date: string) => (
        <Tooltip title={dayjs(date).format('YYYY-MM-DD HH:mm:ss')}>
          <Text>{dayjs(date).fromNow()}</Text>
        </Tooltip>
      ),
    },
    {
      title: '过期时间',
      dataIndex: 'expires_at',
      key: 'expires_at',
      width: 180,
      render: (date: string) => {
        const isExpired = dayjs(date).isBefore(dayjs());
        return (
          <Tooltip title={dayjs(date).format('YYYY-MM-DD HH:mm:ss')}>
            <Text type={isExpired ? 'danger' : undefined}>
              {isExpired ? '已过期' : dayjs(date).fromNow()}
            </Text>
          </Tooltip>
        );
      },
    },
    {
      title: '操作',
      key: 'action',
      width: 120,
      fixed: 'right',
      render: (_, record) => (
        <Button
          type="link"
          danger
          icon={<DeleteOutlined />}
          onClick={() => handleRevokeDevice(record)}
          disabled={record.is_current}
        >
          撤销信任
        </Button>
      ),
    },
  ];

  return (
    <div>
      <PageHeader
        title="受信任设备"
        subTitle="管理您的受信任设备，撤销不再使用的设备"
        extra={
          <Button icon={<ReloadOutlined />} onClick={refetch} loading={isLoading}>
            刷新
          </Button>
        }
      />

      <Card>
        <Space direction="vertical" size="large" style={{ width: '100%' }}>
          <Alert
            message="关于受信任设备"
            description={
              <ul style={{ marginBottom: 0, paddingLeft: 20 }}>
                <li>在登录时选择"信任此设备"，可在 30 天内免除两步验证</li>
                <li>建议仅在个人设备上信任，不要在公共设备上使用此功能</li>
                <li>如发现异常设备，请立即撤销信任并修改密码</li>
                <li>当前设备不可撤销，请在其他设备上操作</li>
              </ul>
            }
            type="info"
            showIcon
          />

          <Table
            columns={columns}
            dataSource={devices}
            rowKey="device_id"
            loading={isLoading}
            pagination={{
              pageSize: 10,
              showSizeChanger: true,
              showTotal: (total) => `共 ${total} 个设备`,
            }}
            scroll={{ x: 1200 }}
          />
        </Space>
      </Card>
    </div>
  );
};

export default TrustedDevices;
