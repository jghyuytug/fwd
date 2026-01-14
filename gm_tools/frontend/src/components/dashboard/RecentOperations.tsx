/**
 * Dashboard 最近操作组件
 */

import { FC } from 'react';
import { Card, Table, Tag, Space, Typography, Button } from 'antd';
import { HistoryOutlined, EyeOutlined } from '@ant-design/icons';
import { useNavigate } from 'react-router-dom';
import type { RecentOperation } from '@/types';
import type { ColumnsType } from 'antd/es/table';
import dayjs from 'dayjs';
import relativeTime from 'dayjs/plugin/relativeTime';
import 'dayjs/locale/zh-cn';

dayjs.extend(relativeTime);
dayjs.locale('zh-cn');

const { Text } = Typography;

interface RecentOperationsProps {
  operations: RecentOperation[];
  loading: boolean;
}

const RecentOperations: FC<RecentOperationsProps> = ({ operations, loading }) => {
  const navigate = useNavigate();

  // 操作类型颜色映射
  const getOperationTypeColor = (type: string): string => {
    const colorMap: Record<string, string> = {
      item_grant: 'blue',
      account_ban: 'red',
      account_unban: 'green',
      account_edit: 'orange',
      player_query: 'cyan',
      system_config: 'purple',
    };
    return colorMap[type] || 'default';
  };

  // 操作类型中文映射
  const getOperationTypeLabel = (type: string): string => {
    const labelMap: Record<string, string> = {
      item_grant: '道具发放',
      account_ban: '账号封禁',
      account_unban: '解除封禁',
      account_edit: '账号编辑',
      player_query: '玩家查询',
      system_config: '系统配置',
    };
    return labelMap[type] || type;
  };

  // 状态标签
  const getStatusTag = (status: RecentOperation['status']) => {
    const statusConfig = {
      success: { color: 'success', text: '成功' },
      failed: { color: 'error', text: '失败' },
      pending: { color: 'processing', text: '处理中' },
    };
    const config = statusConfig[status];
    // 如果 config 不存在，使用默认值
    if (!config) {
      console.warn('[RecentOperations] Unknown status:', status);
      return <Tag color="default">{status || '未知'}</Tag>;
    }
    return <Tag color={config.color}>{config.text}</Tag>;
  };

  const columns: ColumnsType<RecentOperation> = [
    {
      title: '操作类型',
      dataIndex: 'operation_type',
      key: 'operation_type',
      width: 120,
      render: (type: string) => (
        <Tag color={getOperationTypeColor(type)}>{getOperationTypeLabel(type)}</Tag>
      ),
    },
    {
      title: '操作描述',
      dataIndex: 'operation_desc',
      key: 'operation_desc',
      ellipsis: true,
    },
    {
      title: '操作员',
      dataIndex: 'operator',
      key: 'operator',
      width: 120,
    },
    {
      title: '目标用户',
      dataIndex: 'target_user',
      key: 'target_user',
      width: 120,
      render: (user?: string) => user || '-',
    },
    {
      title: '状态',
      dataIndex: 'status',
      key: 'status',
      width: 100,
      render: (status: RecentOperation['status']) => getStatusTag(status),
    },
    {
      title: '操作时间',
      dataIndex: 'created_at',
      key: 'created_at',
      width: 160,
      render: (date: string) => (
        <Space direction="vertical" size={0}>
          <Text>{dayjs(date).format('MM-DD HH:mm')}</Text>
          <Text type="secondary" style={{ fontSize: 12 }}>
            {dayjs(date).fromNow()}
          </Text>
        </Space>
      ),
    },
  ];

  return (
    <Card
      title={
        <Space>
          <HistoryOutlined />
          <span>最近操作</span>
        </Space>
      }
      extra={
        <Button
          type="link"
          icon={<EyeOutlined />}
          onClick={() => navigate('/audit/operation')}
        >
          查看全部
        </Button>
      }
    >
      <Table
        columns={columns}
        dataSource={operations}
        rowKey="operation_id"
        loading={loading}
        pagination={false}
        size="small"
        scroll={{ x: 800 }}
      />
    </Card>
  );
};

export default RecentOperations;
