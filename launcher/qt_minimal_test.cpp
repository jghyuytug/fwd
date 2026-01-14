#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout;
    QLabel *label = new QLabel("Qt Test - 如果你看到这个，Qt环境正常");
    
    layout->addWidget(label);
    window.setLayout(layout);
    window.setWindowTitle("Qt环境测试");
    window.resize(300, 100);
    window.show();
    
    return app.exec();
}