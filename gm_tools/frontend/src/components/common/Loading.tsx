/**
 * Loading 加载组件
 */

import { Spin } from 'antd';
import { FC } from 'react';

interface LoadingProps {
  tip?: string;
  fullscreen?: boolean;
  size?: 'small' | 'default' | 'large';
}

const Loading: FC<LoadingProps> = ({ tip = '加载中...', fullscreen = false, size = 'large' }) => {
  if (fullscreen) {
    return (
      <div className="loading-overlay">
        <Spin size={size} tip={tip} />
      </div>
    );
  }

  return (
    <div style={{ textAlign: 'center', padding: '50px 0' }}>
      <Spin size={size} tip={tip} />
    </div>
  );
};

export default Loading;
