/**
 * 玩家搜索表单组件
 */

import { FC } from 'react';
import { Form, Input, Select, Button, Space, Checkbox, Card } from 'antd';
import { SearchOutlined, ReloadOutlined } from '@ant-design/icons';
import type { PlayerSearchParams } from '@/types/player';

interface SearchFormProps {
  onSearch: (values: PlayerSearchParams) => void;
  loading?: boolean;
}

const SearchForm: FC<SearchFormProps> = ({ onSearch, loading }) => {
  const [form] = Form.useForm<PlayerSearchParams>();

  const handleSubmit = (values: PlayerSearchParams) => {
    onSearch(values);
  };

  const handleReset = () => {
    form.resetFields();
  };

  return (
    <Card>
      <Form
        form={form}
        layout="inline"
        onFinish={handleSubmit}
        initialValues={{
          search_type: 'character',
          fuzzy: false,
        }}
      >
        <Form.Item
          name="search_type"
          label="搜索类型"
          rules={[{ required: true, message: '请选择搜索类型' }]}
        >
          <Select style={{ width: 150 }}>
            <Select.Option value="character">角色名</Select.Option>
            <Select.Option value="account">账号ID</Select.Option>
            <Select.Option value="character_id">角色ID</Select.Option>
          </Select>
        </Form.Item>

        <Form.Item
          name="keyword"
          label="搜索内容"
          rules={[
            { required: true, message: '请输入搜索内容' },
            { min: 2, message: '至少输入2个字符' },
          ]}
        >
          <Input
            placeholder="请输入搜索关键词"
            style={{ width: 300 }}
            allowClear
          />
        </Form.Item>

        <Form.Item name="fuzzy" valuePropName="checked">
          <Checkbox>模糊搜索</Checkbox>
        </Form.Item>

        <Form.Item>
          <Space>
            <Button
              type="primary"
              htmlType="submit"
              icon={<SearchOutlined />}
              loading={loading}
            >
              搜索
            </Button>
            <Button icon={<ReloadOutlined />} onClick={handleReset}>
              重置
            </Button>
          </Space>
        </Form.Item>
      </Form>
    </Card>
  );
};

export default SearchForm;
