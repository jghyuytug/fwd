#include "regeditdialog.h"
#include "ui_regeditdialog.h"
#include "aes_cbc.h"
#include "httpaesrequest.h"
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

regeditDialog::regeditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::regeditDialog)
{
    ui->setupUi(this);
}

regeditDialog::~regeditDialog()
{
    delete ui;
}

void regeditDialog::on_pushButton_clicked()
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

    postData.append("regedit|");
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

void regeditDialog::requestFinished(QNetworkReply *reply)
{
    AES_CBC aes;

    QByteArray rawResponse = reply->readAll();
    reply->deleteLater();

    qDebug() << "=============== 注册响应调试 ===============";
    qDebug() << "原始响应(hex):" << rawResponse.toHex();
    qDebug() << "原始响应(base64):" << rawResponse.toBase64();
    qDebug() << "原始响应长度:" << rawResponse.size();

    // 解密为QByteArray
    QByteArray decrypted = aes.decrypt(rawResponse);

    qDebug() << "解密后(hex):" << decrypted.toHex();
    qDebug() << "解密后长度:" << decrypted.length();

    // 截取到第一个NULL字节前的内容
    int nullPos = decrypted.indexOf('\0');
    QByteArray cleanBytes = (nullPos >= 0) ? decrypted.left(nullPos) : decrypted;

    // 转换为QString并去除空白
    QString cleanResponse = QString::fromUtf8(cleanBytes).trimmed();

    qDebug() << "清理后响应:" << cleanResponse;
    qDebug() << "=======================================";

    // 写入调试日志文件
    QFile logFile("C:/Users/aaq/Desktop/dnfLogin-master/register_debug.log");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream log(&logFile);
        log << "\n=============== 注册响应调试 " << QDateTime::currentDateTime().toString() << " ===============\n";
        log << "原始响应(hex): " << rawResponse.toHex() << "\n";
        log << "原始响应(base64): " << rawResponse.toBase64() << "\n";
        log << "原始响应长度: " << rawResponse.size() << "\n";
        log << "原始响应(QString): " << QString(rawResponse) << "\n";
        log << "解密后(hex): " << decrypted.toHex() << "\n";
        log << "解密后长度: " << decrypted.length() << "\n";
        log << "NULL位置: " << nullPos << "\n";
        log << "清理后(hex): " << cleanBytes.toHex() << "\n";
        log << "清理后响应: " << cleanResponse << "\n";
        log << "=======================================\n";
        logFile.close();
    }

    if(cleanResponse == "success"){
        QMessageBox::about(this,tr("success"), tr("恭喜！注册成功。"));
    }else if(cleanResponse == "repeat"){
        QMessageBox::warning(this,tr("Warning"), tr("账号已被使用！"), QMessageBox::Ok);
    }else{
        QMessageBox::warning(this,tr("Warning"), tr("注册失败！实际响应: ") + cleanResponse, QMessageBox::Ok);
    }
}
