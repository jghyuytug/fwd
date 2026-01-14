#include "windows.h"
#include "widget.h"
#include <QApplication>
#include <QMessageBox>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Qt 5.15+ 不再需要手动设置编码
    // QTextCodec相关函数已被移除
    
    try {
        qDebug() << "正在创建主窗口...";
        Widget w;
        
        qDebug() << "正在显示主窗口...";
        w.show();

        qDebug() << "进入事件循环...";
        return a.exec();
    } catch (const std::exception& e) {
        qDebug() << "异常:" << e.what();
        QMessageBox::critical(nullptr, "错误", QString("程序发生异常: %1").arg(e.what()));
        return -1;
    } catch (...) {
        qDebug() << "未知异常";
        QMessageBox::critical(nullptr, "错误", "程序发生未知异常");
        return -1;
    }
}
