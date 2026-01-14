/**
 * 403 权限不足页面
 */

import { FC } from 'react';
import { Result, Button } from 'antd';
import { useNavigate } from 'react-router-dom';

const Forbidden: FC = () => {
  const navigate = useNavigate();

  return (
    <Result
      status="403"
      title="403"
      subTitle="抱歉，您没有权限访问此页面"
      extra={
        <Button type="primary" onClick={() => navigate('/')}>
          返回首页
        </Button>
      }
    />
  );
};

export default Forbidden;
