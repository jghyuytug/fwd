/********************************************************************************
** Form generated from reading UI file 'repasswddialog.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REPASSWDDIALOG_H
#define UI_REPASSWDDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <checkqlineedit.h>
#include <clickablelabel.h>

QT_BEGIN_NAMESPACE

class Ui_repasswdDialog
{
public:
    CheckQLineEdit *text_username;
    CheckQLineEdit *text_passwd_old;
    CheckQLineEdit *text_passwd_new;
    CheckQLineEdit *text_passwd_new2;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QPushButton *btn_repasswd;
    ClickableLabel *lab_forget;
    QLabel *label_5;

    void setupUi(QDialog *repasswdDialog)
    {
        if (repasswdDialog->objectName().isEmpty())
            repasswdDialog->setObjectName("repasswdDialog");
        repasswdDialog->resize(340, 200);
        repasswdDialog->setMinimumSize(QSize(340, 200));
        repasswdDialog->setMaximumSize(QSize(340, 200));
        text_username = new CheckQLineEdit(repasswdDialog);
        text_username->setObjectName("text_username");
        text_username->setGeometry(QRect(120, 20, 151, 20));
        text_passwd_old = new CheckQLineEdit(repasswdDialog);
        text_passwd_old->setObjectName("text_passwd_old");
        text_passwd_old->setGeometry(QRect(120, 50, 151, 20));
        text_passwd_old->setEchoMode(QLineEdit::Password);
        text_passwd_new = new CheckQLineEdit(repasswdDialog);
        text_passwd_new->setObjectName("text_passwd_new");
        text_passwd_new->setGeometry(QRect(120, 80, 151, 20));
        text_passwd_new->setEchoMode(QLineEdit::Password);
        text_passwd_new2 = new CheckQLineEdit(repasswdDialog);
        text_passwd_new2->setObjectName("text_passwd_new2");
        text_passwd_new2->setGeometry(QRect(120, 110, 151, 20));
        text_passwd_new2->setEchoMode(QLineEdit::Password);
        label = new QLabel(repasswdDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(60, 20, 54, 21));
        label_2 = new QLabel(repasswdDialog);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(60, 50, 54, 21));
        label_3 = new QLabel(repasswdDialog);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(60, 80, 54, 21));
        label_4 = new QLabel(repasswdDialog);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(50, 110, 54, 21));
        btn_repasswd = new QPushButton(repasswdDialog);
        btn_repasswd->setObjectName("btn_repasswd");
        btn_repasswd->setGeometry(QRect(140, 140, 71, 31));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(btn_repasswd->sizePolicy().hasHeightForWidth());
        btn_repasswd->setSizePolicy(sizePolicy);
        lab_forget = new ClickableLabel(repasswdDialog);
        lab_forget->setObjectName("lab_forget");
        lab_forget->setGeometry(QRect(260, 180, 54, 16));
        QFont font;
        font.setBold(true);
        font.setUnderline(true);
        lab_forget->setFont(font);
        label_5 = new QLabel(repasswdDialog);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(30, 180, 230, 16));

        retranslateUi(repasswdDialog);

        QMetaObject::connectSlotsByName(repasswdDialog);
    } // setupUi

    void retranslateUi(QDialog *repasswdDialog)
    {
        repasswdDialog->setWindowTitle(QCoreApplication::translate("repasswdDialog", "Dialog", nullptr));
        text_username->setText(QString());
        text_passwd_old->setText(QString());
        text_passwd_new->setText(QString());
        text_passwd_new2->setText(QString());
        label->setText(QCoreApplication::translate("repasswdDialog", "\347\224\250\346\210\267\345\220\215\357\274\232", nullptr));
        label_2->setText(QCoreApplication::translate("repasswdDialog", "\346\227\247\345\257\206\347\240\201\357\274\232", nullptr));
        label_3->setText(QCoreApplication::translate("repasswdDialog", "\346\226\260\345\257\206\347\240\201\357\274\232", nullptr));
        label_4->setText(QCoreApplication::translate("repasswdDialog", "\347\241\256\350\256\244\345\257\206\347\240\201\357\274\232", nullptr));
        btn_repasswd->setText(QCoreApplication::translate("repasswdDialog", "\344\277\256\346\224\271\345\257\206\347\240\201", nullptr));
        lab_forget->setText(QCoreApplication::translate("repasswdDialog", "\345\277\230\350\256\260\345\257\206\347\240\201", nullptr));
        label_5->setText(QCoreApplication::translate("repasswdDialog", "\345\246\202\346\236\234\345\277\230\350\256\260\345\257\206\347\240\201\350\257\267\347\202\271\345\207\273\345\217\263\344\276\247\351\200\232\350\277\207\345\256\211\345\205\250\347\240\201\346\211\276\345\233\236\357\274\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class repasswdDialog: public Ui_repasswdDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REPASSWDDIALOG_H
