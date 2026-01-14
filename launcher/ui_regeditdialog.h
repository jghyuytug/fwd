/********************************************************************************
** Form generated from reading UI file 'regeditdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGEDITDIALOG_H
#define UI_REGEDITDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <checkqlineedit.h>

QT_BEGIN_NAMESPACE

class Ui_regeditDialog
{
public:
    CheckQLineEdit *text_username;
    CheckQLineEdit *text_passwd1;
    CheckQLineEdit *text_passwd2;
    CheckQLineEdit *text_qq;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QPushButton *pushButton;

    void setupUi(QDialog *regeditDialog)
    {
        if (regeditDialog->objectName().isEmpty())
            regeditDialog->setObjectName("regeditDialog");
        regeditDialog->resize(340, 200);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(regeditDialog->sizePolicy().hasHeightForWidth());
        regeditDialog->setSizePolicy(sizePolicy);
        regeditDialog->setMinimumSize(QSize(340, 200));
        regeditDialog->setMaximumSize(QSize(340, 200));
        text_username = new CheckQLineEdit(regeditDialog);
        text_username->setObjectName("text_username");
        text_username->setGeometry(QRect(110, 30, 151, 20));
        text_passwd1 = new CheckQLineEdit(regeditDialog);
        text_passwd1->setObjectName("text_passwd1");
        text_passwd1->setGeometry(QRect(110, 60, 151, 20));
        text_passwd1->setEchoMode(QLineEdit::Password);
        text_passwd2 = new CheckQLineEdit(regeditDialog);
        text_passwd2->setObjectName("text_passwd2");
        text_passwd2->setGeometry(QRect(110, 90, 151, 20));
        text_passwd2->setEchoMode(QLineEdit::Password);
        text_qq = new CheckQLineEdit(regeditDialog);
        text_qq->setObjectName("text_qq");
        text_qq->setGeometry(QRect(110, 120, 151, 20));
        QFont font;
        font.setPointSize(9);
        text_qq->setFont(font);
        text_qq->setEchoMode(QLineEdit::Password);
        label = new QLabel(regeditDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(50, 30, 54, 21));
        label_2 = new QLabel(regeditDialog);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(60, 60, 54, 21));
        label_3 = new QLabel(regeditDialog);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(40, 90, 54, 21));
        label_4 = new QLabel(regeditDialog);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(50, 120, 54, 21));
        pushButton = new QPushButton(regeditDialog);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(110, 150, 111, 31));
        sizePolicy.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy);

        retranslateUi(regeditDialog);

        QMetaObject::connectSlotsByName(regeditDialog);
    } // setupUi

    void retranslateUi(QDialog *regeditDialog)
    {
        regeditDialog->setWindowTitle(QCoreApplication::translate("regeditDialog", "\350\264\246\345\217\267\346\263\250\345\206\214", nullptr));
        label->setText(QCoreApplication::translate("regeditDialog", "\347\224\250\346\210\267\345\220\215\357\274\232", nullptr));
        label_2->setText(QCoreApplication::translate("regeditDialog", "\345\257\206\347\240\201\357\274\232", nullptr));
        label_3->setText(QCoreApplication::translate("regeditDialog", "\347\241\256\350\256\244\345\257\206\347\240\201\357\274\232", nullptr));
        label_4->setText(QCoreApplication::translate("regeditDialog", "\345\256\211\345\205\250\347\240\201\357\274\232", nullptr));
        pushButton->setText(QCoreApplication::translate("regeditDialog", "\346\263\250\345\206\214", nullptr));
    } // retranslateUi

};

namespace Ui {
    class regeditDialog: public Ui_regeditDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGEDITDIALOG_H
