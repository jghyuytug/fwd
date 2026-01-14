/**
 * 玩家搜索页面
 */

import { FC, useState } from 'react';
import { Space, Card, message } from 'antd';
import { PageHeader } from '@/components/common';
import SearchForm from '@/components/player/SearchForm';
import SearchResults from '@/components/player/SearchResults';
import { playerApi } from '@/services/player';
import type { PlayerSearchParams, PlayerSearchResult } from '@/types/player';
import type { TablePaginationConfig } from 'antd/es/table';

const PlayerSearch: FC = () => {
  const [searchParams, setSearchParams] = useState<PlayerSearchParams | null>(null);
  const [results, setResults] = useState<PlayerSearchResult[]>([]);
  const [loading, setLoading] = useState(false);
  const [pagination, setPagination] = useState<TablePaginationConfig>({
    current: 1,
    pageSize: 20,
    total: 0,
    showSizeChanger: true,
    showTotal: (total) => `共 ${total} 条记录`,
    pageSizeOptions: ['10', '20', '50', '100'],
  });

  // 执行搜索
  const handleSearch = async (params: PlayerSearchParams) => {
    setLoading(true);
    setSearchParams(params);

    try {
      const response = await playerApi.search({
        ...params,
        page: 1,
        page_size: pagination.pageSize || 20,
      });

      setResults(response.items);
      setPagination({
        ...pagination,
        current: response.page,
        total: response.total,
      });
    } catch (error: any) {
      message.error(error.response?.data?.message || '搜索失败');
      setResults([]);
      setPagination({
        ...pagination,
        current: 1,
        total: 0,
      });
    } finally {
      setLoading(false);
    }
  };

  // 分页变化
  const handlePaginationChange = async (newPagination: TablePaginationConfig) => {
    if (!searchParams) return;

    setLoading(true);

    try {
      const response = await playerApi.search({
        ...searchParams,
        page: newPagination.current || 1,
        page_size: newPagination.pageSize || 20,
      });

      setResults(response.items);
      setPagination({
        ...pagination,
        current: response.page,
        pageSize: newPagination.pageSize,
        total: response.total,
      });
    } catch (error: any) {
      message.error(error.response?.data?.message || '加载数据失败');
    } finally {
      setLoading(false);
    }
  };

  return (
    <div>
      <PageHeader
        title="玩家查询"
        subTitle="搜索玩家角色和账号信息"
      />

      <Space direction="vertical" size="large" style={{ width: '100%' }}>
        {/* 搜索表单 */}
        <SearchForm onSearch={handleSearch} loading={loading} />

        {/* 搜索结果 */}
        {searchParams && (
          <Card title="搜索结果">
            <SearchResults
              data={results}
              loading={loading}
              pagination={pagination}
              onChange={handlePaginationChange}
            />
          </Card>
        )}
      </Space>
    </div>
  );
};

export default PlayerSearch;
