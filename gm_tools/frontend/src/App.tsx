import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom';
import { QueryClient, QueryClientProvider } from '@tanstack/react-query';
import { ErrorBoundary } from '@/components/common';
import { MainLayout, BlankLayout } from '@/components/layout';
import PrivateRoute from '@/components/common/PrivateRoute';
import { useTokenExpiration } from '@/hooks/useTokenExpiration';

// Pages
import Login from '@/pages/Login';
import Dashboard from '@/pages/Dashboard';
import NotFound from '@/pages/NotFound';
import Forbidden from '@/pages/Forbidden';
import ChangePassword from '@/pages/ChangePassword';
import TrustedDevices from '@/pages/TrustedDevices';
import PlayerSearch from '@/pages/PlayerSearch';
import PlayerDetail from '@/pages/PlayerDetail';

// Create a client for react-query
const queryClient = new QueryClient({
  defaultOptions: {
    queries: {
      refetchOnWindowFocus: false,
      retry: 1,
      staleTime: 5 * 60 * 1000, // 5 minutes
    },
  },
});

function AppContent() {
  useTokenExpiration();

  return (
    <Routes>
            {/* Public routes with BlankLayout */}
            <Route element={<BlankLayout />}>
              <Route path="/login" element={<Login />} />
              <Route path="/403" element={<Forbidden />} />
            </Route>

            {/* Private routes with MainLayout */}
            <Route
              element={
                <PrivateRoute>
                  <MainLayout />
                </PrivateRoute>
              }
            >
              {/* Dashboard */}
              <Route path="/" element={<Navigate to="/dashboard" replace />} />
              <Route path="/dashboard" element={<Dashboard />} />

              {/* Player Management */}
              <Route
                path="/player"
                element={
                  <PrivateRoute requiredPermission="player.view">
                    <PlayerSearch />
                  </PrivateRoute>
                }
              />
              <Route
                path="/player/:id"
                element={
                  <PrivateRoute requiredPermission="player.view">
                    <PlayerDetail />
                  </PrivateRoute>
                }
              />

              {/* GM Operations */}
              <Route
                path="/gm/item-grant"
                element={
                  <PrivateRoute requiredPermission="item.grant">
                    <div>道具发放页面（待实现 - Phase 6）</div>
                  </PrivateRoute>
                }
              />
              <Route
                path="/gm/account-manage"
                element={
                  <PrivateRoute
                    requiredPermissions={['account.ban', 'account.unban', 'account.edit']}
                  >
                    <div>账号管理页面（待实现 - Phase 6）</div>
                  </PrivateRoute>
                }
              />

              {/* Audit Logs */}
              <Route
                path="/audit/operation"
                element={
                  <PrivateRoute requiredPermission="audit.view">
                    <div>操作日志页面（待实现 - Phase 7）</div>
                  </PrivateRoute>
                }
              />
              <Route
                path="/audit/login"
                element={
                  <PrivateRoute requiredPermission="audit.view">
                    <div>登录日志页面（待实现 - Phase 7）</div>
                  </PrivateRoute>
                }
              />

              {/* System Management */}
              <Route
                path="/system/permissions"
                element={
                  <PrivateRoute requiredPermission="system.manage">
                    <div>权限管理页面（待实现 - Phase 8）</div>
                  </PrivateRoute>
                }
              />
              <Route
                path="/system/roles"
                element={
                  <PrivateRoute requiredPermission="system.manage">
                    <div>角色管理页面（待实现 - Phase 8）</div>
                  </PrivateRoute>
                }
              />

              {/* User Settings */}
              <Route path="/settings/password" element={<ChangePassword />} />
              <Route path="/settings/devices" element={<TrustedDevices />} />
            </Route>

            {/* 404 page */}
            <Route path="*" element={<NotFound />} />
          </Routes>
  );
}

function App() {
  return (
    <ErrorBoundary>
      <QueryClientProvider client={queryClient}>
        <BrowserRouter>
          <AppContent />
        </BrowserRouter>
      </QueryClientProvider>
    </ErrorBoundary>
  );
}

export default App;
