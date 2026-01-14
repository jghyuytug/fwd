/**
 * 页面标题组件
 */

import { FC, ReactNode } from 'react';
import { PageHeader as AntPageHeader } from '@ant-design/pro-components';
import { useNavigate } from 'react-router-dom';

interface PageHeaderProps {
  title: string;
  subTitle?: string;
  extra?: ReactNode;
  onBack?: () => void;
}

const PageHeader: FC<PageHeaderProps> = ({ title, subTitle, extra, onBack }) => {
  const navigate = useNavigate();

  const handleBack = () => {
    if (onBack) {
      onBack();
    } else {
      navigate(-1);
    }
  };

  return (
    <AntPageHeader
      title={title}
      subTitle={subTitle}
      extra={extra}
      onBack={onBack !== undefined || history.length > 1 ? handleBack : undefined}
      style={{ marginBottom: 16, background: '#fff', padding: '16px 24px' }}
    />
  );
};

export default PageHeader;
