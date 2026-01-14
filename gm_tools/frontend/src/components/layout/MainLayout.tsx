/**
 * 主布局组件
 */

import { FC, useState } from 'react';
import { Layout, Menu, Dropdown, Avatar, Space, Button, Breadcrumb } from 'antd';
import {
  MenuFoldOutlined,
  MenuUnfoldOutlined,
  DashboardOutlined,
  UserOutlined,
  ToolOutlined,
  AuditOutlined,
  SettingOutlined,
  LogoutOutlined,
  SearchOutlined,
  LockOutlined,
  MobileOutlined,
} from '@ant-design/icons';
import { Outlet, useNavigate, useLocation, Link } from 'react-router-dom';
import { useAuthStore } from '@/stores/useAuthStore';
import type { MenuProps } from 'antd';

const { Header, Sider, Content } = Layout;

const MainLayout: FC = () => {
  const [collapsed, setCollapsed] = useState(false);
  const navigate = useNavigate();
  const location = useLocation();
  const { user, clearAuth } = useAuthStore();

  // 侧边栏菜单项
  const menuItems: MenuProps['items'] = [
    {
      key: '/dashboard',
      icon: <DashboardOutlined />,
      label: '控制台',
    },
    {
      key: '/player',
      icon: <SearchOutlined />,
      label: '玩家查询',
    },
    {
      key: '/gm',
      icon: <ToolOutlined />,
      label: 'GM操作',
      children: [
        {
          key: '/gm/item-grant',
          label: '道具发放',
        },
        {
          key: '/gm/account-manage',
          label: '账号管理',
        },
      ],
    },
    {
      key: '/audit',
      icon: <AuditOutlined />,
      label: '审计日志',
      children: [
        {
          key: '/audit/operation',
          label: '操作日志',
        },
        {
          key: '/audit/login',
          label: '登录日志',
        },
      ],
    },
    {
      key: '/system',
      icon: <SettingOutlined />,
      label: '系统管理',
      children: [
        {
          key: '/system/permissions',
          label: '权限管理',
        },
        {
          key: '/system/roles',
          label: '角色管理',
        },
      ],
    },
  ];

  // 用户下拉菜单
  const userMenuItems: MenuProps['items'] = [
    {
      key: 'profile',
      icon: <UserOutlined />,
      label: '个人信息',
    },
    {
      key: 'password',
      icon: <LockOutlined />,
      label: '修改密码',
    },
    {
      key: 'devices',
      icon: <MobileOutlined />,
      label: '受信任设备',
    },
    {
      key: 'settings',
      icon: <SettingOutlined />,
      label: '设置',
    },
    {
      type: 'divider',
    },
    {
      key: 'logout',
      icon: <LogoutOutlined />,
      label: '退出登录',
      danger: true,
    },
  ];

  const handleMenuClick = ({ key }: { key: string }) => {
    navigate(key);
  };

  const handleUserMenuClick: MenuProps['onClick'] = ({ key }) => {
    if (key === 'logout') {
      clearAuth();
      navigate('/login');
    } else if (key === 'profile') {
      navigate('/profile');
    } else if (key === 'password') {
      navigate('/settings/password');
    } else if (key === 'devices') {
      navigate('/settings/devices');
    } else if (key === 'settings') {
      navigate('/settings');
    }
  };

  // 面包屑导航
  const getBreadcrumbs = () => {
    const pathSnippets = location.pathname.split('/').filter((i) => i);
    const breadcrumbItems = pathSnippets.map((_, index) => {
      const url = `/${pathSnippets.slice(0, index + 1).join('/')}`;
      return {
        title: <Link to={url}>{pathSnippets[index]}</Link>,
      };
    });

    return [{ title: <Link to="/">首页</Link> }, ...breadcrumbItems];
  };

  return (
    <Layout style={{ minHeight: '100vh' }}>
      {/* 侧边栏 */}
      <Sider trigger={null} collapsible collapsed={collapsed} theme="dark">
        <div
          style={{
            height: 64,
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center',
            color: '#fff',
            fontSize: collapsed ? 16 : 20,
            fontWeight: 'bold',
          }}
        >
          {collapsed ? 'GM' : 'DNF GM Tools'}
        </div>
        <Menu
          theme="dark"
          mode="inline"
          selectedKeys={[location.pathname]}
          items={menuItems}
          onClick={handleMenuClick}
        />
      </Sider>

      {/* 右侧内容区 */}
      <Layout>
        {/* 顶部导航栏 */}
        <Header style={{ padding: '0 24px', background: '#fff', display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
          <Button
            type="text"
            icon={collapsed ? <MenuUnfoldOutlined /> : <MenuFoldOutlined />}
            onClick={() => setCollapsed(!collapsed)}
            style={{ fontSize: 16, width: 64, height: 64 }}
          />

          <Dropdown menu={{ items: userMenuItems, onClick: handleUserMenuClick }} placement="bottomRight">
            <Space style={{ cursor: 'pointer' }}>
              <Avatar icon={<UserOutlined />} />
              <span>{user?.username || '未登录'}</span>
            </Space>
          </Dropdown>
        </Header>

        {/* 面包屑 */}
        <div style={{ padding: '16px 24px', background: '#fff', borderBottom: '1px solid #f0f0f0' }}>
          <Breadcrumb items={getBreadcrumbs()} />
        </div>

        {/* 主内容区 */}
        <Content style={{ margin: '24px', padding: 24, background: '#fff', minHeight: 280 }}>
          <Outlet />
        </Content>
      </Layout>
    </Layout>
  );
};

export default MainLayout;
