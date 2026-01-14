/********************************************************************************
** Form generated from reading UI file 'forgetdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORGETDIALOG_H
#define UI_FORGETDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <checkqlineedit.h>

QT_BEGIN_NAMESPACE

class Ui_forgetDialog
{
public:
    CheckQLineEdit *text_passwd2;
    QLabel *label_2;
    QLabel *label_4;
    CheckQLineEdit *text_username;
    QLabel *label;
    QLabel *label_3;
    CheckQLineEdit *text_qq;
    QPushButton *pushButton;
    CheckQLineEdit *text_passwd1;

    void setupUi(QDialog *forgetDialog)
    {
        if (forgetDialog->objectName().isEmpty())
            forgetDialog->setObjectName("forgetDialog");
        forgetDialog->resize(340, 200);
        text_passwd2 = new CheckQLineEdit(forgetDialog);
        text_passwd2->setObjectName("text_passwd2");
        text_passwd2->setGeometry(QRect(110, 120, 151, 20));
        text_passwd2->setEchoMode(QLineEdit::Password);
        label_2 = new QLabel(forgetDialog);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(60, 90, 51, 21));
        label_4 = new QLabel(forgetDialog);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(50, 60, 54, 21));
        text_username = new CheckQLineEdit(forgetDialog);
        text_username->setObjectName("text_username");
        text_username->setGeometry(QRect(110, 30, 151, 20));
        label = new QLabel(forgetDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(50, 30, 54, 21));
        label_3 = new QLabel(forgetDialog);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(40, 120, 54, 21));
        text_qq = new CheckQLineEdit(forgetDialog);
        text_qq->setObjectName("text_qq");
        text_qq->setGeometry(QRect(110, 60, 151, 20));
        text_qq->setEchoMode(QLineEdit::Password);
        pushButton = new QPushButton(forgetDialog);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(120, 150, 111, 31));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy);
        text_passwd1 = new CheckQLineEdit(forgetDialog);
        text_passwd1->setObjectName("text_passwd1");
        text_passwd1->setGeometry(QRect(110, 90, 151, 20));
        text_passwd1->setEchoMode(QLineEdit::Password);

        retranslateUi(forgetDialog);

        QMetaObject::connectSlotsByName(forgetDialog);
    } // setupUi

    void retranslateUi(QDialog *forgetDialog)
    {
        forgetDialog->setWindowTitle(QCoreApplication::translate("forgetDialog", "\345\277\230\350\256\260\345\257\206\347\240\201", nullptr));
        label_2->setText(QCoreApplication::translate("forgetDialog", "\345\257\206\347\240\201\357\274\232", nullptr));
        label_4->setText(QCoreApplication::translate("forgetDialog", "\345\256\211\345\205\250\347\240\201\357\274\232", nullptr));
        label->setText(QCoreApplication::translate("forgetDialog", "\347\224\250\346\210\267\345\220\215\357\274\232", nullptr));
        label_3->setText(QCoreApplication::translate("forgetDialog", "\347\241\256\350\256\244\345\257\206\347\240\201\357\274\232", nullptr));
        pushButton->setText(QCoreApplication::translate("forgetDialog", "\351\207\215\347\275\256\345\257\206\347\240\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class forgetDialog: public Ui_forgetDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORGETDIALOG_H
