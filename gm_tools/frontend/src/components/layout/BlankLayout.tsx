/**
 * 空白布局组件（用于登录页等）
 */

import { FC } from 'react';
import { Outlet } from 'react-router-dom';

const BlankLayout: FC = () => {
  return (
    <div style={{ minHeight: '100vh', background: '#f0f2f5' }}>
      <Outlet />
    </div>
  );
};

export default BlankLayout;
