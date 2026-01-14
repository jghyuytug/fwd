/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <checkqlineedit.h>
#include <clickablelabel.h>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QPushButton *btn_login;
    CheckQLineEdit *text_user;
    QCheckBox *checkBox;
    ClickableLabel *lab_regedit;
    ClickableLabel *lab_reset;
    CheckQLineEdit *text_passwd;
    QLabel *lab_I_1;
    QLabel *label_3;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(260, 172);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Widget->sizePolicy().hasHeightForWidth());
        Widget->setSizePolicy(sizePolicy);
        Widget->setMinimumSize(QSize(260, 172));
        Widget->setMaximumSize(QSize(260, 172));
        QIcon icon;
        icon.addFile(QString::fromUtf8("DNF.ico"), QSize(), QIcon::Normal, QIcon::Off);
        Widget->setWindowIcon(icon);
        btn_login = new QPushButton(Widget);
        btn_login->setObjectName("btn_login");
        btn_login->setGeometry(QRect(50, 130, 151, 31));
        text_user = new CheckQLineEdit(Widget);
        text_user->setObjectName("text_user");
        text_user->setGeometry(QRect(20, 20, 221, 30));
        QFont font;
        font.setPointSize(15);
        text_user->setFont(font);
        checkBox = new QCheckBox(Widget);
        checkBox->setObjectName("checkBox");
        checkBox->setGeometry(QRect(20, 100, 111, 16));
        lab_regedit = new ClickableLabel(Widget);
        lab_regedit->setObjectName("lab_regedit");
        lab_regedit->setGeometry(QRect(110, 100, 54, 16));
        lab_reset = new ClickableLabel(Widget);
        lab_reset->setObjectName("lab_reset");
        lab_reset->setGeometry(QRect(190, 100, 54, 16));
        text_passwd = new CheckQLineEdit(Widget);
        text_passwd->setObjectName("text_passwd");
        text_passwd->setGeometry(QRect(20, 60, 221, 30));
        text_passwd->setFont(font);
        text_passwd->setEchoMode(QLineEdit::Password);
        lab_I_1 = new QLabel(Widget);
        lab_I_1->setObjectName("lab_I_1");
        lab_I_1->setGeometry(QRect(95, 100, 16, 16));
        label_3 = new QLabel(Widget);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(170, 100, 16, 16));
        sizePolicy.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy);

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "DNF\347\231\273\345\275\225\345\231\250", nullptr));
        btn_login->setText(QCoreApplication::translate("Widget", "\347\231\273\345\275\225", nullptr));
        checkBox->setText(QCoreApplication::translate("Widget", "\350\256\260\344\275\217\345\257\206\347\240\201", nullptr));
        lab_regedit->setText(QCoreApplication::translate("Widget", "\350\264\246\345\217\267\346\263\250\345\206\214", nullptr));
        lab_reset->setText(QCoreApplication::translate("Widget", "\344\277\256\346\224\271\345\257\206\347\240\201", nullptr));
        lab_I_1->setText(QCoreApplication::translate("Widget", "|", nullptr));
        label_3->setText(QCoreApplication::translate("Widget", "|", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
