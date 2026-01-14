#include <QApplication>
#include <QMessageBox>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QMessageBox::information(nullptr, "Test", "Qt Test Program Started!");
    
    return 0;
}