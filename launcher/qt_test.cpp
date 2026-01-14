#include <QApplication>
#include <QMessageBox>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 显示Qt版本信息
    qDebug() << "Qt Version:" << QT_VERSION_STR;
    qDebug() << "Application started successfully";
    
    // 显示消息框测试
    QMessageBox::information(nullptr, "DNF Login Test", 
        QString("Qt Version: %1\nTest Successful!").arg(QT_VERSION_STR));
    
    return 0;
}