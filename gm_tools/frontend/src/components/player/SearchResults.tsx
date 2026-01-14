/**
 * 玩家搜索结果组件
 */

import { FC } from 'react';
import { Table, Tag, Button, Space, Typography } from 'antd';
import { EyeOutlined, ShoppingOutlined, InboxOutlined } from '@ant-design/icons';
import { useNavigate } from 'react-router-dom';
import type { PlayerSearchResult } from '@/types/player';
import type { ColumnsType, TablePaginationConfig } from 'antd/es/table';
import dayjs from 'dayjs';
import relativeTime from 'dayjs/plugin/relativeTime';
import 'dayjs/locale/zh-cn';

dayjs.extend(relativeTime);
dayjs.locale('zh-cn');

const { Text } = Typography;

interface SearchResultsProps {
  data: PlayerSearchResult[];
  loading: boolean;
  pagination: TablePaginationConfig;
  onChange: (pagination: TablePaginationConfig) => void;
}

const SearchResults: FC<SearchResultsProps> = ({ data, loading, pagination, onChange }) => {
  const navigate = useNavigate();

  // 状态标签
  const getStatusTag = (status: PlayerSearchResult['status']) => {
    const statusConfig = {
      active: { color: 'success', text: '正常' },
      banned: { color: 'error', text: '封禁' },
      suspended: { color: 'warning', text: '冻结' },
    };
    const config = statusConfig[status];
    return <Tag color={config.color}>{config.text}</Tag>;
  };

  // 职业显示
  const renderJob = (record: PlayerSearchResult) => {
    return (
      <Space direction="vertical" size={0}>
        <Text strong>{record.job_name}</Text>
        {record.grow_type && (
          <Text type="secondary" style={{ fontSize: 12 }}>
            {record.grow_type}
          </Text>
        )}
      </Space>
    );
  };

  const columns: ColumnsType<PlayerSearchResult> = [
    {
      title: '角色名',
      dataIndex: 'character_name',
      key: 'character_name',
      width: 150,
      render: (name: string, record) => (
        <Button
          type="link"
          onClick={() => navigate(`/player/${record.character_id}`)}
          style={{ padding: 0 }}
        >
          {name}
        </Button>
      ),
    },
    {
      title: '职业',
      key: 'job',
      width: 150,
      render: (_, record) => renderJob(record),
    },
    {
      title: '等级',
      dataIndex: 'level',
      key: 'level',
      width: 80,
      align: 'center',
      render: (level: number) => <Tag color="blue">Lv.{level}</Tag>,
    },
    {
      title: '账号ID',
      dataIndex: 'account_name',
      key: 'account_name',
      width: 150,
    },
    {
      title: '状态',
      dataIndex: 'status',
      key: 'status',
      width: 100,
      align: 'center',
      render: (status: PlayerSearchResult['status']) => getStatusTag(status),
    },
    {
      title: '最后登录',
      dataIndex: 'last_login',
      key: 'last_login',
      width: 180,
      render: (date?: string) => {
        if (!date) return <Text type="secondary">从未登录</Text>;
        return (
          <Space direction="vertical" size={0}>
            <Text>{dayjs(date).format('MM-DD HH:mm')}</Text>
            <Text type="secondary" style={{ fontSize: 12 }}>
              {dayjs(date).fromNow()}
            </Text>
          </Space>
        );
      },
    },
    {
      title: '操作',
      key: 'action',
      width: 200,
      fixed: 'right',
      render: (_, record) => (
        <Space size="small">
          <Button
            type="link"
            size="small"
            icon={<EyeOutlined />}
            onClick={() => navigate(`/player/${record.character_id}`)}
          >
            详情
          </Button>
          <Button
            type="link"
            size="small"
            icon={<ShoppingOutlined />}
            onClick={() => navigate(`/player/${record.character_id}?tab=equipment`)}
          >
            装备
          </Button>
          <Button
            type="link"
            size="small"
            icon={<InboxOutlined />}
            onClick={() => navigate(`/player/${record.character_id}?tab=warehouse`)}
          >
            仓库
          </Button>
        </Space>
      ),
    },
  ];

  return (
    <Table
      columns={columns}
      dataSource={data}
      rowKey="character_id"
      loading={loading}
      pagination={pagination}
      onChange={onChange}
      scroll={{ x: 1000 }}
    />
  );
};

export default SearchResults;
