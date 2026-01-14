#include "forgetdialog.h"
#include "ui_forgetdialog.h"
#include "aes_cbc.h"
#include "httpaesrequest.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include <QDebug>

forgetDialog::forgetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::forgetDialog)
{
    ui->setupUi(this);
}

forgetDialog::~forgetDialog()
{
    delete ui;
}

void forgetDialog::on_pushButton_clicked()
{
    QByteArray postData;

    if(ui->text_username->text().size() < 3
            || ui->text_passwd1->text().size() < 6
            || ui->text_qq->text().size() < 5){
        QMessageBox::warning(this,tr("Error"),tr("用户名>=3位、密码>=6位、QQ号>=5位"),QMessageBox::Ok);
        return;
    }

    if(ui->text_passwd1->text() != ui->text_passwd2->text()) {
        QMessageBox::warning(this,tr("Error"),tr("两次输入的密码不一样"),QMessageBox::Ok);
        return;
    }

    if(!ui->text_passwd1->isValidity()
            || !ui->text_qq->isValidity()
            || !ui->text_username->isValidity()) {
        QMessageBox::warning(this,tr("Warning"), tr("用户名或密码不合法，只能包含a-z、A-Z、0-9"), QMessageBox::Ok);
        return;
    }

    postData.append("forget|");
    postData.append(ui->text_username->text().toUtf8());
    postData.append("|");
    postData.append(QCryptographicHash::hash (
                        ui->text_passwd1->text().toLatin1(),
                        QCryptographicHash::Md5).toHex());
    postData.append("|");
    postData.append(ui->text_qq->text().toUtf8());

    HttpAesRequest *loginRequest = new HttpAesRequest();

    connect(loginRequest,SIGNAL(finished(QNetworkReply*)),
            this,SLOT(requestFinished(QNetworkReply*)));

    loginRequest->sendPOST(postData);
}

void forgetDialog::requestFinished(QNetworkReply *reply)
{
    AES_CBC aes;

    QByteArray rawResponse = reply->readAll();
    reply->deleteLater();

    qDebug() << "密码找回响应(hex):" << rawResponse.toHex();

    // 解密为QByteArray
    QByteArray decrypted = aes.decrypt(rawResponse);

    // 截取到第一个NULL字节前的内容
    int nullPos = decrypted.indexOf('\0');
    QByteArray cleanBytes = (nullPos >= 0) ? decrypted.left(nullPos) : decrypted;

    // 转换为QString并去除空白
    QString response = QString::fromUtf8(cleanBytes).trimmed();

    qDebug() << "密码找回响应:" << response;

    if(response == "success") {
        QMessageBox::about(this,tr("success"), tr("重置成功"));
        this->close();
    }else{
        QMessageBox::warning(this,tr("Warning"), tr("重置失败，请检查用户名和QQ号是否正确。"), QMessageBox::Ok);
    }
}
