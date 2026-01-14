#include "repasswdDialog.h"
#include "ui_repasswdDialog.h"
#include "httpaesrequest.h"
#include "aes_cbc.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include <QDebug>

repasswdDialog::repasswdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::repasswdDialog)
{
    ui->setupUi(this);
    connect(ui->lab_forget, SIGNAL(clicked()), this, SLOT(lab_forget_clicked()));
}

repasswdDialog::~repasswdDialog()
{
    delete ui;
}

void repasswdDialog::on_btn_repasswd_clicked()
{
    QByteArray postData;

    if(ui->text_username->text().size() < 3
            || ui->text_passwd_old->text().size() < 6
            || ui->text_passwd_new->text().size() < 6){
        QMessageBox::warning(this,tr("Error"),tr("用户名>=3位、密码>=6位"),QMessageBox::Ok);
        return;
    }

    if(ui->text_passwd_new->text() != ui->text_passwd_new2->text()) {
        QMessageBox::warning(this,tr("Error"),tr("两次输入的密码不一样"),QMessageBox::Ok);
        return;
    }

    if(!ui->text_passwd_new->isValidity()
            || !ui->text_passwd_old->isValidity()
            || !ui->text_username->isValidity()) {
        QMessageBox::warning(this,tr("Warning"), tr("用户名或密码不合法，只能包含a-z、A-Z、0-9"), QMessageBox::Ok);
        return;
    }

    postData.append("repasswd|");
    postData.append(ui->text_username->text().toUtf8());
    postData.append("|");
    postData.append(QCryptographicHash::hash (
                        ui->text_passwd_old->text().toLatin1(),
                        QCryptographicHash::Md5).toHex());

    postData.append("|");
    postData.append((QCryptographicHash::hash (
                         ui->text_passwd_new->text().toLatin1(),
                         QCryptographicHash::Md5).toHex()));

    HttpAesRequest *loginRequest = new HttpAesRequest();

    connect(loginRequest,SIGNAL(finished(QNetworkReply*)),
            this,SLOT(requestFinished(QNetworkReply*)));

    loginRequest->sendPOST(postData);
}

void repasswdDialog::requestFinished(QNetworkReply *reply)
{
    AES_CBC aes;

    QByteArray rawResponse = reply->readAll();
    reply->deleteLater();

    qDebug() << "密码修改响应(hex):" << rawResponse.toHex();

    // 解密为QByteArray
    QByteArray decrypted = aes.decrypt(rawResponse);

    // 截取到第一个NULL字节前的内容
    int nullPos = decrypted.indexOf('\0');
    QByteArray cleanBytes = (nullPos >= 0) ? decrypted.left(nullPos) : decrypted;

    // 转换为QString并去除空白
    QString response = QString::fromUtf8(cleanBytes).trimmed();

    qDebug() << "密码修改响应:" << response;

    if(response == "success"){
        QMessageBox::about(this,tr("success"), tr("修改成功"));
    }else if(response == "passworderror"){
        QMessageBox::warning(this,tr("Warning"), tr("旧密码错误！"), QMessageBox::Ok);
    }else{
        QMessageBox::warning(this,tr("Warning"), tr("修改失败！"), QMessageBox::Ok);
    }
}

void repasswdDialog::lab_forget_clicked()
{
    fgDialog = new forgetDialog();
    fgDialog->show();
    this->close();
}
